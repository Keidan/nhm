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

#define RULE_PATTERN "sh=value dh=value si=value di=value sp1=value sp2=value dp1=value dp2=value np=value tp=value"
#define MIN_STR_RULE_LEN 3 /* xx: */
#define MIN_PORT_RULE_LEN (MIN_STR_RULE_LEN + 1) /* xxN: */


/* Parameters */
bool debug = 0;


static ssize_t sysfs_help_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { 
  return sprintf(buf, "list(ro): To display the blacklisted rules.\n"	\
		 "add(w-rule): Add a new rule.\n"			\
		 "  Fmt: "RULE_PATTERN"\n"				\
		 "  All fields are optionals (min 1)\n"			\
		 "  sh= Source hardware address.\n"			\
		 "  dh= Destination hardware address.\n"		\
		 "  si= Source IPv4 address.\n"				\
		 "  di= Destination IPv4 address.\n"			\
		 "  sp1= Source port or min port.\n"			\
		 "  sp2= Source port or max port.\n"			\
		 "  dp1= Destination port or min port.\n"		\
		 "  dp2= Destination port or max port.\n"		\
		 "  np= Network protocol (eg: after the ethernet hdr).\n" \
		 "  tp= Transport protocol (eg: after the IP hdr).\n"	\
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

static bool to_uint(char *value, size_t length, unsigned int *p_uint) {
  char str[6];
  int ret;
  /* temp copy */
  memset(str, 0, 6);
  strncpy(str, value, length);
  /* int convert */
  ret = kstrtouint(str, 10, p_uint);
  if((ret == -ERANGE) || (ret == -EINVAL)) return false;
  return true;
}

static ssize_t sysfs_add_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  bool found = 0;
  size_t length;
  char *value, *copy = (char*)buf + count;
  int last_space = count, diff = 0;
  struct nhm_net_entry_s entry;

  if(count > MIN_STR_RULE_LEN) {
    memset(&entry, 0, sizeof(struct nhm_net_entry_s));
    while(copy != (buf - 1)) {
      if(copy[0] == ' ' || copy == buf) {
	/* argument length */
	length = last_space - (copy - buf);
	/* trim spaces */
	value = copy[0] == ' ' ? copy + 1 : copy;
	if(value[length - 1] == ' ') length--;

	if(!memcmp(value, "sh=", MIN_STR_RULE_LEN)) {
	  strncpy(entry.sh, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "dh=", MIN_STR_RULE_LEN)) {
	  strncpy(entry.dh, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "si=", MIN_STR_RULE_LEN)) {
	  strncpy(entry.si, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "di=", MIN_STR_RULE_LEN)) {
	  strncpy(entry.di, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "sp1=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry.sp[0]);
	} else if(!memcmp(value, "sp2=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry.sp[1]);
	} else if(!memcmp(value, "dp1=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry.dp[0]);
	} else if(!memcmp(value, "dp2=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry.dp[1]);
	} else if(!memcmp(value, "np=", MIN_STR_RULE_LEN)) {
	  to_uint(value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN, &entry.np);
	} else if(!memcmp(value, "tp=", MIN_STR_RULE_LEN)) {
	  to_uint(value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN, &entry.tp);
	}
	diff = (int)(copy - buf);
	if(diff != -1) last_space = diff;
      }
      copy--;
    }
    
    printk(KERN_ERR "sh='%s' dh='%s' si='%s' di='%s' sp=%d-%d dp=%d-%d np=%d tp=%d\n", entry.sh, entry.dh, entry.si, entry.di, entry.sp[0], entry.sp[1], entry.dp[0], entry.dp[1], entry.np, entry.tp);
  }
  if(!found)
    printk(KERN_ERR "[NHM] Invalid rule format: '%s'\n", RULE_PATTERN);
  return count;
}

static struct nhm_sysfs_s nhm_sysfs = {
  .list = { NULL, NULL },
  .add = { NULL, sysfs_add_store },
  .del = { NULL, NULL },  
  .config = { sysfs_config_show, sysfs_config_store },
  .help = { sysfs_help_show, NULL },
};



/* Module init */
static int __init init_nhm(void) {
  /* create a dir in sys/ */
  if (nhm_sysfs_link("nhm", nhm_sysfs)) return -ENOMEM;
  nhm_net_start();
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

