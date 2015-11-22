#define __KERNEL__
#define MODULE
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>         
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("linux-simple-firewall");
MODULE_AUTHOR("dnair");

struct iphdr *ip_header;	//ip header pointer
static unsigned char *ip_address = "\xAC\x10\x01\x01";
static unsigned char *sip_address = "\xAC\x10\x00\x02";
int err = 0;
static unsigned char *class1_beg;
static unsigned char *class1_end;
static unsigned char *class2_beg;
static unsigned char *class2_end;
static unsigned char *class3_beg;
static unsigned char *class3_end;
static unsigned char *class4_beg;
static unsigned char *class4_end;
static unsigned char *class5_beg;
static unsigned char *class5_end;
static unsigned char *class6_beg;
static unsigned char *class6_end;

char read_4byte_data[6];
char read_16byte_data[17];

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
unsigned short prio = 4;
unsigned int main_hook(unsigned int hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff*))
{
	if(strcmp(in->name,allow) == 0){ return NF_ACCEPT; }
	ip_header = ip_hdr(skb);
	__u8 tos_bits = ip_header->tos;				//tos bits
	
	__u8 priority = prio<<2;
	__u8 origin_tos = tos_bits;
	__u8 ECN_mask = 3;	//ECN mask to get ECN bits
	__u8 ECN;		//ECN values
	__u8 new_tos;
	/* ECN_MASK AND origin_tos will give ECN values*/ 
	ECN = origin_tos & ECN_mask;
	ip_header->check = 0;
	printk(KERN_INFO "checksum %d\n", ip_header->check);
	if ( (ip_header->saddr) == *(unsigned int*)sip_address)
	{
		printk(KERN_INFO "tos changed\n");
		new_tos = priority | ECN;
	}
	printk(KERN_INFO "old tos %d ecn %d\n", ip_header->tos, ECN);
	ip_header->tos = new_tos; 
	
	printk(KERN_INFO "new tos %d\n", new_tos);
	//recalulate checksum and set

	//csum_replace2(&ip_header->check, htons(origin_tos), htons(new_tos));

	return NF_ACCEPT;
}

int init_module()
{
		printk(KERN_INFO "initialize kernel module\n");
	
	
	char  *conf_file_path ="/etc/aqmconf";
	struct file* conf_file;
	int breads;
	mm_segment_t o_fs = get_fs();
	set_fs(get_ds());
	conf_file = filp_open(conf_file_path, O_RDONLY, 0);
	
	if(IS_ERR(conf_file)){
		err = PTR_ERR(conf_file);
		printk(KERN_INFO "some error opening file!!\n");
	}       
	
	if(conf_file != NULL){
		printk(KERN_INFO "initializing parameters\n");
		breads = conf_file->f_op->read(conf_file, read_16byte_data, 17, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_16byte_data[16] = '\0';
		int i = 0;
		for(i=0;i<17;i++){
			printk(KERN_INFO "%dth byte read : %d\n", i, read_16byte_data[i]);
			strncat(class1_beg, read_16byte_data[i],1);
		}
		printk(KERN_INFO "lets see this %x\n", class1_beg);
	} else {
		printk(KERN_INFO "error reading file!!");
	}
	set_fs(o_fs);
	filp_close(conf_file, NULL);
	
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
