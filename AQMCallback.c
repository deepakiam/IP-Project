#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include "Red.h"

int minths[6];					//array to hold min threshold values for each class
int maxths[6];					//array to hold max threshold values for each class
int wqs[6];					//array to hold  weight for the moving ave for each class
int maxpbs[6];				//array to hold maxpb values for each class
int minthred, maxthred;				// min and max threshold for queue sizes for base RED implementation
int wqred, maxpbred;				//weight and maxpb for base RED implementation

/* Create the AQM hook function */
static unsigned int aqm_hook(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *)){
        if(is_wred == 1){
		printk(KERN_INFO "WRED implemented\n");
		struct iphdr *ip_header = (struct iphdr *)skb_network_header(skb);
		unsigned int tos =((unsigned int)ip_header->tos);		// get TOS field form the header
		unsigned int tos_mask = 252;					// mask value set to  11111100 to get the DSCP bits
		unsigned int class = tos&tos_mask;				// bitwise and with mask value to get DSCP bits
		switch(class){
			case 4 :
				enqueue(red(skb,minths[0],maxths[0],wqs[0],maxpbs[0]));
				break;
			case 12 :
                                enqueue(red(skb,minths[1],maxths[1],wqs[1],maxpbs[1]));
                                break;
			case 28 :
                                enqueue(red(skb,minths[2],maxths[2],wqs[2],maxpbs[2]));
                                break; 
			case 60 :
                                enqueue(red(skb,minths[3],maxths[3],wqs[3],maxpbs[3]));
                                break;
			case 124 :
                                enqueue(red(skb,minths[4],maxths[4],wqs[4],maxpbs[4]));
                                break;
                        case 252 :
                                enqueue(red(skb,minths[5],maxths[5],wqs[5],maxpbs[5]));
                                break;
                        default :
                                enqueue(red(skb,minths[5],maxths[5],wqs[5],maxpbs[5]));
                                break;
		}
		
        }else{
		printk(KERN_INFO "No WRED implemented");
		enqueue(red(skb, minthred, maxthred, wqred, maxpbred));	//invoke red here for packet processing
	}
	
	printk(KERN_INFO "executing the AQM hook function");
	return NF_ACCEPT;			   //after processing the packet, return NF_ACCEPT to let the packet pass
}

/* Register the hook function */
static struct nf_hook_ops nfhops;

/* Kernel module init handler */
//static int __init myinit(void);
int init_module(void){
        struct file  *conf_file;
	head = kmalloc(sizeof(node), GFP_KERNEL);
	tail = kmalloc(sizeof(node), GFP_KERNEL);
	drop_pack = kmalloc(sizeof(node), GFP_KERNEL);        
        conf_file = filp_open("conf.txt", O_RDONLY, 0);
        //fscanf(conf_file, "%d", &is_wred);
	//fscanf(conf_file, "%d %d %f %f ", &minthred, &maxthred, &wqred, &maxpbred);
	//fscanf(conf_file, "%d", &constant);
	int n;
	int i = 0;
	//fscanf(conf_file, "%d",&n);
	for(i = 0; i < n; i++){
		//fscanf(conf_file, "%d",&minths[i]);
		//fscanf(conf_file, "%d",&maxths[i]);
		//fscanf(conf_file, "%f",&wqs[i]);
		//fscanf(conf_file, "%f",&maxpbs[i]);
	}
	nfhops.hook = aqm_hook;				//hook function
	nfhops.priority = NF_IP_PRI_FIRST;		//function registered for hughest priority
	nfhops.hooknum = 2;			//callback for this hook
	nfhops.pf = NFPROTO_IPV4; 			//protocol is IPv4
	nf_register_hook(&nfhops);
	return 0;
}

/* Kernel module exit handler */
//static int __exit void my_exit(void)
void cleanup_module(void){
    nf_unregister_hook(&nfhops);
	return 0;
}

//module_init(myinit);
//module_exit(my_exit);
