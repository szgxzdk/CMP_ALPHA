#include "point-point.h"

/* ************************** SIM_MESS.C ***************************************/
/* handling EVG_MSG, adding packet into sending flit list, adding receive message into the message list */
counter_t time_schedule(Packet packet, counter_t current_time, int busy_flag, int *tran_type, int *meta_delay)
{
        long size = packet.packet_size;
        int p_type = packet.packet_type;
                                                                                                                                                                                                     
        long src = packet.src;
        long des = packet.des;
	int delay, packet_transfer_time, schedule_time, setup_time;

	packet_transfer_time = size/flit_tranfer_time;	
	if(packet.msgno == 23782 || packet.msgno == 23781 || packet.msgno == 23779)
		printf("here\n");

	setup_time = setup_delay(src, des,current_time, p_type);
	delay = setup_time + packet_transfer_time;	// tranfer time and laser setup time

	/* statistics collection */
	if(packet.retry != 1)
		steering_delay += setup_time;

	if(!busy_flag)
		schedule_time = current_time + delay;
	else
	{
		schedule_time = delay_counter[src] + delay;

		/* statistics collection */
		if(packet.retry != 1)
		{
			network_queue_delay += delay_counter[src] - current_time;
			if(p_type == DATA)
				data_network_queue_delay += delay_counter[src] - current_time;
			else
				meta_network_queue_delay += delay_counter[src] - current_time;
		}	
		else if(packet.retry == 1)
		{
			if(p_type == DATA)
				retry_data_queue ++;
			else
				retry_meta_queue ++;
		}
	}

	/* statistics collection */
	if((setup_time < laser_warmup_time) && (first_packet_time[src] != 0))
		last_packet_time[src] = schedule_time - packet_transfer_time + 1;
	else
	{
		first_packet_time[src] = schedule_time - packet_transfer_time + 1;
		last_packet_time[src] = first_packet_time[src];
	}
#ifdef PSEUDO_SCHEDULE_DATA
	if(packet.data_delay && !packet.miss_flag && p_type == DATA)
	{
		schedule_time += packet.data_delay;
		data_delay += packet.data_delay;
		data_packets_delay ++;
	}
	else if(packet.data_delay && p_type == DATA)
		data_packets_no_delay ++;
#endif
	if(p_type == DATA)
	{
		counter_t start_cycle = schedule_time - packet_transfer_time + 1;

		if(data_chan_timeslot == 1)
		{
			counter_t timeflag = start_cycle;

			if(start_cycle % packet_transfer_time != 0)
				start_cycle = (start_cycle/packet_transfer_time)*packet_transfer_time + packet_transfer_time; 
			timeslot_wait_time += start_cycle - timeflag;	
			schedule_time = start_cycle + packet_transfer_time - 1;
			//printf("start_time is %d\n", start_cycle);
		}
#ifdef PER_SCHEDULE
		if(start_cycle <= receiver_occ_time[des][packet.data_id])
		{
			data_confliction_delay += receiver_occ_time[des][packet.data_id] - start_cycle;
			confliction_delay += receiver_occ_time[des][packet.data_id] - start_cycle;
			start_cycle = receiver_occ_time[des][packet.data_id] + 1;
			schedule_time = start_cycle + packet_transfer_time - 1;
		}
		receiver_occ_time[des][packet.data_id] = schedule_time;
#endif				
	}
	else
	{
#ifdef PSEUDO_SCHEDULE
		if(packet.operation == 2 || packet.operation == 3 || packet.operation == 4
		   || packet.operation == 8 || packet.operation == 9)
		{
			if(packet.des != last_meta_packet_des[src])
			{
				if(schedule_time - last_meta_packet[src] < 5)
				{
					meta_wait_time[src] = 5 - (schedule_time - last_meta_packet[src]) + 1;
					meta_timeslot_wait_time += meta_wait_time[src];
					schedule_time = last_meta_packet[src] + 1+data_packet_size/flit_tranfer_time;
					meta_delay_packets ++;
					packet.transfer_time = schedule_time;
				}
			}
			last_meta_packet[src] = schedule_time;
			last_meta_packet_des[src] = des;
		}
#endif

#ifdef PSEUDO_SCHEDULE_REORDER
		if(packet.operation == 2 || packet.operation == 3 || packet.operation == 4
		   || packet.operation == 8 || packet.operation == 9)
		{
			if(packet.des != last_meta_packet_des[src])
			{
				if(delay_counter[src] < delay_counter_backup[src])
					schedule_time = schedule_time - delay_counter[src] + delay_counter_backup[src];
				if(schedule_time - last_meta_packet[src] < 5)
				{
					int flag = is_transmitter_backup_empty(src);
					meta_wait_time[src] = 5 - (schedule_time - last_meta_packet[src]) + 1;
					meta_timeslot_wait_time += meta_wait_time[src];
					schedule_time = last_meta_packet[src] + 1+data_packet_size/flit_tranfer_time;
					meta_delay_packets ++;
					packet.transfer_time = schedule_time;
					*tran_type = 1;
					backup_transfer_waittime[src] += meta_wait_time[src];
					add_message(current_time + backup_transfer_waittime[src], MSG_BACKUP, packet);
					if(flag)
						transmitter_backup_schedule[src] = schedule_time;
					delay_counter_backup[src] = schedule_time;
					last_packet_des_backup[src] = des;
				}
				else if(delay_counter[src] < delay_counter_backup[src])
					schedule_time = schedule_time + delay_counter[src] - delay_counter_backup[src];
			}
			if(last_meta_packet[src] < schedule_time)
			{
				last_meta_packet[src] = schedule_time;
				last_meta_packet_des[src] = des;
			}
		}
#endif
	}
#ifdef PSEUDO_SCHEDULE_REORDER
	if(*tran_type != 1)
	{
		if(((schedule_time >= transmitter_backup_schedule[src])||(packet.operation == 2 || packet.operation == 3 || packet.operation == 4
                   || packet.operation == 8 || packet.operation == 9)) && (!is_transmitter_backup_empty(src)))
		{
			
			int delay_time = (delay_counter_backup[src] - delay_counter[src]);

			if(setup_time > 0)
			{
				if(last_packet_des_backup[src] == des)
				{  
					delay_time = delay_time - laser_setup_time;
					laser_direction_switch_counter --;
				}
			}
			else
			{
				if(last_packet_des_backup[src] != des)
				{
					delay_time = delay_time + laser_setup_time;
					laser_direction_switch_counter ++;
				}
			}
			if(packet.retry != 1)
			{
				network_queue_delay += delay_time;
				if(p_type == DATA)
					data_network_queue_delay += delay_time;
				else
					meta_network_queue_delay += delay_time;
			}	

			schedule_time += delay_time;
			packet.transfer_time = schedule_time;
			add_message(current_time + backup_transfer_waittime[src]+1, MSG_BACKUP, packet);
			*tran_type = 1;	
			delay_counter_backup[src] = schedule_time;
			last_packet_des_backup[src] = des;
			if(packet.operation == 2 ||  packet.operation == 3 || packet.operation == 4
                   	   || packet.operation == 8 || packet.operation == 9)
			{
				last_meta_packet[src] = schedule_time;
				last_meta_packet_des[src] = des;
			}
		}
		else if(!is_transmitter_backup_empty(src))
		{
			if(p_type == DATA)
				data_packets_forward ++;
			else 
				meta_packets_forward ++;
		}
	}
#endif
	if(p_type == META)
	{
		if(packet.operation == 2 || packet.operation == 3 || packet.operation == 4
		   || packet.operation == 8 || packet.operation == 9)
		{
			counter_t schedule_time_fake;
			if(packet.des != last_meta_packet_des_fake[src])
			{
				if(schedule_time - last_meta_packet_fake[src] < 5)
				{
					meta_reduced_packets ++;
					meta_close_packets ++;
					if(RTCC_flag[src] == 0)
						meta_close_packets ++;
					RTCC_flag[src] = 1;
#ifdef PSEUDO_SCHEDULE_DATA
					int delay = 5 - (schedule_time - last_meta_packet_fake[src]) + 1;
					*meta_delay = last_delay_for_data[src] + delay;
					last_delay_for_data[src] += delay;
#endif
				}
				else
				{
					RTCC_flag[src] = 0;
					last_delay_for_data[src] = 0;
				}
			}
			else
			{
				RTCC_flag[src] = 0;
				last_delay_for_data[src] = 0;
			}
			last_meta_packet_fake[src] = schedule_time;
			last_meta_packet_des_fake[src] = des;
		}
	}
	return schedule_time;
}

