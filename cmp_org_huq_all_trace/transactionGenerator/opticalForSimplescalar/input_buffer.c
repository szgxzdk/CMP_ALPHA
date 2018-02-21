
#include "point-point.h"
/* *************************** INPUT_BUFFER.C **********************************/
/* insert packets into input buffer */
void input_buffer_add(Packet packet, counter_t time_stamp)
{
    int i;
    long src = packet.src;

    //if(input_buffer[src].flits_in_buffer > input_buffer_size)
      //  panic("NETWORK: There is no free entry to insert in input buffer");
    
    for (i = 0; i < input_buffer_size; i++)
    {
    	if(packet.retry != 0)
        {
		if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.msgno == packet.msgno)
		{
			input_buffer[src].InputEntry[i].packet.retry = packet.retry;
			input_buffer[src].InputEntry[i].time_stamp = time_stamp;
			return;
		}
	}
	else
	{
		if(!input_buffer[src].InputEntry[i].isValid)
		{
		    input_buffer[src].InputEntry[i].isValid = 1;
		    input_buffer[src].InputEntry[i].packet = packet;
		    input_buffer[src].InputEntry[i].time_stamp = time_stamp;

		    input_buffer[src].flits_in_buffer += packet.packet_size;
		    input_buffer[src].packets_in_buffer ++;
		    if(max_packets_inbuffer < input_buffer[src].packets_in_buffer)
			max_packets_inbuffer = input_buffer[src].packets_in_buffer;
		    if(max_flits_inbuffer < input_buffer[src].flits_in_buffer)
			max_flits_inbuffer = input_buffer[src].flits_in_buffer;
		    return;
		}
	}
    }
    panic("NETWORK: There is no free entry to insert in input buffer");
}

/* free packet free input buffer */
void input_buffer_free(Packet packet, counter_t sim_cycle)
{
	int i, n, pendingnum, m, Msgno = 0;
	long src = packet.src;
        for (i=0;i<input_buffer_size;i++)
        {
                if(input_buffer[src].InputEntry[i].isValid
                   && (input_buffer[src].InputEntry[i].packet.msgno == packet.msgno))
                {
			if(input_buffer[src].InputEntry[i].time_stamp < sim_cycle)
				panic("NETWORK: INPUT_BUFFER_FREE: packets should be retry first");
			Packet packet = input_buffer[src].InputEntry[i].packet;
                        input_buffer[src].InputEntry[i].isValid = 0;
			input_buffer[src].flits_in_buffer -= packet.packet_size;
			input_buffer[src].packets_in_buffer --;
			input_buffer_num[src] -= packet.packet_size;
			if(packet.retry == 1)
			{
				if(packet.packet_type == DATA)
				{
					switch (packet.retry_num)
					{
						case 0: panic("NETWORK: retry time can not be zero if the retry flag is on!");
						case 1: data_retry_num[0]++; 
							break;
						case 2: data_retry_num[1]++;
							break;
						case 3: data_retry_num[2]++;
							break;
						case 4: data_retry_num[3]++;
							break;
						default: if(packet.retry_num > 4) 
								data_retry_num[4]++;
							break;	
					}
				}
				else
				{
					switch (packet.retry_num)
					{
						case 0: panic("NETWORK: retry time can not be zero if the retry flag is on!");
						case 1: meta_retry_num[0]++; 
							break;
						case 2: meta_retry_num[1]++;
							break;
						case 3: meta_retry_num[2]++;
							break;
						case 4: meta_retry_num[3]++;
							break;
						default: if(packet.retry_num > 4) 
								meta_retry_num[4]++;
							break;	
					}
				}
				if(max_packet_retry < packet.retry_num)
					max_packet_retry = packet.retry_num;
			}	
#ifdef CONF_RES_ACK
			for(n=0; n<PENDING_SIZE; n++)
			{
				pendingnum = input_buffer[src].InputEntry[i].packet.pending[n];
				if(pendingnum)
				{
					input_buffer[src].InputEntry[i].packet.pending[n] = 0;
					if(n == 0)
					{ /* retry the first pending packet */
						retry_packet(pendingnum, sim_cycle, src);
						Msgno = pendingnum;
					}
					else
					{ /* put the rest packet pending to the first packet */
						if(n == 1)
							pending_num[0] ++;
						else if(n == 2)
							pending_num[1] ++;
						else if(n > 2)
							pending_num[2] ++;

						if(Msgno == 0)
							panic("NETWORK: first pending packet msgno should be none zero!");
						for(m=0;m<input_buffer_size;m++)
						{
							if(input_buffer[src].InputEntry[m].isValid && input_buffer[src].InputEntry[m].packet.msgno == Msgno)
							{
								if(n == 1 && input_buffer[src].InputEntry[m].packet.pending[n-1] != 0)
									panic("NETWORK: pending parent is not the real father!");  
								input_buffer[src].InputEntry[m].packet.pending[n-1] = pendingnum;
								break;
							}
						}
					}	
				}
				else
					break;
			}
#endif
                        break;
                }
        }
	
}

