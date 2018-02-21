#include "header.h"

void schedule_network(struct TRANS_EVENT *event, int packet_type, counter_t sim_cycle);

void eventq_insert(struct TRANS_EVENT *event)    
{
        struct TRANS_EVENT *ev, *prev;     
        for (prev = NULL, ev = event_queue; ev && ((ev->when) <= (event->when)); prev = ev, ev = ev->next);
        if (prev)
        {
                event->next = ev;
                prev->next = event;
        }
        else
        {
                event->next = event_queue;
                event_queue = event;
        }
}

void event_create(int packet_type, int operation, int threadid)
{
	/* create a new transaction, send it into network */
	int src, des;
	src = threadid;
	des = des_gen(src);	/* generate a different destination */
	MissNo ++;
	struct TRANS_EVENT *event = calloc(1, sizeof(struct TRANS_EVENT));
	if(event == NULL) panic("Out of Virtual Memory");
	event->src1 = src/mesh_size;
	event->src2 = src%mesh_size;
	event->des1 = des/mesh_size;
	event->des2 = des%mesh_size;
	event->operation = operation;
	event->start_cycle = sim_cycle;
	event->parent = NULL;
	event->childcount = 0;
	event->when = sim_cycle + WAIT_TIME;
	event->Threadid = threadid;
	event->missno = MissNo;
	packet_type = meta_packet_size;	

	/* check if the input buffer have space, drop the event if out of space */
	if(0 ||
#ifdef POPNET
		popnetBufferSpace(event->src1, event->src2)
#endif
#ifdef OPTICAL_INTERCONNECT
		opticalBufferSpace(event->src1, event->src2)
#endif
	)
	{
		mshr_entry[threadid] --;
		free(event);
		return;
	}
	/* statistics */
	if(operation == MISS_READ)
		total_read_misses ++;
	else if(operation == MISS_WRITE)
		total_write_misses ++;
	else
		total_write_upgrades ++;

	/* call the network function */
	schedule_network(event, packet_type, sim_cycle);

	/* keep record of every event */
	eventq_insert(event);
}

void schedule_network(struct TRANS_EVENT *event, int packet_type, counter_t sim_cycle)
{
	msgno ++;
	/* call the network function to insert a packet into network */
	#ifdef OPTICAL_INTERCONNECT
	directMsgInsert(event->src1, event->src2, event->des1, event->des2, sim_cycle, packet_type, msgno, 0 /* addr */, event->operation, 0 /* delay */, 0 /* L2 missflag */, 0 /* prefetch */);	
	#endif
	#ifdef POPNET
		popnetMessageInsert(event->src1, event->src2, event->des1, event->des2, sim_cycle, packet_type, msgno, 0);
	#endif
	event->msgno = msgno;

	if(packet_type == meta_packet_size)
		total_meta_packets ++;
	else 	
		total_data_packets ++;
//	printf("insert msgno %lld\n", msgno);

	return;
}

void dir_eventq_nextevent(void)                  // return next directory event occured, call a function working on event
{
        struct TRANS_EVENT *event, *ev, *next, *prev;
        int i;
	event = event_queue;
       	ev = NULL;
       	while (event!=NULL)
       	{
       	        next = event->next;
       	                                                                                                                                                                                             
       	        if (event->when <= sim_cycle)
       	        {
       	                if(event == event_queue)
       	                {
       	                        event_queue = event->next;
				event->next = NULL;
       	                        dir_fifo_enqueue(event, 1);
       	                        event = event_queue;
       	                }
       	                else
       	                {
       	                        ev->next = event->next;
				event->next = NULL;
       	                        dir_fifo_enqueue(event, 1);
       	                        event = ev->next;
       	                }
       	        }
               	else
               	{
                      		ev = event;
                      		event = event->next;
               	}
       	
	}
}
