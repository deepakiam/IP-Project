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
int is_wred=0;
static unsigned long class1_beg;
static unsigned long class1_end;
static unsigned long class2_beg;
static unsigned long class2_end;
static unsigned long class3_beg;
static unsigned long class3_end;
static unsigned long class4_beg;
static unsigned long class4_end;
static unsigned long class5_beg;
static unsigned long class5_end;
static unsigned long class6_beg;
static unsigned long class6_end;

char read_1byte_data[2];
char read_8byte_data[9];
char read_10byte_data[11];

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

int compare (long addr1, long addr2, long saddr);

unsigned int main_hook(unsigned int hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff*))
{
	if(strcmp(in->name,allow) == 0){ return NF_ACCEPT; }
	ip_header = ip_hdr(skb);
	__u8 tos_bits = ip_header->tos;				//tos bits
	unsigned short prio = 63;

	if (compare(class1_beg, class1_end, (long)ip_header->saddr))
		prio = 1;
	else if(compare(class2_beg, class2_end, (long)ip_header->saddr))
		prio = 3;
	else if(compare(class3_beg, class3_end, (long)ip_header->saddr))
		prio = 7;
	else if(compare(class4_beg, class4_end, (long)ip_header->saddr))
		prio = 15;
	else if(compare(class5_beg, class5_end, (long)ip_header->saddr))
		prio = 31;
	else if(compare(class6_beg, class6_end, (long)ip_header->saddr))
		prio = 63;
		

	__u8 priority = prio<<2;
	__u8 origin_tos = tos_bits;
	__u8 ECN_mask = 3;	//ECN mask to get ECN bits
	__u8 ECN;		//ECN values
	__u8 new_tos;
	/* ECN_MASK AND origin_tos will give ECN values*/ 
	ECN = origin_tos & ECN_mask;
	ip_header->check = 0;
	printk(KERN_INFO "checksum %d\n", ip_header->check);
	printk(KERN_INFO "tos changed\n");
	new_tos = priority | ECN;
		
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
		breads = conf_file->f_op->read(conf_file, read_1byte_data, 2, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_1byte_data[1] = '\0';
		kstrtol(read_1byte_data, 10, &is_wred);
		printk(KERN_INFO "lets see this %x\n", is_wred);
		
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class1_beg);
		printk(KERN_INFO "lets see this %x\n", class1_beg);
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class1_end);
		printk(KERN_INFO "lets see this %x\n", class1_end);
		
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class2_beg);
		printk(KERN_INFO "lets see this %x\n", class2_beg);
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class2_end);
		printk(KERN_INFO "lets see this %x\n", class2_end);
		
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class3_beg);
		printk(KERN_INFO "lets see this %x\n", class3_beg);
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class3_end);
		printk(KERN_INFO "lets see this %x\n", class3_end);
		
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class4_beg);
		printk(KERN_INFO "lets see this %x\n", class4_beg);
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class4_end);
		printk(KERN_INFO "lets see this %x\n", class4_end);
		
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class5_beg);
		printk(KERN_INFO "lets see this %x\n", class5_beg);
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class5_end);
		printk(KERN_INFO "lets see this %x\n", class5_end);
		
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class6_beg);
		printk(KERN_INFO "lets see this %x\n", class6_beg);
		breads = conf_file->f_op->read(conf_file, read_10byte_data, 11, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_10byte_data[10] = '\0';
		kstrtol(read_10byte_data, 10, &class6_end);
		printk(KERN_INFO "lets see this %x\n", class6_end);
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

int compare (long addr1, long addr2, long saddr)
{
	int ads1[] = {0xff & addr1>>24, 0xff & addr1>>16, 0xff & addr1>>8, 0xff & addr1};
	int ads2[] = {0xff & addr2>>24, 0xff & addr2>>16, 0xff & addr2>>8, 0xff & addr2};
	int sads[] = {0xff & saddr, 0xff & saddr>>8, 0xff & saddr>>16, 0xff & saddr>>24};
	
	if (ads1[0] == sads[0] && ads1[1] == sads[1] && ads1[2] == sads[2])
	{
		if (ads1[3] <= sads[3] && sads[3] <= ads2[3])
			return 1;
	}
	return 0;
}
