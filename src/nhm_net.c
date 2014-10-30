#include "nhm_common.h"
#include <linux/version.h>	/* Needed for KERNEL_VERSION + LINUX_VERSION_CODE */
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/udp.h>

/* Global variables */
struct sk_buff *sock_buff;
struct iphdr *ip_header;
struct udphdr *udp_header;
static struct nf_hook_ops nfho;

/* Rgister the functions */
EXPORT_SYMBOL(nhm_net_start);
EXPORT_SYMBOL(nhm_net_stop);

struct nhm_net_entry_s entries;

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


/**
 * @fn void nhm_net_start(void)
 * @brief Start the netfilter hook.
 */
void nhm_net_start(void) {
  INIT_LIST_HEAD(&entries.list);
  nfho.hook = hook_func;
  nfho.hooknum = 1;
  nfho.pf = PF_INET;
  nfho.priority = NF_IP_PRI_FIRST;
  nf_register_hook(&nfho);
}


/**
 * @fn void nhm_net_stop(void)
 * @brief Stop the netfilter hook.
 */
void nhm_net_stop(void) {
  struct list_head *pos, *q;
  struct nhm_net_entry_s *entry;
  nf_unregister_hook(&nfho);
  list_for_each_safe(pos, q, &entries.list){
    entry = list_entry(pos, struct nhm_net_entry_s, list);
    list_del(pos);
    kfree(entry);
  }
}
