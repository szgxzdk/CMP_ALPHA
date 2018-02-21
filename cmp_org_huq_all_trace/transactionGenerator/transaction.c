#include "header.h"

void transaction_start(counter_t sim_cycle)
{ /* generate transactions at the sender */
	int operation, packet_type;
	int i, threadid;
	for(i=0;i<total_num_node;i++)
	{
		if(mshr_entry[i] > mshr_size)
			continue;

		if(generator(read_miss/total_num_node))
		{/* read miss transaction */
			packet_type = meta_packet_size;	
			operation = MISS_READ;
			threadid = i;
			event_create(packet_type, operation, threadid);
			mshr_entry[i] ++;
		}
		if(generator(write_miss/total_num_node))
		{/* write miss transaction */
			packet_type = meta_packet_size;	
			operation = MISS_WRITE;
			threadid = i;
			event_create(packet_type, operation, threadid);
			mshr_entry[i] ++;
		}
		if(generator(write_upgrade/total_num_node))
		{/* write update transaction */
			packet_type = meta_packet_size;	
			operation = WRITE_UPDATE;
			threadid = i;
			event_create(packet_type, operation, threadid);
			mshr_entry[i] ++;
		}
	}
}

void reset_ports()
{
	int i;
	for(i = 0; i < total_num_node; i++)
	{
		dir_fifo_portuse[i] = 0;
		l1_fifo_portuse[i] = 0;
	}
}

int dir_fifo_enqueue(struct TRANS_EVENT *event, int type)
{
    int threadid = event->des1*mesh_size + event->des2;
    if(event->operation == INV_MSG_READ || event->operation == INV_MSG_WRITE || event->operation == ACK_DIR_READ_SHARED || event->operation == ACK_DIR_READ_EXCLUSIVE || event->operation == ACK_DIR_WRITE || event->operation == ACK_DIR_WRITEUPDATE)
    {
    	if(l1_fifo_num[threadid] >= DIR_FIFO_SIZE)
		panic("L1 cache FIFO is full");
    	if(type == 0 && l1_fifo_num[threadid] >= dir_fifo_size)
		return 0;
	event->when = sim_cycle + cache_dl1_lat;
    	l1_fifo[threadid][l1_fifo_tail[threadid]] = event;
    	l1_fifo_tail[threadid] = (l1_fifo_tail[threadid]+1)%DIR_FIFO_SIZE;
    	l1_fifo_num[threadid]++;
    	return 1;
    }
    else
    {
    	if(dir_fifo_num[threadid] >= DIR_FIFO_SIZE)
		panic("DIR FIFO is full");
    	if(type == 0 && dir_fifo_num[threadid] >= dir_fifo_size)
		return 0;

	event->when = sim_cycle + cache_dl2_lat;
    	dir_fifo[threadid][dir_fifo_tail[threadid]] = event;
    	dir_fifo_tail[threadid] = (dir_fifo_tail[threadid]+1)%DIR_FIFO_SIZE;
    	dir_fifo_num[threadid]++;
    	return 1;
    }
}

void dir_fifo_dequeue()
{
    int i;
    /* L2 and dir queue */
    for(i = 0; i < MAXTHREADS; i++)
    {
	if(dir_fifo_num[i] <= 0 || dir_fifo_portuse[i] >= DIR_FIFO_PORTS)
		continue;
	while(1)
	{
		if(dir_fifo[i][dir_fifo_head[i]]->when > sim_cycle)
			break;
		if(event_operation(dir_fifo[i][dir_fifo_head[i]]) == 0)
			break;
		dir_fifo_portuse[i]++;
		dir_fifo_head[i] = (dir_fifo_head[i]+1)%DIR_FIFO_SIZE;
		dir_fifo_num[i]--;
		if(dir_fifo_portuse[i] >= DIR_FIFO_PORTS || dir_fifo_num[i] <= 0)
			break;		
	}
    }
    
    /* L1 queues */
    for(i = 0; i < MAXTHREADS; i++)
    {
	extern struct res_pool *fu_pool;

	if(l1_fifo_num[i] <= 0 || l1_fifo_portuse[i] >= DIR_FIFO_PORTS)
		continue;
	while(1)
	{
		if(l1_fifo[i][l1_fifo_head[i]]->when > sim_cycle)
			break;
		if(event_operation(l1_fifo[i][l1_fifo_head[i]]) == 0)
			break;
		l1_fifo_portuse[i]++;
		l1_fifo_head[i] = (l1_fifo_head[i]+1)%DIR_FIFO_SIZE;
		l1_fifo_num[i]--;
		if(l1_fifo_portuse[i] >= DIR_FIFO_PORTS || l1_fifo_num[i] <= 0)
			break;		
	}
    }
}
/*		DIR FIFO IMPLEMENTATION ENDS		*/

