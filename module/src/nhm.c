// 'nhm' netfilter hooks


#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#include "nhm_common.h"

#define DEVICE_NAME "nhm"        ///< The device will appear at /dev/nhm using this value
#define CLASS_NAME  "nhm"        ///< The device class -- this is a character device driver 

#define DRIVER_VERSION "1.0"
#define DRIVER_AUTHOR  "Keidan (Kevin Billonneau)"
#define DRIVER_DESC    "Netfilter Hook Module"
#define DRIVER_LICENSE "GPL"

static int    major_number;                 ///< Stores the device number -- determined automatically
static struct nhm_s message;           ///< That is passed from userspace
static int    number_opens = 0;             ///< Counts the number of times the device is opened
static struct class*  nhm_class  = NULL;    ///< The device-driver class struct pointer
static struct device* nhm_device = NULL;    ///< The device-driver device struct pointer
 
// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
 
/** 
 *  @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops = {
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release,
};
 
/** 
 *  @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init nhm_init(void){
  printk(KERN_INFO "NHM: Initializing the NHM LKM.\n");
 
  // Try to dynamically allocate a major number for the device -- more difficult but worth it
  major_number = register_chrdev(0, DEVICE_NAME, &fops);
  if (major_number < 0){
    printk(KERN_ALERT "NHM failed to register a major number.\n");
    return major_number;
  }
  printk(KERN_INFO "NHM: registered correctly with major number %d\n", major_number);
 
  // Register the device class
  nhm_class = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(nhm_class)){                // Check for error and clean up if there is
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_ALERT "NHM: Failed to register device class.\n");
    return PTR_ERR(nhm_class);          // Correct way to return an error on a pointer
  }
  printk(KERN_INFO "NHM: device class registered correctly.\n");
 
  // Register the device driver
  nhm_device = device_create(nhm_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
  if (IS_ERR(nhm_device)){               // Clean up if there is an error
    class_destroy(nhm_class);           // Repeated code but the alternative is goto statements
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_ALERT "NHM: Failed to create the device.\n");
    return PTR_ERR(nhm_device);
  }
  printk(KERN_INFO "NHM: device class created correctly.\n"); // Made it! device was initialized
  return 0;
}
 
/** 
 *  @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit nhm_exit(void){
  device_destroy(nhm_class, MKDEV(major_number, 0));     // remove the device
  class_unregister(nhm_class);                          // unregister the device class
  class_destroy(nhm_class);                             // remove the device class
  unregister_chrdev(major_number, DEVICE_NAME);             // unregister the major number
  printk(KERN_INFO "NHM: Goodbye from the LKM!.\n");
}
 
/** 
 *  @brief The device open function that is called each time the device is opened
 *  This will only increment the number_opens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
  number_opens++;
  printk(KERN_INFO "NHM: Device has been opened %d time(s)\n", number_opens);
  return 0;
}
 
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
  int error_count = 0, size = NHM_LENGTH;
  // copy_to_user has the format ( * to, *from, size) and returns 0 on success
  error_count = copy_to_user(buffer, &message, size);
 
  if (error_count==0){            // if true then have success
    printk(KERN_INFO "NHM: Sent %d characters to the user\n", size);
    return 0;
  }
  else {
    printk(KERN_INFO "NHM: Failed to send %d characters to the user\n", error_count);
    return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
  }
}
 
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
  if(len > NHM_LENGTH){
    printk(KERN_INFO "NHM: Invalid message size\n");
    return -EFAULT; 
  }
  memcpy(&message, buffer, len);
  printk(KERN_INFO "NHM: Received a new message, ation: %d\n", message.action);
  return len;
}
 
/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
  printk(KERN_INFO "NHM: Device successfully closed.\n");
  return 0;
}
 
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

