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

void enqueue(node* node);

void dequeue();

void drop_packet(node* node); //should it be node specific?? wont it just check the mark and drop it while traversing the linked list?

int getTimeInterval();