int is_transmitter_backup_empty(int src)
{
	int flag = 0;
	if(transmitter_backup[src].tail == NULL)
		flag = 1; //empty
	return flag;
}
void receive_msg(Packet packet, counter_t sim_cycle)
{  /* first receive the packet from the processor, adding packet into its sending flit list */
        long size = packet.packet_size;
        int p_type = packet.packet_type;
                                                                                                                                                                                                     
        long src = packet.src;
        long des = packet.des;
        long i;
        int busy_flag = 0, tran_type = 0;
	int match, meta_delay = 0;
        counter_t current_time = packet.start_time;
	counter_t packet_transfer_time, schedule_time;
	packet_transfer_time = size/flit_tranfer_time;

	if(current_time < sim_cycle)
		current_time = sim_cycle; 

	//printf("Receiving packet %d at cycle %d, packet retry %d\n", packet.msgno, sim_cycle, packet.retry);
	busy_flag = channel_busy_check(src);

#ifdef CONF_RES_ACK
	//if(packet.retry == 0)
        //	match = is_appending_addr(packet); 
	//else
	//	match = 0;

	/* firt check if this packet has the same addr with the appending packet, if does, not transfer this packet until appending packet is clear*/
	if(1)
#endif
	{
		schedule_time = time_schedule(packet, current_time, busy_flag, &tran_type, &meta_delay);
        	/* then adding receive message (DATA_MSG or META_MSG) to message list */
        	if(p_type == DATA)
        	{ /* if it is a data packet */
			/* all data packets are sloted in 5 cycles */
			data_packet_transfer ++;

        	        while(packet.flitnum != 0)
        	        { /* schedule the data packet tranporting time */
        	                int flitNum = packet.flitnum;
        	                flitNum--;
        	                packet.flitnum = flitNum;
				packet.transfer_time = schedule_time;
				packet.meta_delay = meta_delay;
        	                                                                                                                                                                                     
        	                /* if(flitNum == (packet_transfer_time - 1))
        	                        conflict_check(&packet, (schedule_time - flitNum)); */
	
	                        add_message(schedule_time - flitNum, DATA_MSG, packet);
	                }
	        }
	        else
	        {  /* schedule the meta packet tranporting time */
			if(packet.msgno == 151667 || packet.msgno == 151675)
				printf("here\n");
			meta_packet_transfer ++;
			packet.transfer_time = schedule_time;
			packet.meta_delay = meta_delay;
	                add_message(schedule_time, META_MSG, packet);
	        }

		packet.flitnum = size/flit_tranfer_time;
		input_buffer_add(packet, schedule_time + laser_setup_time + confirmation_time*flit_tranfer_time);
		
		/* recording the infront packet destination for the laser_setup_time*/
		if(tran_type == 0)
		{
			delay_counter[src] = schedule_time;
			last_packet_des[src] = des;	 
			laser_power_on_cycle[src] = delay_counter[src];
		}

		/* Insert flit into transmitter buffer */
		if(tran_type == 0)
		{
			if(size == 1)
			{
				Flit *flit;
				flit = (Flit *)malloc(sizeof(Flit));
				flit->flit_type = TAIL;
				flit->next = NULL;
				flit->Num = packet.msgno;
				/* before adding any flit to flit list at sender, check if there is packet under transporting */
				add_flit(flit, &transmitter[src]);
			}                             
			else
			{                                                                                                                                               
				for(i=0; i<size; i++)
				{
					Flit *flit;
					flit = (Flit *)malloc(sizeof(Flit));
					if(i == 0)
					{ /* packet header */
						flit->flit_type = HEADER;
						flit->next = NULL;
						flit->Num = packet.msgno;
						/* before adding any flit to flit list at sender, check if there is packet under transporting */
						add_flit(flit, &transmitter[src]);
					}
					else if(i == (size-1))
					{ /* packet tail */
						flit->flit_type = TAIL;
						flit->next = NULL;
						flit->Num = packet.msgno;
						add_flit(flit, &transmitter[src]);
					}
					else
					{ /* packet body */
						flit->next = NULL;
						flit->flit_type = BODY;
						flit->Num = packet.msgno;
						add_flit(flit, &transmitter[src]);
					}
				}
			}
		}
		else
		{
			if(size == 1)
                        {
                                Flit *flit;
                                flit = (Flit *)malloc(sizeof(Flit));
                                flit->flit_type = TAIL;
                                flit->next = NULL;
                                flit->Num = packet.msgno;
                                /* before adding any flit to flit list at sender, check if there is packet under transporting */
                                add_flit(flit, &transmitter_backup[src]);
                        }
                        else
                        {             
                                for(i=0; i<size; i++)
                                {
                                        Flit *flit;
                                        flit = (Flit *)malloc(sizeof(Flit));
                                        if(i == 0)
                                        { /* packet header */
                                                flit->flit_type = HEADER;
                                                flit->next = NULL;
                                                flit->Num = packet.msgno;
                                                /* before adding any flit to flit list at sender, check if there is packet under transporting */
                                                add_flit(flit, &transmitter_backup[src]);
                                        }
                                        else if(i == (size-1))
					{ /* packet tail */
                                                flit->flit_type = TAIL;
                                                flit->next = NULL;
                                                flit->Num = packet.msgno;
                                                add_flit(flit, &transmitter_backup[src]);
                                        }
					else
                                        { /* packet body */ 
                                                flit->next = NULL;
                                                flit->flit_type = BODY;
                                                flit->Num = packet.msgno;
                                                add_flit(flit, &transmitter_backup[src]);
                                        }
                                }
                        }
		}
	
	}
#ifdef CONF_RES_ACK
	else
		input_buffer_add(packet, WAIT_TIME);
#endif

}


