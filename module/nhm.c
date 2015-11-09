// 'nhm' netfilter hooks


#include <linux/version.h>	/* Needed for KERNEL_VERSION + LINUX_VERSION_CODE */
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#include <linux/slab.h>           // Needed by kmalloc
#include <linux/string.h>         // Needed by strings operations
#include <linux/list.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <nhm.h>

struct nhm_list_s {
    struct nhm_s entry;
    struct list_head list;
};


#define DRIVER_VERSION "1.0"
#define DRIVER_AUTHOR  "Keidan (Kevin Billonneau)"
#define DRIVER_DESC    "Netfilter Hook Module"
#define DRIVER_LICENSE "GPL"

static int                    number_opens = 0;             ///< Counts the number of times the device is opened
static struct class*          nhm_class  = NULL;            ///< The device-driver class struct pointer
static struct device*         nhm_device = NULL;            ///< The device-driver device struct pointer
static struct nf_hook_ops     nfho;
static struct mutex           nhm_mutex;
static struct list_head       nhm_entries;
static struct list_head*      nhm_entries_index = NULL;
static unsigned int           nhm_entries_length;
static nhm_nf_type_te         nhm_nf_type = NHM_NF_TYPE_ACCEPT;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
static spinlock_t nhm_entries_lock;
#define raw_spin_lock spin_lock
#define raw_spin_unlock spin_unlock
#else
static DEFINE_RAW_SPINLOCK(nhm_entries_lock);
#endif

static int     nhm_dev_open(struct inode *, struct file *);
static int     nhm_dev_release(struct inode *, struct file *);
static ssize_t nhm_dev_read(struct file *, char *, size_t, loff_t *);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int     nhm_dev_ioctl(struct inode *i, struct file *file, unsigned int cmd, unsigned long arg);
#else
static long    nhm_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif
static void    nhm_hook_start(void);
static void    nhm_hook_stop(void);
static void    nhm_list_clear(void);

static struct file_operations fops = {
  .open = nhm_dev_open,
  .read = nhm_dev_read,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
  .ioctl = nhm_dev_ioctl,
#else
  .unlocked_ioctl = nhm_dev_ioctl,
#endif
  .release = nhm_dev_release,
};
 
