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
#include <linux/moduleparam.h>    // Needed by params
#include <linux/list.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/socket.h>
#include <linux/udp.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <nhm.h>

struct nhm_list_s {
    struct nhm_s rule;
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
static struct list_head       nhm_rules;
static struct list_head*      nhm_rules_index = NULL;
static unsigned int           nhm_rules_length;
static nhm_nf_type_te         nhm_nf_type = NHM_NF_TYPE_ACCEPT;
static bool                   ipv6_support = 0;
static unsigned char          null_hw[NHM_LEN_HW] = NHM_NULL_HW;
static unsigned char          null_ip6[NHM_LEN_IPv6] = NHM_NULL_IPv6;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
static spinlock_t             nhm_rules_lock;
#define raw_spin_lock         spin_lock
#define raw_spin_unlock       spin_unlock
#else
static DEFINE_RAW_SPINLOCK    (nhm_rules_lock);
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
  spin_lock_init(&nhm_rules_lock);
#endif
  INIT_LIST_HEAD(&nhm_rules);
  nhm_rules_length = 0;
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
  nhm_rules_index = &nhm_rules;
  return 0;
}

static int nhm_dev_release(struct inode *inodep, struct file *filep){
  number_opens--;
  return 0;
}

static void nhm_print_rule(const char* title, struct nhm_s *message) {
  unsigned char buffer [4];
  printk(KERN_INFO "[NHM] %s %s %s\n", title, message->dev,
	 (message->dir == NHM_DIR_INPUT ? "input" : (message->dir == NHM_DIR_OUTPUT ? "output" : "both")));
  printk(KERN_INFO "[NHM] HWaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
	 message->hw[0], message->hw[1], message->hw[2], message->hw[3], message->hw[4], message->hw[5]);
  nhm_from_ipv4(buffer, 0, message->ip4);
  printk(KERN_INFO "[NHM] IPv4: %d.%d.%d.%d:[%d-%d]\n", buffer[3], buffer[2], buffer[1], buffer[0], message->port[0], message->port[1]);
  printk(KERN_INFO "[NHM] Proto: eth-%d, ip-%d\n", message->eth_proto, message->ip_proto);
}

