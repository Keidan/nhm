// 'nhm' netfilter hooks

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/moduleparam.h>  /* Needed by params */
#include "nhm_common.h"


#define DRIVER_VERSION "1.0"
#define DRIVER_AUTHOR  "Keidan"
#define DRIVER_DESC    "Netfilter Hook Module"
#define DRIVER_LICENSE "GPL"

/* Config defines */
#define CONFIG_DEBUG "debug="
#define CONFIG_DEBUG_LEN 6 /* debug= */

/* Parameters */
bool debug = 0;


static ssize_t sysfs_help_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { 
  return sprintf(buf, "list(ro): To display the blacklisted rules.\n"	\
		 "add(w-rule): Add a new rule.\n"			\
		 "del(w-rule): Delete a rule.\n"			\
		 "config(rw): Config update.\n"				\
		 "  debug=0/1\n");
}
static ssize_t sysfs_config_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { 
  return sprintf(buf, "debug:%d\n", debug);
}
static ssize_t sysfs_config_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  if(count > CONFIG_DEBUG_LEN && !strncmp(buf, CONFIG_DEBUG, CONFIG_DEBUG_LEN)) {
    sscanf(buf, CONFIG_DEBUG"%d", (int*)&debug);
    printk(KERN_INFO "[NHM] New debug mode : %d\n", debug);
  } else
    printk(KERN_ERR "[NHM] Invalid or unknown option: '%s", buf);
  return count;
}

static struct nhm_sysfs_s nhm_sysfs = {
  .list = { NULL, NULL },
  .add = { NULL, NULL },
  .del = { NULL, NULL },  
  .config = { sysfs_config_show, sysfs_config_store },
  .help = { sysfs_help_show, NULL },
};



/* Module init */
static int __init init_nhm(void) {
  nhm_net_start();
  /* create a dir in sys/ */
  if (nhm_sysfs_link("nhm", nhm_sysfs)) return -ENOMEM;
  if(debug)
    printk(KERN_INFO "[NHM] Successfully inserted protocol module into kernel.\n");
  return 0;	
}

/* Module cleanup */
static void __exit cleanup_nhm(void) {
  nhm_net_stop();
  nhm_sysfs_unlink();
  if(debug)
    printk(KERN_INFO "[NHM] Successfully unloaded protocol module.\n");
}

/* module parameters */
module_param(debug, bool, 0);
MODULE_PARM_DESC(debug, "Enable extra logs.");
/* module functions */
module_init(init_nhm);
module_exit(cleanup_nhm);

/* module infos */
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

