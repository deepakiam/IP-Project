#include <stdio.h>
#include <stdlib.h>
#include <red.h>

void main(Packet packet, int priority)
{
	int packet_count = -1;
	int avg_queue_size = 0;
	srand((unsigned) time(&t));
	time time, q_idle_time_start;
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
	
}