static ssize_t nhm_dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
  int error_count = 0, size = NHM_LENGTH;
  struct nhm_list_s *tmp;
  mutex_lock(&nhm_mutex);
  if(nhm_rules_index == &nhm_rules) nhm_rules_index = nhm_rules_index->next;
  if(!nhm_rules_index) {
    mutex_unlock(&nhm_mutex);
    printk(KERN_ALERT "NHM: Invalid list pointer\n");
    return -EFAULT; 
  }
  tmp = list_entry(nhm_rules_index, struct nhm_list_s, list);
  raw_spin_lock(&nhm_rules_lock);
  nhm_rules_index = nhm_rules_index->next;
  raw_spin_unlock(&nhm_rules_lock);

  error_count = copy_to_user(buffer, &tmp->rule, size);
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
	nhm_print_rule("Add new rule", &message);
	/* sanity check */
        raw_spin_lock(&nhm_rules_lock);
	new = kmalloc(sizeof(struct nhm_list_s), GFP_KERNEL);
	if(!new) {
	  printk(KERN_ALERT "NHM: not enough memory.\n");
	  err = -ENOMEM; 
	} else {
	  /* add new rule */
	  memcpy(&new->rule, &message, NHM_LENGTH);
	  memset(&new->list, 0, sizeof(struct list_head));
	  //INIT_LIST_HEAD(&new->list);
	  nhm_rules_length++;
	  list_add_tail(&new->list, &nhm_rules);
	}
        raw_spin_unlock(&nhm_rules_lock);
      }
      break;
    }
    case NHM_IOCTL_DEL: {
      /* copy for user space */
      if(copy_from_user(&message, user_buffer, NHM_LENGTH)) {
	printk(KERN_ALERT "[NHM] The copy from user failed\n");
	err = -EIO;
      } else {
	raw_spin_lock(&nhm_rules_lock);
	list_for_each_safe(ptr, next, &nhm_rules) {
	  tmp = list_entry(ptr, struct nhm_list_s, list);
	  if(nhm_is_same(&message, &tmp->rule)) {
	    nhm_print_rule("Remove rule", &message);
	    nhm_rules_length--;
	    list_del(ptr);
	    kfree(tmp);
	    break;
	  }
	}
	raw_spin_unlock(&nhm_rules_lock);
      }
      break;
    }
    case NHM_IOCTL_ZERO: {
      nhm_rules_index = &nhm_rules;
      break;
    }
    case NHM_IOCTL_CLEAR: {
      nhm_list_clear();
      break;
    }
    case NHM_IOCTL_LENGTH: {
      if(copy_to_user(user_buffer, &nhm_rules_length, sizeof(typeof(nhm_rules_length)))) {
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
  raw_spin_lock(&nhm_rules_lock);
  list_for_each_safe(ptr, next, &nhm_rules) {
    tmp = list_entry(ptr, struct nhm_list_s, list);
    list_del(ptr);
    kfree(tmp);
  }
  nhm_rules_length = 0;
  raw_spin_unlock(&nhm_rules_lock);
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

/**
 * @fn static void nhm_hook_fill_rule(struct sk_buff *sock_buff, struct nhm_s *r, nhm_dir_te dir)
 * @brief Fill the rule with the current packet.
 * @param sock_buff sock_buff
 * @param r r
 * @param dir dir
 * @return 1 if the packet is consumed, 0 else.
 */
static void nhm_hook_fill_rule(struct sk_buff *sock_buff, struct nhm_s *r, nhm_dir_te dir) {
  struct ethhdr *eth;
  unsigned int iphdr_len;
  struct iphdr* iphdr_v4;
  struct ipv6hdr* iphdr_v6;
  struct udphdr* udp;
  struct tcphdr* tcp;
  memset(&r, 0, NHM_LENGTH);

  /* decode the headers and fill the variables */
  eth = eth_hdr(sock_buff);
  if (eth) {
    if(dir == NHM_DIR_INPUT || dir == NHM_DIR_BOTH)
      memcpy(r->hw, eth->h_source, NHM_LEN_HW);
    else if(dir == NHM_DIR_OUTPUT)
      memcpy(r->hw, eth->h_dest, NHM_LEN_HW);
    r->eth_proto = eth->h_proto;
    iphdr_v4 = (struct iphdr *)skb_network_header(sock_buff);
    if (iphdr_v4) {
      if(iphdr_v4->version == 6 && ipv6_support) {
	iphdr_v6 = (struct ipv6hdr *)iphdr_v4;
	iphdr_len = sizeof(struct ipv6hdr);
	r->ip_proto = iphdr_v6->nexthdr;
	r->ip4 = 0;
	if(dir == NHM_DIR_INPUT || dir == NHM_DIR_BOTH)
	  memcpy(r->ip6, &iphdr_v6->saddr.s6_addr, NHM_LEN_IPv6);
	else if(dir == NHM_DIR_OUTPUT)
	  memcpy(r->ip6, &iphdr_v6->saddr.s6_addr, NHM_LEN_IPv6);
      } else {
	iphdr_len = sizeof(struct iphdr);
	r->ip_proto = iphdr_v4->protocol;
	memcpy(r->ip6, null_ip6, NHM_LEN_IPv6);
	if(dir == NHM_DIR_INPUT || dir == NHM_DIR_BOTH)
	  r->ip4 = iphdr_v4->saddr;
	else if(dir == NHM_DIR_OUTPUT)
	  r->ip4 = iphdr_v4->daddr;
      }
      if (r->ip_proto == IPPROTO_UDP) {
	udp = (struct udphdr *)(skb_transport_header(sock_buff)+iphdr_len);
	if(dir == NHM_DIR_INPUT || dir == NHM_DIR_BOTH)
	  r->port[0] = udp->source;
	else if(dir == NHM_DIR_OUTPUT)
	  r->port[0] = udp->dest;
      } else if (r->ip_proto == IPPROTO_TCP) {
	tcp = (struct tcphdr *)(skb_transport_header(sock_buff)+iphdr_len);
	if(dir == NHM_DIR_INPUT || dir == NHM_DIR_BOTH)
	  r->port[0] = tcp->source;
	else if(dir == NHM_DIR_OUTPUT)
	  r->port[0] = tcp->dest;
      }
    }
  }
}


/**
 * @fn static char nhm_hook_test_rule(struct sk_buff *sock_buff, const struct net_device *idevice, const struct net_device *odevice, struct nhm_s *rule, nhm_dir_te dir)
 * @brief Manage the current rule.
 * @param sock_buff sock_buff
 * @param idevice idevice
 * @param odevice odevice
 * @param rule rule
 * @param dir dir
 * @return 1 if the packet is consumed, 0 else.
 */
static char nhm_hook_test_rule(const struct net_device *idevice, const struct net_device *odevice, struct nhm_s *packet_r, struct nhm_s *rule) {
  /* Tests the device name, and continues if the variable is not initialized */
  if((rule->dev[0] && memcmp(rule->dev, idevice->name, IFNAMSIZ) == 0) || (rule->dev[0] && memcmp(rule->dev, odevice->name, IFNAMSIZ) == 0) || !rule->dev[0]) {
    /* Tests the hardware address, and continues if the variable is not initialized */
    if((memcmp(rule->hw, null_hw, NHM_LEN_HW) && memcmp(rule->hw, packet_r->hw, NHM_LEN_HW) == 0) || memcmp(rule->hw, null_hw, NHM_LEN_HW) == 0) {
      /* Tests the ethernet protocol, and continues if the variable is not initialized */
      if((rule->eth_proto && rule->eth_proto == packet_r->eth_proto) || !rule->eth_proto) {
	/* Tests the IPv4 address and the IPv6 address, and continues if the variable is not initialized */
	if(((rule->ip4 && rule->ip4 == packet_r->ip4) || !rule->ip4) 
	   || ((memcmp(rule->ip6, null_ip6, NHM_LEN_IPv6) && memcmp(rule->ip6, packet_r->ip6, NHM_LEN_IPv6) == 0) || memcmp(rule->ip6, null_ip6, NHM_LEN_IPv6) == 0)) {
	  /* Tests the IP proto, and continues if the variable is not initialized */
	  if((rule->ip_proto && rule->ip_proto == packet_r->ip_proto) || !rule->ip_proto) {
	    /* Tests the port(s), and continues if the variable is not initialized */
	    if((rule->port[0] && rule->port[1] && packet_r->port[0] >= rule->port[0] && packet_r->port[0] <= rule->port[1])
	       || (rule->port[0] && !rule->port[1] && packet_r->port[0] == rule->port[0])
	       || (!rule->port[0] && rule->port[1] && packet_r->port[0] == rule->port[1])
	       || (!rule->port[0] && !rule->port[1])) {
	      return 1; /* consume the packet */
	    }
	  }
	}
      }
    }
  }
  return 0; /* nothing done */
}

/* Netfilter hook */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
static unsigned int nhm_hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
#else
static unsigned int nhm_hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
#endif
  struct list_head *ptr, *next;
  struct nhm_list_s *tmp;
  struct sk_buff *sock_buff;
  struct nhm_s r;
  unsigned int result = nhm_get_type(nhm_nf_type);
  sock_buff = skb;
  if (sock_buff) { 

    mutex_lock(&nhm_mutex);
    raw_spin_lock(&nhm_rules_lock);
    list_for_each_safe(ptr, next, &nhm_rules) {
      tmp = list_entry(ptr, struct nhm_list_s, list);
      if(tmp->rule.dir == NHM_DIR_BOTH) {
	nhm_hook_fill_rule(sock_buff, &r, NHM_DIR_INPUT);
	if(nhm_hook_test_rule(in, out, &r, &tmp->rule)) {
	  result = nhm_get_type(tmp->rule.nf_type);
	  break;
	}
	nhm_hook_fill_rule(sock_buff, &r, NHM_DIR_OUTPUT);
	if(nhm_hook_test_rule(in, out, &r, &tmp->rule)) {
	  result = nhm_get_type(tmp->rule.nf_type);
	  break;
	}
      } else {
	nhm_hook_fill_rule(sock_buff, &r, tmp->rule.dir);
	if(nhm_hook_test_rule(in, out, &r, &tmp->rule)) {
	  result = nhm_get_type(tmp->rule.nf_type);
	  break;
	}
      }
    }
    raw_spin_unlock(&nhm_rules_lock);
    mutex_unlock(&nhm_mutex);
  }
  return result;
}

/**
 * @fn static void nhm_hook_start(void)
 * @brief Start the netfilter hook.
 */
static void nhm_hook_start(void) {
  nfho.hook = nhm_hook_func;
  nfho.hooknum = 1;
  nfho.pf = !ipv6_support ? PF_INET : PF_INET6;
  nfho.priority = NF_IP_PRI_FIRST;
  nf_register_hook(&nfho);
}


/**
 * @fn static void nhm_hook_stop(void)
 * @brief Stop the netfilter hook.
 */
static void nhm_hook_stop(void) {
  nf_unregister_hook(&nfho);
}
 
/////////////////////////////////////////////////////////////////////////////////////////
/* module parameters */
module_param(ipv6_support, bool, 0);
MODULE_PARM_DESC(ipv6_support, "1 to enable the support of the IPv6 packets, 0 else.");
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

