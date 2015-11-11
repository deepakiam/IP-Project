#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/time.h>
#include<linux/skbuff.h>

struct q_node{
struct sk_buff **packet;
bool marked;
struct q_node* next;
};

typedef struct q_node q_node;

extern int is_wred;
extern int max_queue_size;
extern int priority;

/*variables used in RED.c*/

extern int avg_queue_size;			//the average queue size
extern int queue_size;				//current queue size
extern int packet_count; 			//number of packets since last marked packet
extern int pa;					//probability values for marking the packets for dropping
extern int pb; 
								//will be marked if the average queue size is between the two threshold values

extern struct timeval q_idle_time_start;	//global constant so that it can be used in all functions
extern unsigned long q_idle_time_start_ms;

extern struct q_node* head;
extern struct q_node* tail;
extern struct q_node* drop_pack;
extern int constant;			//dummy value for the constant for linear function of difference between current time and queue idle time (used in red.c)

struct q_node* red(struct sk_buff** packet, int maxth, int minth, int wq, int maxp);

void enqueue(struct q_node* node);

void dequeue(void);

void drop_packets(void);

int getTimeInterval(void);

long get_idle_time_interval(void);

int get_random_number(void);