/* Laser warm up and set up delay */
int setup_delay(int src, int des, counter_t sim_cycle, int p_type)
{
	int delay = 0;
	if(laser_power[src] != LASER_ON)
	{
		delay += laser_warmup_time;
		laser_power[src] = LASER_ON;
		laser_power_switchon_counter ++;
		laser_switch_on_delay += delay;
		if(p_type == DATA && packets_sending[src] != 0)
			panic("NETWORK: packets_sending is not reset when laser is off!");	
		/* data packets sending during laser switch on and off */
			
	}

	packets_sending[src] ++;

	if(last_packet_des[src] == des)
		delay += 0;
	else
	{  
		/* direction switch --- laser setup time*/
		laser_direction_switch_counter ++;
		delay += laser_setup_time;
	}

	return delay;

}                    
                                                                                                                                                                
/* channel busy check */
int channel_busy_check(int src)
{  /* check if there is a data packet under transporting at this channel */
        if(transmitter[src].tail == NULL)
                return 0;
        else
                return 1;
}
                                                                                                                                                                                                     
                                                                                                                                                                                                     
                                                                                                                                                                                                     
/* handing DATA_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_data(Packet pck, counter_t sim_cycle)
{
#ifdef IDEALCASE
        if(pck.flitnum == 0)
                sending_flit(pck, sim_cycle);
#else
	/*check if it is to the same cache line with packet in the wait_queue */
	sending_flit(pck, sim_cycle);
