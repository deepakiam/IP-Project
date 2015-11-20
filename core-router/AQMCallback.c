#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include "Red.h"

MODULE_LICENSE("GPL");

long minths[6];					//array to hold min threshold values for each class
long maxths[6];					//array to hold max threshold values for each class
long wqs[6];					//array to hold  weight for the moving ave for each class
long maxpbs[6];				//array to hold maxpb values for each class
long minthred, maxthred;				// min and max threshold for queue sizes for base RED implementation
long wqred, maxpbred;				//weight and maxpb for base RED implementation
struct iphdr* ip_header;
unsigned int tos;
unsigned int tos_mask;
unsigned int pr_class;
long n;
long constant;
long is_wred = 0;
long queue_size = 0;
long avg_queue_size = 0;
int total_packet_count = 0;
unsigned long q_idle_time_start_ms = 0;
struct timeval q_idle_time_start;
struct q_node* head;
struct q_node* tail;
struct q_node* drop_pack;
struct q_node* processed_packet;
int err = 0;
char read_1byte_data[2];
char read_2bytes_data[3];
char read_3bytes_data[4];
char read_4bytes_data[5];
static const unsigned int c1 = 4;
static const unsigned int c2 = 12;
static const unsigned int c3 = 28;
static const unsigned int c4 = 60;
static const unsigned int c5 = 124;
static const unsigned int c6 = 252;
int counter = 0;
int packets_marked = 0;
int c1_packets_marked = 0;
int c2_packets_marked = 0;
int c3_packets_marked = 0;
int c4_packets_marked = 0;
int c5_packets_marked = 0;
int c6_packets_marked = 0;
int packets_dropped = 0;
int c1_packets_dropped = 0;
int c2_packets_dropped = 0;
int c3_packets_dropped = 0;
int c4_packets_dropped = 0;
int c5_packets_dropped = 0;
int c6_packets_dropped = 0;
int stop_c1 = 0;
int stop_c2 = 0;
int stop_c3 = 0;
int stop_c4 = 0;
int stop_c5 = 0;
int stop_c6 = 0;
int stop = 0;

void deq_drop_pack(){
	//dequeue();
	packets_dropped++;
	return;
}

/* Create the AQM hook function */
static unsigned int aqm_hook(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *)){
	processed_packet = kmalloc(sizeof(q_node), GFP_KERNEL);
	total_packet_count++;
        if(is_wred == 1){
		printk(KERN_INFO "WRED implemented\n");
		ip_header = ip_hdr(skb);
		if(ip_header == NULL){
			printk(KERN_INFO "ip heder is NULL \n");
			return NF_ACCEPT;
		} else{
			printk(KERN_INFO "ip header read successful\n");
			tos =(ip_header->tos);		// get TOS field form the header
			printk(KERN_INFO "tos : %u \n",tos);
			tos_mask = 252;					// mask value set to  11111100 to get the DSCP bits
			pr_class = tos & tos_mask;				// bitwise and with mask value to get DSCP bits
			printk(KERN_INFO "priority class : %u \n", pr_class);
			if(pr_class == c1){
				processed_packet = red(skb,minths[0],maxths[0],wqs[0],maxpbs[0]);
				if(processed_packet->marked == 1) {
					c1_packets_marked++;
					stop_c1++;
				}	
				if(processed_packet->marked == 1 && queue_size > ((80*maxths[0])/100) && stop_c1>0){
					deq_drop_pack();
					c1_packets_dropped++;
					stop_c1--;
					queue_size--;
					return NF_DROP;
				}
				enqueue(processed_packet);
			} else if(pr_class == c2){
                        	processed_packet = red(skb,minths[1],maxths[1],wqs[1],maxpbs[1]);
				if(processed_packet->marked == 1) {
					c2_packets_marked++;
					stop_c2++;
				}	
				if(processed_packet->marked == 1 && queue_size > ((80*maxths[1])/100) && stop_c2>0){
					deq_drop_pack();
					c2_packets_dropped++;
					stop_c2--;
					queue_size--;
					return NF_DROP;
				}
				enqueue(processed_packet);
                        } else if(pr_class == c3){
                                processed_packet = red(skb,minths[2],maxths[2],wqs[2],maxpbs[2]);
				if(processed_packet->marked == 1) {
					c3_packets_marked++;
					stop_c3++;
				}	
				if(processed_packet->marked == 1 && queue_size > ((80*maxths[2])/100) && stop_c3>0){
					deq_drop_pack();
					c3_packets_dropped++;
					stop_c3--;
					queue_size--;
					return NF_DROP;
				}
				enqueue(processed_packet);
                        } else if(pr_class == c4){
      	        	        processed_packet = red(skb,minths[3],maxths[3],wqs[3],maxpbs[3]);
				if(processed_packet->marked == 1) {
					c4_packets_marked++;
					stop_c4++;
				}	
				if(processed_packet->marked == 1 && queue_size > ((80*maxths[3])/100) && stop_c4>0){
					deq_drop_pack();
					c4_packets_dropped++;
					stop_c4--;
					queue_size--;
					return NF_DROP;
				}
				enqueue(processed_packet);
       		        } else if(pr_class == c5){
                                processed_packet = red(skb,minths[4],maxths[4],wqs[4],maxpbs[4]);
				if(processed_packet->marked == 1) {
					c5_packets_marked++;
					stop_c5++;
				}	
				if(processed_packet->marked == 1 && queue_size > ((80*maxths[4])/100) && stop_c5>0){
					deq_drop_pack();
					c5_packets_dropped++;
					stop_c5--;
					queue_size--;
					return NF_DROP;
				}
				enqueue(processed_packet);
                        } else {
                                processed_packet = red(skb,minths[5],maxths[5],wqs[5],maxpbs[5]);
                                printk(KERN_INFO "class 6\n");
				if(processed_packet->marked == 1) {
					c6_packets_marked++;
					stop_c6++;
				}	
				if(processed_packet->marked == 1 && queue_size > ((80*maxths[5])/100) && stop_c6>0){
					deq_drop_pack();
					c6_packets_dropped++;
					stop_c6--;
					queue_size--;
					return NF_DROP;
				}
				enqueue(processed_packet);
			} /**else {
                                processed_packet = red(skb,minths[5],maxths[5],wqs[5],maxpbs[5]);
                               	printk(KERN_INFO "default class packet\n");
				if(processed_packet->marked == 1){
					packets_marked++;
					stop++;
				}
				if(processed_packet->marked == 1 && queue_size > ((80*maxths[5])/100) && stop_c6 > 0){
					deq_drop_pack();
					c6_packets_dropped++;
					stop_c6--;
					queue_size--;
					printk(KERN_INFO "dropping c6 packets\n");
					return NF_DROP;
				}
				enqueue(processed_packet);
			}**/
		}
        }else{
		printk(KERN_INFO "No WRED implemented");
		processed_packet = red(skb, minthred, maxthred, wqred, maxpbred);	//invoke red here for packet processing
		if(processed_packet->marked == 1){
			packets_marked++;
			stop++;
		}
		if(processed_packet->marked == 1 && queue_size > ((80*maxthred)/100) && stop > 0){
			deq_drop_pack();
			stop--;
			queue_size--;
			printk(KERN_INFO "dropping packets normal RED\n");
			return NF_DROP;
		}
		enqueue(processed_packet);
	}
	
	printk(KERN_INFO "executing the AQM hook function");
	return NF_ACCEPT;			   //after processing the packet, return NF_ACCEPT to let the packet pass
}

