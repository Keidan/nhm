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

#define RULE_PATTERN "sh=value dh=value si=value di=value sp=value dp=value np=value tp=value"
#define MIN_STR_RULE_LEN 3 /* xx: */

struct arg_s {
    const char* name;
    int pos;
};
static struct arg_s g_args [] = {
  { "sh=", -1 },
  { "dh=", -1 },
  { "si=", -1 },
  { "di=", -1 },
  { "sp=", -1 },
  { "dp=", -1 },
  { "np=", -1 },
  { "tp=", -1 },
  { NULL, -1 },
};


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
		 "  sp= Source port or range.\n"			\
		 "  dp= Destination port or range.\n"			\
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
static void next_arg(const char* buf, size_t count, const char* arg, int* pos) {
  size_t i;
  *pos = -1;
  for(i = 0; i < count; i++) 
    if(!memcmp(buf+i, arg, MIN_STR_RULE_LEN)) {
      *pos = (i+MIN_STR_RULE_LEN);
      break;
    }
}
static void sort_args(struct arg_s args[]) {
  struct arg_s arg;
  int i;  int j;

  for (i=0; ; i++) {
    for (j=i+1; ; j++) {
      if(!args[i].name || !args[j].name) return;
      if (args[i].pos > args[j].pos) {
	arg=args[i];
	args[i]=args[j];
	args[j]=arg;
      }
    }
  }
}
static ssize_t sysfs_add_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  bool found = 0;
  struct arg_s *args;
  if(count > MIN_STR_RULE_LEN) {
    args = g_args;
    while(args->name) {
      next_arg(buf, count, args->name, &args->pos);
      args++;
    }
    sort_args(g_args);
    args = g_args;
    while(args->name) {
      printk(KERN_ERR "[NHM] Buffer:%s '%s'\n", args->name, buf+args->pos);
      args++;
    }
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