#endif
}
                                                                                                                                                                                                     
/* handing META_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_meta(Packet pck, counter_t sim_cycle)
{
        pck.flitnum--;
        if(pck.flitnum == 0)
                sending_flit(pck, sim_cycle);
}

/* handling RECIVEING FLIT MSG */
void receive_flit_msg(Packet pck, counter_t sim_cycle)
{
        receive_flit(pck, sim_cycle);
}

#ifdef CONF_RES_ACK
/* Handling acknowledge message */
void receive_ack_msg(Packet pck, counter_t sim_cycle)
{
        int i, iteration;
        Flit *flit;
	input_buffer_free(pck, sim_cycle);	
}
#endif


void transfer_queue(Packet pck, counter_t sim_cycle)
{
	Flit *flit, *new_flit;
	int i = 0, src = pck.src;
	if(last_packet_des[src] != pck.des && delay_counter[src] + laser_setup_time > pck.transfer_time)
	{
		int delay = (delay_counter[src] + laser_setup_time) - pck.transfer_time;
		pck.transfer_time = pck.transfer_time + delay;
		add_message(sim_cycle + delay, MSG_BACKUP, pck);
		backup_msg_delay_adjust(delay, src, sim_cycle, pck.msgno);
		steering_delay_adjust(pck.msgno, delay);	
		delay_counter_backup[src] += delay;
		backup_transfer_waittime[src] += delay;
		last_meta_packet[src] += delay;	
		input_buffer_time_stamp_adjust(pck.src, pck.msgno, delay);
		return;
	}
	for(i=0; i<pck.packet_size; i++)
	{
		flit = transmitter_backup[src].head;
		if(flit->Num != pck.msgno)
			panic("NETWORK: backup queue unmatch!");
		new_flit = (Flit *)malloc(sizeof(new_flit));
		new_flit->Num = flit->Num;
		new_flit->next = NULL;
		new_flit->flit_type = flit->flit_type;
		add_flit(new_flit,&transmitter[src]);
		free_flit(flit, &transmitter_backup[src]);
	}
	flit = transmitter_backup[src].head;
	if(flit != NULL)
		transmitter_backup_schedule[src] = next_backup_schedule_check(flit->Num);
	else 
		transmitter_backup_schedule[src] = 0;
	last_packet_des[src] = pck.des;	 
	delay_counter[src] = pck.transfer_time;
	laser_power_on_cycle[src] = delay_counter[src];
}

