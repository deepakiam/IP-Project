#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>

#include<red.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

/* Create the AQM hook function */
static unsigned int aqm_ hook(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *)){

	//invoke red here for packet processing
	int printk(KERN_INFO "executing the AQM hook function");
	return NF_ACCEPT;			   //after processing the packet, return NF_ACCEPT to let the packet pass
}

/* Register the hook function */
static struct nf_hook_ops nfhops = {
        .hook     = aqm_hook,                      // our hook function
	.priority = NF_IP_PRI_FIRST,               // function registered with highest priorit
        .hooknum  = NF_IP_FORWARD,                 // callback registered for this hook
        .pf       = NFPROTO_IPV4                   // protocol is IPv4 
};

/* Kernel module init handler */
static __init int my_init(void)
{
        return nf_register_hook(&nfhops);
}

/* Kernel module exit handler */
static __exit void my_exit(void)
{
    nf_unregister_hook(&nfhops);
}

module_init(my_init);
module_exit(my_exit);
