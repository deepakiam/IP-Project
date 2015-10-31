#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <linux/sk_buff.h>

typedef struct{
sk_buff *packet;
bool marked;
struct node* next;
} node;

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

struct timeval q_idle_time_start = 0;	//global constant so that it can be used in all functions

node* head=NULL, tail=NULL, drop_pack;
int constant;			//dummy value for the constant for linear function of difference between current time and queue idle time (used in red.c)



void enqueue(node* node);

void dequeue();

void drop_packets();

int getTimeInterval();





