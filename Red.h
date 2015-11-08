#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/time.h>
#include<linux/skbuff.h>

typedef struct{
struct sk_buff *packet;
bool marked;
struct node* next;
}node;

int is_wred = 0;
int max_queue_size = 2000;
int priority = 0;

/*variables used in RED.c*/

int avg_queue_size = 0;			//the average queue size
int queue_size = 0;				//current queue size
int packet_count = -1; 			//number of packets since last marked packet
float pa = 0.0;					//probability values for marking the packets for dropping
float pb = 0.0; 
								//will be marked if the average queue size is between the two threshold values

struct timeval q_idle_time_start;	//global constant so that it can be used in all functions
unsigned long q_idle_time_start_ms = 0.0;

struct node* head=NULL;
struct node* tail=NULL;
struct node*  drop_pack = NULL;
int constant;			//dummy value for the constant for linear function of difference between current time and queue idle time (used in red.c)

struct node* red(struct sk_buff* packet, int maxth, int minth, float wq, float maxp);

void enqueue(node* node);

void dequeue(void);

void drop_packets(void);

int getTimeInterval(void);

long get_idle_time_interval(void);





