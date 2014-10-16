// 'nhm' netfilter hooks

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/version.h>	/* Needed for KERNEL_VERSION + LINUX_VERSION_CODE */
#include <linux/moduleparam.h>  /* Needed by params */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include "nhm_common.h"


#define DRIVER_VERSION "1.0"
#define DRIVER_AUTHOR  "Keidan"
#define DRIVER_DESC    "Netfilter Hook Module"
#define DRIVER_LICENSE "GPL"

/* Config defines */
#define CONFIG_DEBUG "debug="
#define CONFIG_DEBUG_LEN 6 /* debug= */

/* Parameters */
static bool debug = 0;

/* Global variables */
struct sk_buff *sock_buff;
struct iphdr *ip_header;
struct udphdr *udp_header;
static struct nf_hook_ops nfho;



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

/* Netfilter hook */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)
static unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
#else
static unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) {
#endif
  sock_buff = skb;
  if (!sock_buff)
    return NF_ACCEPT;
  else { 
    ip_header = (struct iphdr *)skb_network_header(sock_buff);
    if (!ip_header)
      return NF_ACCEPT;
    else { 
      if (ip_header->protocol == IPPROTO_UDP) {
	udp_header = (struct udphdr *)(skb_transport_header(sock_buff)+sizeof(struct iphdr));
	if(debug) {
	  printk(KERN_INFO "[NHM] DEBUG: nh: 0p%p\n", skb_network_header(sock_buff));
	  printk(KERN_INFO "[NHM] DEBUG: mh: 0p%p\n", skb_mac_header(sock_buff));
	  printk(KERN_INFO "[NHM] DEBUG: From IP address: %d.%d.%d.%dn", ip_header->saddr & 0x000000FF, (ip_header->saddr & 0x0000FF00) >> 8, (ip_header->saddr & 0x00FF0000) >> 16, (ip_header->saddr & 0xFF000000) >> 24);
	  printk(KERN_INFO "[NHM] DEBUG: Ports s:%d, d:%d\n", udp_header->source, udp_header->dest);
	}
	/* Callback function here*/
	return NF_ACCEPT;//NF_DROP;
      } else
	return NF_ACCEPT;
    }
  }
}



/* Module init */
static int __init init_nhm(void) {
  nfho.hook = hook_func;
  nfho.hooknum = 1;
  nfho.pf = PF_INET;
  nfho.priority = NF_IP_PRI_FIRST;
  nf_register_hook(&nfho);
  if(debug)
    printk(KERN_INFO "[NHM] Successfully inserted protocol module into kernel.\n");

  /* create a dir in sys/ */
  if (nhm_sysfs_link("nhm", nhm_sysfs)) return -ENOMEM;
	
  return 0;	
}

/* Module cleanup */
static void __exit cleanup_nhm(void) {
  nf_unregister_hook(&nfho);
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

