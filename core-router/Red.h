#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/time.h>
#include<linux/skbuff.h>

//MODULE_LICENSE("GPL");

struct q_node{
struct sk_buff *packet;
bool marked;
struct q_node* next;
};

typedef struct q_node q_node;

extern long is_wred;
extern int max_queue_size;
extern int priority;
extern long counter;

/*variables used in RED.c*/

extern long avg_queue_size;			//the average queue size
extern long queue_size;				//current queue size
extern long packet_count; 			//number of packets since last marked packet
extern long pa;					//probability values for marking the packets for dropping
extern long pb; 
								//will be marked if the average queue size is between the two threshold values

extern struct timeval q_idle_time_start;	//global constant so that it can be used in all functions
extern unsigned long q_idle_time_start_ms;

extern struct q_node* head;
extern struct q_node* tail;
extern struct q_node* drop_pack;
extern long constant;			//dummy value for the constant for linear function of difference between current time and queue idle time (used in red.c)

struct q_node* red(struct sk_buff* packet, long maxth, long minth, long wq, long  maxp);

void deq_drop_pack(void);

void enqueue(struct q_node* node);

void dequeue(void);

void drop_packets(void);

int getTimeInterval(void);

long get_idle_time_interval(void);

long get_random_number(void);