static unsigned int aqm_hook2(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *)){
	//printk(KERN_INFO "post routing hook\n");
	//counter++;
	//if (counter%50 == 0){
	//	queue_size -=20;
	//}
	//dequeue();
	return NF_ACCEPT;			   //after processing the packet, return NF_ACCEPT to let the packet pass
}

/* Register the hook function */
static struct nf_hook_ops nfhops;

static struct nf_hook_ops nfhops2;

/* Kernel module init handler */
//static int __init myinit(void);
int init_module(void){
	printk(KERN_INFO "aqm module loaded\n");
        char  *conf_file_path ="/etc/aqmconf";
	struct file* conf_file;
	int breads;
	mm_segment_t o_fs = get_fs();
	set_fs(get_ds());
	conf_file = filp_open(conf_file_path, O_RDONLY, 0);
	head = kmalloc(sizeof(q_node), GFP_KERNEL);
	tail = kmalloc(sizeof(q_node), GFP_KERNEL);
	drop_pack = kmalloc(sizeof(q_node), GFP_KERNEL);
	head = NULL;
	tail = NULL;
	drop_pack = NULL;
	if(IS_ERR(conf_file)){
		err = PTR_ERR(conf_file);
		printk(KERN_INFO "some error opening file!!\n");
	}       
	if(conf_file != NULL){
		printk(KERN_INFO "initializing parameters\n");
		breads = conf_file->f_op->read(conf_file, read_1byte_data, 2, &conf_file->f_pos);
		//sys_read(fd, read, 1);
		//printk(KERN_INFO "is_wred : %s\n", read_1byte_data[0]);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_1byte_data[1] = '\0';
		kstrtol(read_1byte_data, 10, &is_wred);
		printk(KERN_INFO "is_wred: %lu\n", is_wred);
		//sys_read(fd, read, 1);
		//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
		//breads = conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
		breads = conf_file->f_op->read(conf_file, read_3bytes_data, 4, &conf_file->f_pos);
		//breads = conf_file->f_op->read(conf_file, read_4bytes_data, 5, &conf_file->f_pos);
		read_3bytes_data[3] = '\0';
		kstrtol(read_3bytes_data, 10, &minthred);
		printk(KERN_INFO "minthred: %lu\n", minthred);
		//sys_read(fd, read, 1);
		//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
		//breads = conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
		breads = conf_file->f_op->read(conf_file, read_3bytes_data, 4, &conf_file->f_pos);
		//breads = conf_file->f_op->read(conf_file, read_4bytes_data, 5, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_3bytes_data[3] = '\0';
		kstrtol(read_3bytes_data, 10, &maxthred);
		printk(KERN_INFO "maxthred: %lu\n", maxthred);
		//sys_read(fd, read, 1);
		//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
		breads = conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_2bytes_data[2] = '\0';
		kstrtol(read_2bytes_data, 10, &wqred);
		printk(KERN_INFO "wqred: %lu\n", wqred);
		//sys_read(fd, read, 1);
		//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
		breads = conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_2bytes_data[2] = '\0';
		kstrtol(read_2bytes_data, 10, &maxpbred);
		printk(KERN_INFO "is_wred: %lu minthred: %lu maxthred: %lu wqred: %lu maxpred: %lu\n ", is_wred, minthred, maxthred, wqred, maxpbred);
		breads = conf_file->f_op->read(conf_file, read_1byte_data, 2, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_1byte_data[1] = '\0';
		kstrtol(read_1byte_data, 10, &constant);	
		long i = 0;
		//sys_read(fd, read, 1);
		//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
		breads = conf_file->f_op->read(conf_file, read_1byte_data, 2, &conf_file->f_pos);
		printk(KERN_INFO "bytes read : %d\n", breads);
		read_1byte_data[1] = '\0';
		kstrtol(read_1byte_data, 10, &n);
		for(i = 0; i < n; i++){
			//sys_read(fd, read, 1);
			//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
			conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
			read_2bytes_data[2] = '\0';
			kstrtol(read_2bytes_data, 10, &minths[i]);
			//sys_read(fd, read, 1);
			//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
			conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
			read_2bytes_data[2] = '\0';
			kstrtol(read_2bytes_data, 10, &maxths[i]);
			//sys_read(fd, read, 1);
			//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
			conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
			read_2bytes_data[2] = '\0';
			kstrtol(read_2bytes_data, 10, &wqs[i]);
			//sys_read(fd, read, 1);
			//conf_file->f_op->read(conf_file, read_1byte_data, 1, &conf_file->f_pos);
			conf_file->f_op->read(conf_file, read_2bytes_data, 3, &conf_file->f_pos);
			read_2bytes_data[2] = '\0';
			kstrtol(read_2bytes_data, 10, &maxpbs[i]);
			printk(KERN_INFO "minthred: %lu maxthred: %lu wqred: %lu maxpred: %lu\n ", minths[i], maxths[i], wqs[i], maxpbs[i]);
		}
	} else {
		printk(KERN_INFO "error reading file!!");
	}
	set_fs(o_fs);
	filp_close(conf_file, NULL);
        //fscanf(conf_file, "%d", &is_wred);
	//fscanf(conf_file, "%d %d %f %f ", &minthred, &maxthred, &wqred, &maxpbred);
	//fscanf(conf_file, "%d", &constant);
	//int n;
	//int i = 0;
	//fscanf(conf_file, "%d",&n);
	//for(i = 0; i < n; i++){
		//fscanf(conf_file, "%d",&minths[i]);
		//fscanf(conf_file, "%d",&maxths[i]);
		//fscanf(conf_file, "%f",&wqs[i]);
		//fscanf(conf_file, "%f",&maxpbs[i]);
	//}
	nfhops.hook = aqm_hook;				//hook function
	nfhops.priority = NF_IP_PRI_FIRST;		//function registered for hughest priority
	nfhops.hooknum = 2;			//callback for this hook
	nfhops.pf = NFPROTO_IPV4; 			//protocol is IPv4
	nf_register_hook(&nfhops);
	nfhops2.hook = aqm_hook2;				//hook function
	nfhops2.priority = NF_IP_PRI_LAST;		//function registered for hughest priority
	nfhops2.hooknum = 4;			//callback for this hook
	nfhops2.pf = NFPROTO_IPV4; 			//protocol is IPv4
	nf_register_hook(&nfhops2);
	return 0;
}

/* Kernel module exit handler */
//static int __exit void my_exit(void)
void cleanup_module(void){
    nf_unregister_hook(&nfhops);
    nf_unregister_hook(&nfhops2);
	printk(KERN_INFO "total packets served : %d\n", total_packet_count);
	printk(KERN_INFO "current queue size: %d\n", queue_size);
	printk(KERN_INFO "number of total packets dropped : %d\n", packets_dropped);
	printk(KERN_INFO "class 1 packets dropped : %d\n",c1_packets_dropped);
	//printk(KERN_INFO)
	printk(KERN_INFO "module unloaded from kernel!");
	return ;
}

//module_init(myinit);
//module_exit(my_exit);
