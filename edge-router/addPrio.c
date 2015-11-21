#define __KERNEL__
#define MODULE
#include <linux/ip.h>             
#include <linux/netdevice.h>      
#include <linux/skbuff.h>         
#include <linux/udp.h>          
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/init.h>
#include <asm-generic/types.h>
#include <net/checksum.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("linux-simple-firewall");
MODULE_AUTHOR("dnair");

struct iphdr *ip_header;	//ip header pointer
static unsigned char *ip_address = "\xAC\x10\x01\x01";
static unsigned char *sip_address = "\xAC\x10\x00\x05";

static struct nf_hook_ops netfilter_ops;                        
static char *interface = "lo";                          
static char *allow = "eth0";                          
static char *internal = "eth2";                          
static char *external = "eth1";                          
unsigned char *port = "\x00\x17";
unsigned char *htport = "\x00\x50";
unsigned int httport = 80;
struct sk_buff *sock_buff;                              
struct udphdr *udp_header;    
struct tcphdr *tcp_header;
struct icmphdr *icmp_header;
unsigned int main_hook(unsigned int hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff*))
{
	ip_header = ip_hdr(skb);
	tos_bits = ip_header->tos;				//tos bits
	
	__u8 priority = 4;
	__u8 origin_tos = tos_bits;
	__u8 ECN_mask = 3;	//ECN mask to get ECN bits
	__u8 ECN;		//ECN values

	/* ECN_MASK AND origin_tos will give ECN values*/ 
	ECN = origin_tos & ECN_mask;
    
	if ( (ip_header->daddr) == *(unsigned int*)sip_address)
		__u8 new_tos = priority | ECN;
	printk(KERN_INFO "old tos %d\n", ip_header->tos);
	ip_header->tos = new_tos; 
	
	printk(KERN_INFO "new tos %d\n", new_tos);
	//recalulate checksum and set

	//csum_replace2(&ip_header->check, htons(origin_tos), htons(new_tos));

	return NF_ACCEPT;
}

int init_module()
{
		printk(KERN_INFO "initialize kernel module\n");
        netfilter_ops.hook              =       main_hook;
        netfilter_ops.pf                =       PF_INET;        
        netfilter_ops.hooknum           =       0;
        netfilter_ops.priority          =       NF_IP_PRI_FIRST;
        nf_register_hook(&netfilter_ops);
        
return 0;
}
void cleanup_module() 
{
	nf_unregister_hook(&netfilter_ops); 
}
