#include <linux/kernel.h>
#include <linux/in.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/init.h>
#include <asm/types.h>
#include <linux/ip.h>
#include <net/checksum.h>

         
struct sk_buff *sock_buff;	//current buffer
struct iphdr *ip_header;	//ip header pointer
char addr1[21];			//range start address 
char addr2[21];			//range end address
char priority[3];		//range priority
int priority_flag = 0;		//set-priority
unsigned short checksum(int iphl, unsigned short ipheader[]);
int * parseIPV4(char* ipAddress, int arr[4]);
int isInRange(char *start, char *end, char *check);
char* convert_IP(int IP);


unsigned int hook_setpriority(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
	
	sock_buff = *skb;
	unsigned short ip_len;
	struct sockaddr_in src_addr;
	unsigned int tot_len;
	__u8 tos_bits;	//DSCP(6) + ECN(2) 
	


	//extract header info on incoming pkt
	
	ip_header = ip_hdr(sock_buff);	//extract ip_header
	
	ip_len = ip_header->ihl;				//no. of words of IP header
	printk(KERN_INFO "got these details len = %d\n", ip_len);
	return NF_ACCEPT;

	//memset(&src_addr, 0, sizeof(src_addr));
	//src_addr.sin_addr.s_addr = ip_header->saddr;			        //source ip address
	unsigned long s_addr = ip_header->saddr;			        //source ip address
	
	tot_len = ip_header->tot_len;			//total packet length
	tos_bits = ip_header->tos;				//tos bits
	
	printk(KERN_INFO "got these details len = %d, addr = %d, tot_len = %d, tos_bits = %d\n", ip_len, s_addr, tot_len, tos_bits);
	return NF_ACCEPT;
	
	//determine the class of packet from source
	char incoming_addr[20] ;
	//sprintf(incoming_addr,"%s",convert_IP(src_addr.sin_addr.s_addr));

	__u8 origin_tos = tos_bits;
	__u8 ECN_mask = 3;	//ECN mask to get ECN bits
	__u8 ECN;		//ECN values

	/* ECN_MASK AND origin_tos will give ECN values*/ 
	ECN = origin_tos & ECN_mask;

	/* Currently we are implementing on a limited class range. 
	   To address this dynamically, use of configuration file will be used to check against the class.
	   Address Class Range will be as follows: 
	Class A : 	Higest Priority :	100.0.0.1 - 100.0.0.63   
	Class B :	Medium Priority :	100.0.0.64 - 100.0.0.127 
	Class C :	Lowest Priority :	100.0.0.128 - 100.0.0.255
	*/
	

/*	reading from /etc/wred.conf	*/

	char *conf_fpath = "/etc/wred.conf";
	struct file* conf_file;
	int b_read;
	mm_segment_t old_fs = get_fs();
	set_fs(get_ds());
	conf_file = filp_open(conf_fpath,O_RDONLY,0);
	int a=0;
	for (;a<21;a++)	
	{addr1[a]='\0';	addr2[a]='\0';	}
	a=0;
	for(;a<3;a++)	priority[a]='\0';
	
	b_read = conf_file->f_op->read(conf_file,addr1,20, &conf_file->f_pos);
	printk(KERN_INFO "read from file : %s\n", addr1);


/*	*/
	__u8 priority = 0;

	if(isInRange("100.0.0.0","100.0.0.63",incoming_addr) == 1)			//Class A  Bit format : 000 001 XX
	{
		priority = 4;
			
	}else if(isInRange("100.0.0.64","100.0.0.127",incoming_addr) == 1)		//Class B  Bit format : 000 011 XX
	{
		priority = 12;
			
	}else if(isInRange("100.0.0.128","100.0.0.255",incoming_addr) == 1)		//Class C  Bit format : 000 111 XX
	{
		priority = 28;
	}

	__u8 new_tos = priority | ECN;	

	//set priority in the header and modify header details

	ip_header->tos = new_tos; 

	//recalulate checksum and set

	csum_replace2(&ip_header->check, htons(origin_tos), htons(new_tos));

	//forward the packet

	return NF_ACCEPT;
	
}


/*Function to check if *check lies within the IPv4 address range of *start and *end inclusive*/

int isInRange(char *start, char *end, char *check)	
{
	int address_array[3][4];
	int b=0;
	
	parseIPV4(start,address_array[0]);	
	parseIPV4(end,address_array[1]);
	parseIPV4(check,address_array[2]);

	for(b=0;b<4;b++)
		if(((address_array[0][b] <= address_array[2][b]) && (address_array[2][b] <= address_array[1][b])) == 0) return 0;

	return 1;

}

/*Function to separate octets of IPv4 address*/

int * parseIPV4(char* ipAddress, int arr[4]) {
 	
	sscanf(ipAddress, "%d.%d.%d.%d", &arr[3], &arr[2], &arr[1], &arr[0]);
	return arr;	
}


/*Function to calculate header checksum*/

unsigned short checksum(int iphl, unsigned short ipheader[])
{
	unsigned long sum = 0;
	//unsigned short temp_tos_word =0;
	int i=0;
	for(;i<(iphl*2);i++)
	{	
		/*if(i == 0)
		{	
			temp_tos_word = ipheader[i] & 0xFF00;
			ipheader[i] = temp_tos_word | newtos;
		}*/
		sum = sum + (unsigned long)ipheader[i];
	}
	
	unsigned short t1 = sum&0XFF;		//get last 16 - bits
	unsigned short t2 = (sum>>16)&0xFF;	//get carry forward
	
	return (~(t1 + t2));	//add the carry forward and sum and take 1's complement

}

/* Convert IP address from integer to dotted decimal*/
char* convert_IP(int IP)
{
    unsigned char dot_dec[4];
    dot_dec[0] = IP & 0xFF;   dot_dec[1] = (IP >> 8) & 0xFF;    dot_dec[2] = (IP >> 16) & 0xFF;    dot_dec[3] = (IP >> 24) & 0xFF;	
    sprintf(dot_dec,"%d.%d.%d.%d\n", dot_dec[0], dot_dec[1], dot_dec[2], dot_dec[3]);        
    return dot_dec;


}

static struct nf_hook_ops nfho;

int init_module()
{
  printk(KERN_INFO "entered the module\n");
  //&nfho = (struct nf_hook_ops *) malloc(sizeof(&nfho));
  nfho.hook = hook_setpriority;          	//hook function call
  nfho.hooknum = 2;		//call when decsion is made for forwarding
  nfho.pf = NFPROTO_IPV4;                           		//IPV4 pkts
  nfho.priority = NF_IP_PRI_FIRST;             	//set to highest priority
  nf_register_hook(&nfho);                     	//register hook

  return 0;                                    //return 0 for success
}


void cleanup_module()
{
  nf_unregister_hook(&nfho);                     //unregister
}

