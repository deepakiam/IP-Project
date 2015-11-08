#include <stdio.h>
#include <stdlib.h>
#include "Red.h"
#include <linux/time.h>

//node* head=NULL;
//node* tail=NULL;
struct timeval q_idle_time_start = 0;	//global constant so that it can be used in all functions
node* drop_pack = head;		//constant used in drop_packet function


long get_idle_time_interval(){
	struct timeval curr_time_ms = gettimeofday(NULL, NULL); 	//gives microsecond value, which is what we might require/
	return (curr_time_ms - q_idle_time_start); 
}

void enqueue(node* node){
	if(head == NULL && tail== NULL){
		head = tail = node;
		queue_size++;
		return;
	}
	node->next=tail;
	tail=node;
	queue_size
}

void dequeue(){
	if(head == NULL){
		return;
	}
	if(head == tail){
		head = tail = NULL;
		queue_size--;
		q_idle_time_start = gettimeofday(NULL, NULL);		//re-initialize idle time to current time
		return;
	}
	node* head_node = head;
	head = head->next;
	free(head_node);
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
	node* temp = drop_pack->next;
	while(temp != NULL)
	{
		if (temp->marked == true)
		{
			drop_pack->next = temp->next;
			free(temp);
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

node* red(sk_buff* packet, int maxth, int minth, float wq, float maxpb){
	int m, randm;
	
	node* new_node = (node*)malloc(sizeof(node));
	new_node->packet = packet;
	new_node->marked = 0;				//default marking is false
	new_node->next = NULL;

                if (head == NULL && tail == NULL)
						return new_node;
				else if (head != NULL && tail != NULL)
                        avg_queue_size = ((1-wq)*avg_queue_size) + (wq * queue_size);
                else
                {
                        int m = constant * (time - q_idle_time_start);
                        avg_queue_size = ((1 - wq) ^ m) * avg_queue_size;
					
				}
                
                if (minth <= avg_queue_size && avg_queue_size < maxth)
                {
                      		packet_count++;
                     		pb = maxpb * ((avg_queue_size - minth)/(maxth - minth));
                       		pa = pb / (1 - (packet_count * pb));

                      		randm = rand() % 100;

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

