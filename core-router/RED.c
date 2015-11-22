#include<linux/skbuff.h>
#include "Red.h"
#include <linux/time.h>

//drop_pack = head;		//constant used in drop_packet function

//MODULE_LICENSE("GPL");

long pa = 0;
long pb = 0;
long packet_count = 0;
int q_size_dec_count = 0;
long get_idle_time_interval(){
	struct timeval curr_time;
	unsigned long curr_time_ms;
	 do_gettimeofday(&curr_time); 	//gives microsecond value, which is what we might require/
	curr_time_ms = (u32)((curr_time.tv_sec*1000) - (sys_tz.tz_minuteswest * 60000) );
	return (curr_time_ms - q_idle_time_start_ms); 
}

long get_random_number(){
	long r;
	long rtemp;
	get_random_bytes(&r, sizeof(r));
	rtemp = r%100;
	if(rtemp < 0){
		rtemp = 0 - rtemp;
	}
	return rtemp;
}

void enqueue(struct q_node* node){
	counter++;
	if(counter%50 == 49){
		q_size_dec_count++;
		if(q_size_dec_count%3 == 1){
			queue_size -=30;
		} else if(q_size_dec_count%3 == 2) {
			queue_size-=35;
		}else {
			queue_size-=30;
		}
	}
//	if(counter%75 == 74){
//		queue_size -= 30;
//	}
	if(head == NULL && tail== NULL){
		printk(KERN_INFO "Enqueuing : head and tail NULL \n");
		head = tail = node;
		queue_size++;
		return;
	}
	printk(KERN_INFO "enqueuing : queue has something already\n");
	queue_size++;
	printk(KERN_INFO "queue-size : %lu\n", queue_size);
	node->next=tail;
	tail=node;
	return;
}

void dequeue(){
	if(head == NULL){
		printk(KERN_INFO "dequeuing but head is null");
		return;
	}
	if(head == tail){
		head = tail = NULL;
		queue_size--;
		do_gettimeofday(&q_idle_time_start);
		q_idle_time_start_ms =(u32) ((q_idle_time_start.tv_sec*1000) - (sys_tz.tz_minuteswest * 60000));
		printk(KERN_INFO "head and tail null. queue size : %lu\n", queue_size);
		return;
	}
	struct q_node* head_node = head;
	head = head->next;
	kfree(head_node);
	queue_size--;
	printk(KERN_INFO "dequeued packet. queue count: %lu\n", queue_size);
	return;
}

void drop_packets(){
	//the drop_pack pointer will continue to point to the same memory location, 
	//so we dont have to traverse the whole list again and again where we already deleted the marked packets.
	if (head == NULL)
		return;
	else if(drop_pack == NULL)
		drop_pack = head;
	struct q_node* temp = kmalloc(sizeof(q_node), GFP_KERNEL); 
	temp = drop_pack->next;
	while(temp != NULL)
	{
		if (temp->marked == true)
		{
			drop_pack->next = temp->next;
			kfree(temp);
			temp = drop_pack->next;
			queue_size--;
		}
		else
		{
			drop_pack = drop_pack->next;
			temp = temp->next;
		}
	}
	
}

struct q_node* red(struct sk_buff* packet, long minth, long  maxth, long wq, long maxpb){
	long  randm;
	long m;
	struct q_node* new_node = kmalloc(sizeof(q_node), GFP_KERNEL);
	new_node->packet = packet;
	new_node->marked = 0;				//default marking is false
	new_node->next = NULL;
	
                if (queue_size == 0){
			return new_node;
		}else {
                        avg_queue_size = ((100-wq)*avg_queue_size) + (wq * queue_size);
			avg_queue_size /= 100;
			printk(KERN_INFO "average queue size : %lu\n", avg_queue_size);
		}
                //else
                //{
                    //    m = constant * get_idle_time_interval();
                  //      avg_queue_size = (((100- wq)) * avg_queue_size)/100;
					
		//}
                printk(KERN_INFO "ave size : %lu   minth : %lu    maxth : %lu\n", avg_queue_size, minth , maxth);
                if (minth < avg_queue_size && avg_queue_size < maxth)
                {
				packet_count++;
                      		printk(KERN_INFO "packet average queue size in range. packet count : %lu\n", packet_count);
                     		pb = (maxpb * (avg_queue_size - minth))/(maxth - minth);
                       		pa = (100*pb)/(100 - (packet_count * pb));
				printk(KERN_INFO "pa :%lu \n", pa);
                      		randm = get_random_number();
				printk(KERN_INFO "random number : %lu  pb : %lu  pa : %lu \n", randm, pb, pa);
                      		if (randm < pa)
                       		{
                              		new_node->marked = 1;	//marked the packet for deletion
                               		printk(KERN_INFO "marking for dropping\n");
					packet_count = 0;
                       		}
                }
                else if (maxth <= avg_queue_size)
                {
                        				printk(KERN_INFO "max threshhold exceeded\n");
							new_node->marked = 1;				//marked the packet for deletion
							packet_count = 0;
                }
                else
                {
                      	printk(KERN_INFO "packet count reset\n");	
			packet_count = 0;
                }
				
	return new_node;
}

