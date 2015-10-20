#include <stdio.h>
#include <stdlib.h>
#include <Red.h>



long get_idle_time_interval(){
	time_t curr_time = time(NULL);
	curr_time_ms = curr_time*1000;
	return (curr_time_ms - (q_idle_time_start*1000)); 
}

void enqueue(node* node){
	if(head == NULL && tail== NULL){
		head = tail = node;
		return;
	}
	node->next=tail;
	tail=node;
}

void dequeue(){
	if(head == NULL){
		return;
	}
	if(head == tail){
		head = tail = NULL;
		return;
	}
	node* head_node = head;
	head = head->next;
	free(head_node);
}

node* red(sk_buff* packet){
	int m, constant, queue_size, minthr, maxthr, randm;
        float pa, pb, wq, maxpb;

                if (non-empty queue)
                        avg_queue_size = ((1-wq)*avg_queue_size) + (wq * queue_size);
                else
                {
                        int m = constant * (time - q_idle_time_start);
                        avg_queue_size = ((1 - wq) ^ m) * avg_queue_size;
                }
                if (minthr <= avg_queue_size && avg_queue_size < maxthr)
                {
                        packaet_count++;
                        pb = maxpb * ((avg_queue_size - minthr)/(maxthr - minthr));
                        pa = pb / (1 - (packet_count * pb));

                        randm = rand() % 100;

                        if (randm <= (pa * 100))
                        {
                                mark the arriving packet;
                                packet_count = 0;
                        }
                }
                else if (maxthr <= avg_queue_size)
                {
                        mark the arriving packet;
                        packet_count = 0;
                }
                else
                {
                        packet_count = -1;
                }
	node* new_node = (node*)malloc(sizeof(node));
	new_node->packet = packet;
	new_node->marked = 0;				//todo marking
	new_node->next = NULL;
	return new_node;
}

