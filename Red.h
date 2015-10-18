#include<netinet/ip.h>
#include<netinet/in.h>
#include<<netinet/tcp.h>

typedef struct{
const unsigned char *packet;
bool mark;
struct node* next;
} node;

void enqueue(node* node);

void dequeue();

void drop_packet(node* node);