void backup_msg_delay_adjust(int delay, int src, counter_t sim_cycle, int msgno)
{
	Packet packet;
	Msg *current_msg;
	current_msg = msg_list->head;

	while(current_msg != NULL)
	{
		if (current_msg->msg_type != MSG_BACKUP)
		{ /* check the message is the ready to operate or not */
			current_msg = current_msg->next;
			continue;
		}
		packet = current_msg->packet;
		if(current_msg->when >= sim_cycle && packet.src == src && packet.msgno != msgno)
		{
			current_msg->when = current_msg->when + delay;
			current_msg->packet.transfer_time += delay;
			steering_delay_adjust(packet.msgno, delay);
			input_buffer_time_stamp_adjust(packet.src, packet.msgno, delay);
		}
		current_msg = current_msg->next;
	}
	return 0;
}

void input_buffer_time_stamp_adjust(int src, int msgno, int delay)
{
	int i;
	for (i = 0; i < input_buffer_size; i++)
	{
		if(input_buffer[src].InputEntry[i].isValid && input_buffer[src].InputEntry[i].packet.msgno == msgno)
		{
			input_buffer[src].InputEntry[i].time_stamp += delay;
			break;
		}
	}
	
}

void steering_delay_adjust(int msgno, int delay)
{
	Packet packet;
	Msg *current_msg;
	current_msg = msg_list->head;

	while(current_msg != NULL)
	{
		if ((current_msg->msg_type != META_MSG) && (current_msg->msg_type != DATA_MSG))
		{ /* check the message is the ready to operate or not */
			current_msg = current_msg->next;
			continue;
		}
		packet = current_msg->packet;
		if(packet.msgno == msgno)
		{
			current_msg->when = current_msg->when + delay;
			steering_delay += delay;
			current_msg->packet.transfer_time += delay;
		}
		current_msg = current_msg->next;
	}
	return 0;
}
counter_t next_backup_schedule_check(int msgno)
{
	Packet packet;
	Msg *current_msg;
	current_msg = msg_list->head;

	while(current_msg != NULL)
	{
		if (current_msg->msg_type != MSG_BACKUP)
		{ /* check the message is the ready to operate or not */
			current_msg = current_msg->next;
			continue;
		}
		packet = current_msg->packet;
		if(packet.msgno == msgno)
			return (packet.transfer_time - packet.packet_size/flit_tranfer_time + 1);

		current_msg = current_msg->next;
	}
	return 0;
}
/* *****************************************************************************/
