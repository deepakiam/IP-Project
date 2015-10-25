#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <inttypes.h>
static struct nf_hook_ops nfho;         
struct sk_buff *sock_buff;	//current buffer
struct iphdr *ip_header;	//ip header pointer

unsigned int hook_setpriority(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
	
	sock_buff = *skb;
	unsigned shor ip_len;
	struct sockaddr_in src_addr;
	unsigned int tot_len;
	__u8 tos_bits;	//DSCP(6) + ECN(2) 
	


	//extract header info on incoming pkt
	
	ip_header = (struct iphdr *)skb_network_header(sock_buff);	//extract ip_header
	
	ip_len = ip_header->ihl * 4;				//no. of words of IP header

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.sin_addr.s_addr = iph_header->saddr			        //source ip address
	
	tot_len = ntohs(ip_header->tot_len);			//total packet length
	tos_bits = ip_header->tos;				//tos bits
	
	
	
	//determine the class of packet from source
	char incoming_addr[20] ;
	sprintf(incoming_addr,"%s",inet_ntoa(src_addr.sin_addr));

	__u8 origin_tos = tos_bits;
	__u8 ECN_mask = 3;	//ECN mask to get ECN bits
	__u8 ECN;		//ECN values
 
	//Address Class Range : 100.0.0.1 - 100.0.0.63   100.0.0.64 - 100.0.0.127 100.0.0.128 - 100.0.0.255
	
	if(strcmp(incoming_addr,"100.0.0.0") >= 0  && strcmp(incoming_addr,"100.0.0.64") < 0)			//Class A
	{
		
	}else if(strcmp(incoming_addr,"100.0.0.63") > 0  && strcmp(incoming_addr,"100.0.0.128") < 0)		//Class B
	{
			
	}else if(strcmp(incoming_addr,"100.0.0.127") > 0  && strcmp(incoming_addr,"100.0.0.256") < 0)		//Class C
	{

	}

	//set priority in the header and modify header details

	//recalulate checksum and set

	//forward the packet
	
}


int init_module()
{
  nfho.hook = hook_setpriority;          	//hook function call
  nfho.hooknum = NF_IP_FORWARD		//call when decsion is made for forwarding
  nfho.pf = PF_INET;                           		//IPV4 pkts
  nfho.priority = NF_IP_PRI_FIRST;             	//set to highest priority
  nf_register_hook(&nfho);                     	//register hook

  return 0;                                    //return 0 for success
}


void cleanup_module()
{
  nf_unregister_hook(&nfho);                     //unregister
}