/** 
 *  @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init nhm_init(void){
  int major_number;
  printk(KERN_INFO "NHM: Initializing the NHM LKM.\n");
 
  // Try to dynamically allocate a major number for the device -- more difficult but worth it
  major_number = register_chrdev(NHM_MAJOR_NUMBER, NHM_DEVICE_NAME, &fops);
  if (major_number < 0){
    printk(KERN_ALERT "NHM failed to register a major number.\n");
    return major_number;
  }
  printk(KERN_INFO "NHM: registered correctly with major number %d\n", NHM_MAJOR_NUMBER);
 
  // Register the device class
  nhm_class = class_create(THIS_MODULE, NHM_CLASS_NAME);
  if (IS_ERR(nhm_class)){                // Check for error and clean up if there is
    unregister_chrdev(NHM_MAJOR_NUMBER, NHM_DEVICE_NAME);
    printk(KERN_ALERT "NHM: Failed to register device class.\n");
    return PTR_ERR(nhm_class);          // Correct way to return an error on a pointer
  }
  printk(KERN_INFO "NHM: device class registered correctly.\n");
 
  // Register the device driver
  nhm_device = device_create(nhm_class, NULL, MKDEV(NHM_MAJOR_NUMBER, 0), NULL, NHM_DEVICE_NAME);
  if (IS_ERR(nhm_device)){               // Clean up if there is an error
    class_destroy(nhm_class);           // Repeated code but the alternative is goto statements
    unregister_chrdev(NHM_MAJOR_NUMBER, NHM_DEVICE_NAME);
    printk(KERN_ALERT "NHM: Failed to create the device.\n");
    return PTR_ERR(nhm_device);
  }
  printk(KERN_INFO "NHM: device class created correctly.\n"); // Made it! device was initialized
  mutex_init(&nhm_mutex);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
  spin_lock_init(&nhm_entries_lock);
#endif
  INIT_LIST_HEAD(&nhm_entries);
  nhm_entries_length = 0;
  nhm_hook_start();
  return 0;
}
 
/** 
 *  @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit nhm_exit(void){
  printk(KERN_INFO "NHM: Unloading the NHM LKM.\n");
  nhm_hook_stop();
  nhm_list_clear();
  device_destroy(nhm_class, MKDEV(NHM_MAJOR_NUMBER, 0));     // remove the device
  class_unregister(nhm_class);                          // unregister the device class
  class_destroy(nhm_class);                             // remove the device class
  unregister_chrdev(NHM_MAJOR_NUMBER, NHM_DEVICE_NAME);             // unregister the major number
  printk(KERN_INFO "NHM: NHM LKM unloaded.\n");
}
 
static int nhm_dev_open(struct inode *inodep, struct file *filep){
  if(number_opens) {
    printk(KERN_ALERT "NHM: Device already opened.\n");
    return -EBUSY;
  }
  number_opens++;
  nhm_entries_index = &nhm_entries;
  return 0;
}

static int nhm_dev_release(struct inode *inodep, struct file *filep){
  number_opens--;
  return 0;
}

static void nhm_print_entry(const char* title, struct nhm_s *message) {
  unsigned char buffer1 [4], buffer2 [4];
  printk(KERN_INFO "[NHM] %s SrcHWaddr: %02x:%02x:%02x:%02x:%02x:%02x,  DestHWaddr: %02x:%02x:%02x:%02x:%02x:%02x\n", title,
	 message->s_hw[0], message->s_hw[1], message->s_hw[2], message->s_hw[3], message->s_hw[4], message->s_hw[5],
	 message->d_hw[0], message->d_hw[1], message->d_hw[2], message->d_hw[3], message->d_hw[4], message->d_hw[5]);
  nhm_from_ip(buffer1, 0, message->s_ip4);
  nhm_from_ip(buffer2, 0, message->d_ip4);
  printk(KERN_INFO "[NHM] Src: %d.%d.%d.%d:[%d-%d], Dest: %d.%d.%d.%d:[%d-%d]\n", buffer1[3], buffer1[2], buffer1[1], buffer1[0], message->s_port[0], message->s_port[1],
	 buffer2[3], buffer2[2], buffer2[1], buffer2[0], message->d_port[0], message->d_port[1]);
  printk(KERN_INFO "[NHM] Proto: eth-%d, ip-%d\n", message->eth_proto, message->ip_proto);
}

static ssize_t nhm_dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
  int error_count = 0, size = NHM_LENGTH;
  struct nhm_list_s *tmp;
  mutex_lock(&nhm_mutex);
  if(nhm_entries_index == &nhm_entries) nhm_entries_index = nhm_entries_index->next;
  if(!nhm_entries_index) {
    mutex_unlock(&nhm_mutex);
    printk(KERN_ALERT "NHM: Invalid list pointer\n");
    return -EFAULT; 
  }
  tmp = list_entry(nhm_entries_index, struct nhm_list_s, list);
  raw_spin_lock(&nhm_entries_lock);
  nhm_entries_index = nhm_entries_index->next;
  raw_spin_unlock(&nhm_entries_lock);

  error_count = copy_to_user(buffer, &tmp->entry, size);
  if (error_count){
    mutex_unlock(&nhm_mutex);
    printk(KERN_ALERT "[NHM] The copy to user failed: %d\n", error_count);
    return -EFAULT;
  }
  mutex_unlock(&nhm_mutex);
  return size;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int nhm_dev_ioctl(struct inode *i, struct file *file, unsigned int cmd, unsigned long arg) {
#else
static long nhm_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
#endif
  int err = 0;
  char found = 0;
  struct list_head *ptr, *next;
  struct nhm_list_s *new, *tmp;
  struct nhm_s message;
  char* user_buffer = (char*)arg;
  mutex_lock(&nhm_mutex);
  switch(cmd) {
    case NHM_IOCTL_ADD: {
      /* copy for user space */
      if(copy_from_user(&message, user_buffer, NHM_LENGTH)) {
	printk(KERN_ALERT "[NHM] The copy from user failed\n");
	err = -EIO;
      } else {
	found = 0;
	nhm_print_entry("Add new rule", &message);
	/* sanity check */
        raw_spin_lock(&nhm_entries_lock);
	list_for_each_safe(ptr, next, &nhm_entries) {
	  tmp = list_entry(ptr, struct nhm_list_s, list);
	  if(nhm_is_same(&message, &tmp->entry)) {
	    found=1;
	    break;
	  }
	}
	if(!found) {
	  new = kmalloc(sizeof(struct nhm_list_s), GFP_KERNEL);
	  if(!new) {
	    printk(KERN_ALERT "NHM: not enough memory.\n");
	    err = -ENOMEM; 
	  } else {
	    /* add new entry */
	    memcpy(&new->entry, &message, NHM_LENGTH);
	    memset(&new->list, 0, sizeof(struct list_head));
	    //INIT_LIST_HEAD(&new->list);
	    nhm_entries_length++;
	    list_add_tail(&new->list, &nhm_entries);
	  }
	}
        raw_spin_unlock(&nhm_entries_lock);
      }
      break;
    }
    case NHM_IOCTL_DEL: {
      /* copy for user space */
      if(copy_from_user(&message, user_buffer, NHM_LENGTH)) {
	printk(KERN_ALERT "[NHM] The copy from user failed\n");
	err = -EIO;
      } else {
	raw_spin_lock(&nhm_entries_lock);
	list_for_each_safe(ptr, next, &nhm_entries) {
	  tmp = list_entry(ptr, struct nhm_list_s, list);
	  if(nhm_is_same(&message, &tmp->entry)) {
	    nhm_entries_length--;
	    nhm_print_entry("Remove rule", &tmp->entry);
	    list_del(ptr);
	    kfree(tmp);
	    break;
	  }
	}
	raw_spin_unlock(&nhm_entries_lock);
      }
      break;
    }
    case NHM_IOCTL_ZERO: {
      nhm_entries_index = &nhm_entries;
      break;
    }
    case NHM_IOCTL_CLEAR: {
      nhm_list_clear();
      break;
    }
    case NHM_IOCTL_LENGTH: {
      if(copy_to_user(user_buffer, &nhm_entries_length, sizeof(typeof(nhm_entries_length)))) {
	printk(KERN_ALERT "[NHM] The copy to user failed\n");
	err = -EIO;
      }
      break;
    }
    case NHM_IOCTL_NF_TYPE: {
      /* copy for user space */
      if(copy_from_user(&nhm_nf_type, user_buffer, sizeof(nhm_nf_type_te))) {
	printk(KERN_ALERT "[NHM] The copy from user failed\n");
	err = -EIO;
      }
      break;
    }
    default:
      err = -ENOTTY;
  }
  mutex_unlock(&nhm_mutex);
  return err;
}