#ifdef CONF_RES_ACK
int is_appending_addr(Packet packet)
{
    long src = packet.src;
    int i, match = 0, n;
    for (i=0; i<input_buffer_size; i++)
    {
        if(input_buffer[src].InputEntry[i].isValid)
        {
		if(((input_buffer[src].InputEntry[i].packet.addr >> cache_dl1_set_shift) == (packet.addr >> cache_dl1_set_shift))
		     && (input_buffer[src].InputEntry[i].time_stamp != WAIT_TIME) && (input_buffer[src].InputEntry[i].packet.des == packet.des)
		     && (input_buffer[src].InputEntry[i].packet.msgno != packet.msgno))
                {
                        if(match != 0)  panic("NETWORK: INPUT_BUFFER: a miss address belongs to more than two INPUT Entries");
                        match = 1;
			for(n=0; n<PENDING_SIZE; n++)
			{
				if(input_buffer[src].InputEntry[i].packet.pending[n] == 0)
				{
					pending_packet_num ++;
					input_buffer[src].InputEntry[i].packet.pending[n] = packet.msgno;
					break;
				}
				else if(n == PENDING_SIZE - 1)
					panic("NETWORK: pending buffer is full!");
			}
                }
        }
    }
    return match;
}

int retry_pending_check(Packet packet)
{ /* check if there are some previous packets are conflicting over # times, then this packet will be pending until the previous on are resolved*/
	int i;
	int src = packet.src;
	for(i=0; i<input_buffer_size; i++)
	{
		if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.des == packet.des
		   && input_buffer[src].InputEntry[i].packet.msgno < packet.msgno && input_buffer[src].InputEntry[i].packet.retry_num > 2000)
			return 1;
	}
	return 0;
}


void conflict_bin_count(int src, int des, int data_id, counter_t sim_cycle, int wb_flag, int msgno)
{
	if((sim_cycle - bin_count_end[des][data_id]) >= 5)
	{
		data_conf_involves[des][data_id] = 0;
		write_back_involves[des][data_id] = 0;
		bin_count_start[des][data_id] = sim_cycle;
		bin_count_end[des][data_id] = sim_cycle;
		data_conf_involves[des][data_id] ++;			
		
		if(sim_cycle > 3000000)
		{
			//fprintf(fp_trace, "conflict begin\n");
			//fprintf(fp_trace, "des %d, src %d, packet msgno is %d, at sim_cycle %d\n", des, src, msgno, sim_cycle);
		}
		if(wb_flag == 1)
			write_back_involves[des][data_id] ++;
	}
	else
	{
		data_conf_involves[des][data_id] ++;			
		bin_count_end[des][data_id] = sim_cycle;
		//if(sim_cycle > 3000000)
			//fprintf(fp_trace, "des %d, src %d, packet msgno is %d, at sim_cycle %d\n", des, src, msgno, sim_cycle);
		if(wb_flag == 1)
			write_back_involves[des][data_id] ++;
	}
}

void conlict_bincount_collect(counter_t sim_cycle)
{
	int i, j;
	for(i=0; i<PROCESSOR; i++)
	{
		for(j=0; j<data_receivers; j++)
		{
			if((sim_cycle - bin_count_end[i][j]) >= 5)
			{
				if(data_conf_involves[i][j])
				{
					switch (data_conf_involves[i][j])
					{
						case 0: panic("NETWORK: no/one data packet can not conflict with no one!\n"); 
						case 1: data_conf_inv_bincount[6] ++; break; 
						case 2: data_conf_inv_bincount[0] ++; break;
						case 3: data_conf_inv_bincount[1] ++; break;
						case 4: data_conf_inv_bincount[2] ++; break;
						case 5: data_conf_inv_bincount[3] ++; break;
						case 6:
						case 7: 
						case 8: data_conf_inv_bincount[4] ++; break; 
						default: data_conf_inv_bincount[5] ++; break;
					}
					if(data_conf_involves[i][j] > max_conf)
						max_conf = data_conf_involves[i][j];
					if(write_back_involves[i][j] == 1)
						write_back_inv_bincount[0] ++;
					else if(write_back_involves[i][j] == 2)
						write_back_inv_bincount[1] ++;
					else if(write_back_involves[i][j] > 2)
						write_back_inv_bincount[2] ++;
					//if(i == 0)
					//fprintf(fp_trace, "des is %d, conflict end at sim_cycle %d\n", i, sim_cycle);
				}
				data_conf_involves[i][j] = 0;
				write_back_involves[i][j] = 0;
			}
		}
	}
}

