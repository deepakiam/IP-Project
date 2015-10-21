#include<netinet/ip.h>
#include<netinet/in.h>
#include<<netinet/tcp.h>
#include<time.h>
#include<linux/sk_buff.h>

typedef struct{
sk_buff *packet;
bool marked;
struct node* next;
} node;


/*variables used in RED.c*/

int avg_queue_size = 0;			//the average queue size
int queue_size = 0;				//current queue size
int packet_count = -1; 			//number of packets since last marked packet
float pa = 0.0;					//probability values for marking the packets for dropping
float pb = 0.0;
float maxpb = 0.7;				//dummy value for maximum probability with which the packets 
								//will be marked if the average queue size is between the two threshold values
float wq = 0.1;					//weight given to current queue size while calculating average queue size

node* head, tail, drop_pack;

#define maxthr		50			//dummy value for maximum threshold of packets in the buffer (used while marking the packets for deletion)
#define minthr		10			//dummy value for minimum threshold of packets in the buffer (used while marking the packets for deletion)
#define constant	10			//dummy value for the constant for linear function of difference between current time and queue idle time (used in red.c)



void enqueue(node* node);

void dequeue();

void drop_packet(node* node); //should it be node specific?? wont it just check the mark and drop it while traversing the linked list?

int getTimeInterval();