int event_operation(struct TRANS_EVENT *event)
{
	int packet_type;

	if(!(event->operation == ACK_DIR_READ_SHARED || event->operation == ACK_DIR_READ_EXCLUSIVE || event->operation == ACK_DIR_WRITE || event->operation == ACK_DIR_WRITEUPDATE))
	{
		if( 0 || 
#ifdef OPTICAL_INTERCONNECT
		opticalBufferSpace(event->des1, event->des2)
#endif
#ifdef POPNET	
		popnetBufferSpace(event->des1, event->des2)
#endif
		)
			return 0;
	}
	switch(event->operation)
	{
		case MISS_READ:
			event->parent_operation = event->operation;
			if(!generator(L2_missrate))
			{/* L2 cache hit*/
				if(generator(read_direct_reply))
				{ /* directory in share state, directory reply data to L1 directly*/
					int temp1, temp2;
					temp1 = event->src1;
					temp2 = event->src2;
					event->src1 = event->des1;
					event->src2 = event->des2;
					event->des1 = temp1;
					event->des2 = temp2;
					event->start_cycle = sim_cycle;	
					event->when = sim_cycle + WAIT_TIME;
					packet_type = data_packet_size;				
					event->operation = ACK_DIR_READ_EXCLUSIVE;
					/* schedule network event */
					schedule_network(event, packet_type, sim_cycle);
					eventq_insert(event);
				}
				else
				{ /* directory in exclusive/dirty state, thus directory has to forward the request to the remote node */
					struct TRANS_EVENT *new_event = calloc(1, sizeof(struct TRANS_EVENT));
					if(new_event == NULL) panic("Out of Virtual Memory");
					event->childcount ++;
					new_event->src1 = event->des1;
					new_event->src2 = event->des2;
					int des = des_gen(event->des1*mesh_size+event->des2);
					new_event->des1 = des/mesh_size; 
					new_event->des2 = des%mesh_size; 
					new_event->operation = INV_MSG_READ;
					new_event->start_cycle = sim_cycle;		
					new_event->parent = event;
					new_event->Threadid = event->Threadid;
					new_event->when = sim_cycle + WAIT_TIME;
					new_event->missno = event->missno;
					packet_type = meta_packet_size;
	
					/* schedule network event */
					schedule_network(new_event, packet_type, sim_cycle);
					eventq_insert(new_event);
				}		
			}	
			else
			{/* L2 cache miss */
				event->when = sim_cycle + mem_lat;	
				event->operation = WAIT_MEM_READ;
				event->start_cycle = sim_cycle;
				eventq_insert(event);
			}
			break;
		case MISS_WRITE:
			event->parent_operation = event->operation;
			if(!generator(L2_missrate))
			{/* L2 cache hit*/
				if(generator(write_direct_reply))
				{ /* no sharers */
					int temp1, temp2;
					temp1 = event->src1;
					temp2 = event->src2;
					event->src1 = event->des1;
					event->src2 = event->des2;
					event->des1 = temp1;
					event->des2 = temp2;
					event->start_cycle = sim_cycle;	
					event->when = sim_cycle + WAIT_TIME;
					packet_type = data_packet_size;				
					event->operation = ACK_DIR_WRITE;
					/* schedule network event */
					schedule_network(event, packet_type, sim_cycle);
					eventq_insert(event);
				}
				else
				{ /* directory in share/exclusive/dirty state, thus directory has to forward the request to the remote node */
					sharers = (sharers_generator(event->des1*mesh_size+event->des2));
					int threadid;
					for(threadid=0;threadid<total_num_node;threadid++)
					{
						if((sharers & (unsigned long long int)1<<threadid) == (unsigned long long int)1<<threadid)	
						{
							struct TRANS_EVENT *new_event = calloc(1, sizeof(struct TRANS_EVENT));
							if(new_event == NULL) panic("Out of Virtual Memory");
							event->childcount ++;
							new_event->src1 = event->des1;
							new_event->src2 = event->des2;
							new_event->des1 = threadid/mesh_size; 
							new_event->des2 = threadid%mesh_size; 
							new_event->operation = INV_MSG_WRITE;
							new_event->start_cycle = sim_cycle;		
							new_event->parent = event;
							new_event->Threadid = event->Threadid;
							new_event->when = sim_cycle + WAIT_TIME;
							new_event->missno = event->missno;
							packet_type = meta_packet_size;
			
							/* schedule network event */
							schedule_network(new_event, packet_type, sim_cycle);
							eventq_insert(new_event);
						}
					}
				}		
			}	
			else
			{/* L2 cache miss */
				event->when = sim_cycle + cache_dl2_lat;	
				event->operation = WAIT_MEM_READ;
				event->start_cycle = sim_cycle;
				eventq_insert(event);
			}
			break;
		case WRITE_UPDATE:
			event->parent_operation = event->operation;
		       	sharers = (sharers_generator(event->des1*mesh_size+event->des2));
		       	int threadid;
		       	for(threadid=0;threadid<total_num_node;threadid++)
		       	{
		       		if((sharers & (unsigned long long int)1<<threadid) == (unsigned long long int)1<<threadid)	
		       		{
		       			struct TRANS_EVENT *new_event = calloc(1, sizeof(struct TRANS_EVENT));
		       			if(new_event == NULL) panic("Out of Virtual Memory");
		       			event->childcount ++;
		       			new_event->src1 = event->des1;
		       			new_event->src2 = event->des2;
		       			new_event->des1 = threadid/mesh_size; 
		       			new_event->des2 = threadid%mesh_size; 
		       			new_event->operation = INV_MSG_WRITE;
		       			new_event->start_cycle = sim_cycle;		
		       			new_event->parent = event;
					new_event->Threadid = event->Threadid;
					new_event->missno = event->missno;
		       			new_event->when = sim_cycle + WAIT_TIME;
		       			packet_type = meta_packet_size;
		
		       			/* schedule network event */
		       			schedule_network(new_event, packet_type, sim_cycle);
		       			eventq_insert(new_event);
		       		}
		      	}		
			break;
		case INV_MSG_READ:
			if(generator(modified_downgrade))
			{
				event->operation = ACK_MSG_READ;
				packet_type = data_packet_size;
			} 
			else
			{
				event->operation = ACK_MSG_READUPDATE;
				packet_type = meta_packet_size;
			}
			int temp1, temp2;
			temp1 = event->src1;
			temp2 = event->src2;
			event->src1 = event->des1;
			event->src2 = event->des2;
			event->des1 = temp1;
			event->des2 = temp2;
			event->start_cycle = sim_cycle;
			event->when = sim_cycle +WAIT_TIME;

			schedule_network(event, packet_type, sim_cycle);
			eventq_insert(event);
				
			break;
		case INV_MSG_WRITE:
			if(generator(modified_invalidation))
			{
				event->operation = ACK_MSG_WRITE;
				packet_type = data_packet_size;
			} 
			else
			{
				event->operation = ACK_MSG_WRITEUPDATE;
				packet_type = meta_packet_size;
			}
			temp1 = event->src1;
			temp2 = event->src2;
			event->src1 = event->des1;
			event->src2 = event->des2;
			event->des1 = temp1;
			event->des2 = temp2;
			event->start_cycle = sim_cycle;
			event->when = sim_cycle +WAIT_TIME;

			schedule_network(event, packet_type, sim_cycle);
			eventq_insert(event);
				

			break;
		case ACK_MSG_READ:	
		case ACK_MSG_READUPDATE:	
		case ACK_MSG_WRITE:
		case ACK_MSG_WRITEUPDATE:
			event->parent->childcount --;
			if(event->parent->childcount == 0)
			{
				int temp1, temp2;
				temp1 = event->parent->src1;
				temp2 = event->parent->src2;
				event->parent->src1 = event->parent->des1;
				event->parent->src2 = event->parent->des2;
				event->parent->des1 = temp1;
				event->parent->des2 = temp2;
				event->parent->start_cycle = sim_cycle;
				event->parent->when = sim_cycle +WAIT_TIME;
				if(event->operation == ACK_MSG_READ || event->operation == ACK_MSG_READUPDATE)
				{
					packet_type = data_packet_size;
					event->parent->operation = ACK_DIR_READ_SHARED;
				}
				else if(event->operation == ACK_MSG_WRITE)
				{
					packet_type = data_packet_size;
					event->parent->operation = ACK_DIR_WRITE;
				}
				else
				{
					packet_type = meta_packet_size;
					event->parent->operation = ACK_DIR_WRITEUPDATE;
				}
				schedule_network(event->parent, packet_type, sim_cycle);
				eventq_insert(event->parent);
							
			}
			free(event);
			break;
		case ACK_DIR_READ_SHARED:
		case ACK_DIR_READ_EXCLUSIVE:
		case ACK_DIR_WRITE:
		case ACK_DIR_WRITEUPDATE:
			mshr_entry[event->Threadid] --;
			free(event);
			break;
		case WAIT_MEM_READ:
		{
			int temp1, temp2;
			temp1 = event->src1;
			temp2 = event->src2;
			event->src1 = event->des1;
			event->src2 = event->des2;
			event->des1 = temp1;
			event->des2 = temp2;
			event->start_cycle = sim_cycle;	
			event->when = sim_cycle + WAIT_TIME;
			packet_type = data_packet_size;				
			if(event->parent_operation == MISS_READ)
				event->operation = ACK_DIR_READ_EXCLUSIVE;
			else
				event->operation = ACK_DIR_WRITE;
			/* schedule network event */
			schedule_network(event, packet_type, sim_cycle);
			eventq_insert(event);
			break;
		}
		default:
			panic("can not be no operation here!\n");
	}
	return 1;
}



#ifdef OPTICAL_INTERCONNECT  
int MsgComplete(int w, int x, int y, int z, counter_t stTime, counter_t msgNo, counter_t transfer_time, counter_t meta_delay)
#endif
#ifdef POPNET
int popnetMsgComplete(long w, long x, long y, long z, counter_t stTime, long long int msgNo)
#endif
{
        struct TRANS_EVENT *event, *prev;
        event = event_queue;
        prev = NULL;
        if (event!=NULL)
        {
                while (event)
                {
                        if(event->src1 == w && event->src2 == x && event->des1 == y && event->des2 == z && event->start_cycle == stTime && msgNo == event->msgno)
                        {
				struct TRANS_EVENT *temp = event->next;
				if(dir_fifo_enqueue(event, 0) == 0)
					return 0;
				else
				{
					event->when = sim_cycle;

					if(prev == NULL)
						event_queue = temp;
					else
						prev->next = temp;

					/* call the event operation function to excute the transaction */
					totalHopCount ++;
					totalHopDelay += sim_cycle - event->start_cycle;
					//printf("release the msgno %lld\n", event->msgno);

 					return 1;
				}
                        }
                        prev = event;
                        event = event->next;
                }
        }
	panic("Error finding event for the returned message from network");
}
