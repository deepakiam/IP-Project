#include<linux/skbuff.h>
#include "Red.h"
#include <linux/time.h>

//drop_pack = head;		//constant used in drop_packet function

//MODULE_LICENSE("GPL");

long get_idle_time_interval(){
	struct timeval curr_time;
	unsigned long curr_time_ms;
	 do_gettimeofday(&curr_time); 	//gives microsecond value, which is what we might require/
	curr_time_ms = (u32)((curr_time.tv_sec*1000) - (sys_tz.tz_minuteswest * 60000) );
	return (curr_time_ms - q_idle_time_start_ms); 
}

int get_random_number(){
	int r;
	get_random_bytes(&r, sizeof(r));
	return(r%100);
}

void enqueue(struct q_node* node){
	if(head == NULL && tail== NULL){
		head = tail = node;
		queue_size++;
		return;
	}
	node->next=tail;
	tail=node;
	return;
}

void dequeue(){
	if(head == NULL){
		return;
	}
	if(head == tail){
		head = tail = NULL;
		queue_size--;
		do_gettimeofday(&q_idle_time_start);
		q_idle_time_start_ms =(u32) ((q_idle_time_start.tv_sec*1000) - (sys_tz.tz_minuteswest * 60000));
		return;
	}
	struct q_node* head_node = head;
	head = head->next;
	kfree(head_node);
	queue_size--;
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

struct q_node* red(struct sk_buff* packet, long maxth, long  minth, long wq, long maxpb){
	int  randm;
	long m;
	struct q_node* new_node = kmalloc(sizeof(q_node), GFP_KERNEL);
	new_node->packet = packet;
	new_node->marked = 0;				//default marking is false
	new_node->next = NULL;
	
                if (head == NULL && tail == NULL)
			return new_node;
		else if (head != NULL && tail != NULL){
                        avg_queue_size = ((100-wq)*avg_queue_size) + (wq * queue_size);
			avg_queue_size /= 100;
		}
                else
                {
                        m = constant * get_idle_time_interval();
                        avg_queue_size = (((100- wq)) * avg_queue_size)/100;
					
		}
                
                if (minth <= avg_queue_size && avg_queue_size < maxth)
                {
                      		packet_count++;
                     		pb = 100*maxpb * ((avg_queue_size - minth)/(maxth - minth));
                       		pa = 100*pb / (100 - (packet_count * pb));

                      		randm = get_random_number();

                      		if (randm <= (pa * 100))
                       		{
                              		new_node->marked = 1;	//marked the packet for deletion
                               		packet_count = 0;
                       		}
                }
                else if (maxth <= avg_queue_size)
                {
                        
							new_node->marked = 1;				//marked the packet for deletion
							packet_count = 0;
                }
                else
                {
                      		packet_count = -1;
                }
				
	return new_node;
}