void input_buffer_lookup(long src, counter_t sim_cycle)
{
	int i = 0;
	for (i=0; i<input_buffer_size; i++)
	{
		if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].time_stamp < sim_cycle)
		{	
			Packet packet = input_buffer[src].InputEntry[i].packet;	
			int src = packet.src; 
			int des = packet.des;
			int data_id = packet.data_id;
			counter_t schedule_t;


			/* checking if there are some previous packets are conflicting over # times, then this packet will be pending until the previous one are resolved*/
			if(retry_pending_check(packet))
				input_buffer[src].InputEntry[i].time_stamp = sim_cycle + 30; //we can use some other algorisms here
			else
			{
#ifdef PREORITY_PACKETS
				if(packet.retry_num > 3 && packet.prefetch == 2)
					input_buffer[src].InputEntry[i].time_stamp = sim_cycle + 30; //we can use some other algorisms here
#endif
				/* bin count for data conflict involves */
				if(packet.packet_type == DATA)
				{
					int wb_flag = 0;
					conflict_bin_count(src, des, data_id, sim_cycle, wb_flag, packet.msgno);
				}
				/* bin count end */			

				/* statistics collected */
				if(packet.retry != 1)
				{
					if(packet.packet_type == DATA)
						data_conflict_packet_num ++;
					else
						meta_conflict_packet_num ++;
				}
				if(packet.packet_type == DATA)
					data_retry_packet_num ++;
				else	
					meta_retry_packet_num ++;

				input_buffer[src].InputEntry[i].packet.retry = 1;
				input_buffer[src].InputEntry[i].packet.retry_num ++;
				packet.retry_num ++;
	
				/* retry time out algorisms */
				schedule_t = retry_algorithms(packet, sim_cycle);		
				retry_packet(packet.msgno, schedule_t, src);
			}
		}
	}
}

counter_t retry_algorithms(Packet packet, counter_t sim_cycle)
{
	int schedule_t, t1, data_transfer_time, slot_num, mytime;
	if(data_chan_timeslot == 1)
	{ /* time slot for data channel */
		if(packet.packet_type == DATA)
		{
			/* time slot for retry data packets */
			schedule_t = sim_cycle + laser_setup_time;
			data_transfer_time = data_packet_size/flit_tranfer_time;
			t1 = (schedule_t/data_transfer_time)*(data_transfer_time);
			if(t1<schedule_t)
				schedule_t = t1 + 5;

			if(retry_algr == 1)
			{
				/* paper - TBEB */
				mytime = packet_retry_algr(packet);
			}
			else
			{
				/* our exponential backoff algorithms: 0-none backoff; 1-backoff; 2-hybrid with backoff and none backoff ; 3-our TBEB*/
				if((exp_backoff == 0) || (exp_backoff == 2 && packet.retry_num <= 5))
					slot_num = first_slot_num;
				else if((exp_backoff == 1) || (exp_backoff == 2 && packet.retry_num > 5) || (exp_backoff == 3))
				{
					if(exp_backoff == 1)
						slot_num = first_slot_num *exponential_2(packet.retry_num-1);	
					else if(exp_backoff == 3) 
					{
						if(packet.retry_num < 10)
							slot_num = first_slot_num *exponential_2(packet.retry_num-1);	
						else
							slot_num = first_slot_num *exponential_2(10);
					}
					else 
						slot_num = first_slot_num * exponential_2(packet.retry_num-3);
				}
				mytime = rand()%(slot_num);
			}
			schedule_t = schedule_t - laser_setup_time + mytime*(data_transfer_time);
		}
		else 
		{
			if(exp_backoff == 0)
				schedule_t = sim_cycle + rand()%PROCESSOR;
			else
				schedule_t = sim_cycle + rand()%(first_slot_num * exponential_2(packet.retry_num-1));
		}
	}
	else
	{
		if(packet.packet_type == DATA)
		{
			if(exp_backoff == 0)
				slot_num = 70;
			else if(exp_backoff == 1)
				slot_num = 10*exponential_2(packet.retry_num-1);

			schedule_t = sim_cycle + rand()%(slot_num);
		}
		else

		schedule_t = sim_cycle + rand()%(PROCESSOR);
	}
	return schedule_t;
} 

void retry_packet(int msgno, counter_t sim_cycle, long src)
{
	int i = 0;
	for (i=0; i<input_buffer_size; i++)
	{
		if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.msgno == msgno)
		{
			Packet packet = input_buffer[src].InputEntry[i].packet;

			if(packet.retry == 0)
			{
				packet.retry = 2;
				pending_delay += (sim_cycle - packet.start_time);
			}
			else
				packet.retry = 1;
			add_message(sim_cycle, EVG_MSG, packet);
			input_buffer[src].InputEntry[i].time_stamp += WAIT_TIME;
			return;
		}
	}
	panic("NETWORK: can not find the retry packet entry !");
}
#endif
/* *****************************************************************************/