static void nhm_list_clear(void) {
  struct list_head *ptr, *next;
  struct nhm_list_s *tmp;
  raw_spin_lock(&nhm_entries_lock);
  list_for_each_safe(ptr, next, &nhm_entries) {
    tmp = list_entry(ptr, struct nhm_list_s, list);
    list_del(ptr);
    kfree(tmp);
  }
  nhm_entries_length = 0;
  raw_spin_unlock(&nhm_entries_lock);
}

/////////////////////////////////////////////////////////////////////////////////////////

static inline unsigned int nhm_get_type(nhm_nf_type_te nhm_nf_type) {
  if(nhm_nf_type == NHM_NF_TYPE_ACCEPT) return NF_ACCEPT;
  else if(nhm_nf_type == NHM_NF_TYPE_DROP) return NF_DROP;
  else if(nhm_nf_type == NHM_NF_TYPE_STOLEN) return NF_STOLEN;
  else if(nhm_nf_type == NHM_NF_TYPE_QUEUE) return NF_QUEUE;
  else if(nhm_nf_type == NHM_NF_TYPE_REPEAT) return NF_REPEAT;
  return NF_STOP;
}

/* Netfilter hook */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
static unsigned int nhm_hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
#else
static unsigned int nhm_hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
#endif
  struct sk_buff *sock_buff;
  struct iphdr *ip_header;
  struct udphdr *udp_header;
  sock_buff = skb;
  if (!sock_buff)
    return nhm_get_type(nhm_nf_type);
  else { 
    ip_header = (struct iphdr *)skb_network_header(sock_buff);
    if (!ip_header)
      return nhm_get_type(nhm_nf_type);
    else { 
      if (ip_header->protocol == IPPROTO_UDP) {
	udp_header = (struct udphdr *)(skb_transport_header(sock_buff)+sizeof(struct iphdr));
	printk(KERN_INFO "[NHM] DEBUG: nh: 0p%p\n", skb_network_header(sock_buff));
	printk(KERN_INFO "[NHM] DEBUG: mh: 0p%p\n", skb_mac_header(sock_buff));
	printk(KERN_INFO "[NHM] DEBUG: From IP address: %d.%d.%d.%dn", ip_header->saddr & 0x000000FF, (ip_header->saddr & 0x0000FF00) >> 8, (ip_header->saddr & 0x00FF0000) >> 16, (ip_header->saddr & 0xFF000000) >> 24);
	printk(KERN_INFO "[NHM] DEBUG: Ports s:%d, d:%d\n", udp_header->source, udp_header->dest);
	/* Callback function here*/
	return NF_ACCEPT;//NF_DROP;
      } else
	return nhm_get_type(nhm_nf_type);
    }
  }
}

/**
 * @fn static void nhm_hook_start(void)
 * @brief Start the netfilter hook.
 */
static void nhm_hook_start(void) {
  nfho.hook = nhm_hook_func;
  nfho.hooknum = 1;
  nfho.pf = PF_INET;
  nfho.priority = NF_IP_PRI_FIRST;
  //nf_register_hook(&nfho);
}


/**
 * @fn static void nhm_hook_stop(void)
 * @brief Stop the netfilter hook.
 */
static void nhm_hook_stop(void) {
  //nf_unregister_hook(&nfho);
}
 
/////////////////////////////////////////////////////////////////////////////////////////
/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(nhm_init);
module_exit(nhm_exit);

/* module infos */
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

