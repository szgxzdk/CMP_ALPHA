
#include "point-point.h"
/* ******************************** SIM_MAIN.C *********************************/
/* first receive the packet from the processor, add EVG_MSG into message list */
void directMsgInsert(long s1, long s2, long d1, long d2, counter_t sim_cycle, long size, long long int msgNo, md_addr_t addr, int operation, int delay, int L2miss_flag, int prefetch)
{ /* First time receiver the data from the processor, insert the message to central message list*/
        long src, des;
        Packet packet;
        int i;   

        src = s1*MESHNUM + s2;
        des = d1*MESHNUM + d2;
                                                                                                                                                                                                     
        packet.start_time = sim_cycle;
                                                                                                                                                                                                     
        packet.src = src;
        packet.des = des;
        packet.msgno = msgNo;
        packet.packet_size = size;
	packet.addr = addr;
	packet.retry = 0;
	packet.retry_num = 0;
	packet.transfer_time = 0;
	packet.transmitter_time = 0;
	packet.operation = operation;
	packet.meta_delay = 0;
	packet.prefetch = prefetch;
	
	//if(!((operation == 14) || (operation == 15) || (operation == 16) || (operation == 17) || (operation == 10) || (operation == 12)) && delay)
	//	panic("NETWORK: other packets can not have delay except the data packets! ");
	packet.data_delay = delay;
	packet.miss_flag = L2miss_flag;

	for(i=0; i<PENDING_SIZE; i++)
		packet.pending[i] = 0; 

	for (i=0; i<4; i++)
		packet.con_flag[i] = 0;

        if (size <= meta_packet_size)
        { /* meta packet */
                packet.packet_type = META;
                /* how many cycles is this packet going to take to finish tranfering */
		packet.meta_id = receiver_map(src, des, meta_receivers);
		packet.data_id = 0;
                packet.flitnum = meta_packet_size/flit_tranfer_time;
                meta_packets ++;
		int random = rand()%10;
		packet.channel = 0;
        }
        else
        { /* data packet */
                packet.packet_type = DATA;
                /* how many cycles is this packet going to take to finish tranfering */
		packet.data_id = receiver_map(src, des, data_receivers);
		packet.meta_id = 0;
                packet.flitnum = data_packet_size/flit_tranfer_time;
                data_packets ++;
		packet.channel = 1;
        }

        add_message(sim_cycle, EVG_MSG, packet);
	input_buffer_num[src] += size;
}
                                                                                                                                                                                                     


/* call this function at every cycle, check it message list to operate message */
void directRunSim(counter_t sim_cycle)
{
        counter_t cur_time;
        Packet packet;
        Msg *current_msg;
        Msg *pre_msg;
        current_msg = msg_list->head;
        int occupancy_flag = 0;
        int occupancy_data_bincount = 0;
        int occupancy_meta_bincount = 0;
        int i = 0;
                                                                                                                                                                                                     
        int req_cycle_conflicts_flag[4] = {0};
        int data_cycle_conflicts_flag[4] = {0};
        int total_cycle_laser_power_on_flag = 0;                                                                                                                                                                                             
        while(current_msg != NULL)
        {
                Msg *msg;
                msg = current_msg;
                cur_time = msg->when;
                                                                                                                                                                                                     
                if (cur_time > sim_cycle)
                { /* check the message is the ready to operate or not */
                        current_msg = current_msg->next;
                        continue;
                }
                packet = msg->packet;
                switch(msg->msg_type)
                {
                        case EVG_MSG:
                        { /* receive the packet from the processor */
				
				//printf("Receiving packet %d at cycle %d\n", packet.msgno, sim_cycle);
                                receive_msg(packet, sim_cycle);
                                break;
                        }
                        case META_MSG:
                        { /* receive the meta packet at the receiver */
                                occupancy_flag = 1;
                                occupancy_meta_bincount ++;
                                for(i=0; i<4; i++)
                                {
                                        if(cycle_conflict_check(packet, sim_cycle, i+1))
                                                req_cycle_conflicts_flag[i] = 1;
                                }
                                receive_msg_meta(packet, sim_cycle);
                                break;
                        }
                        case DATA_MSG:
                        { /* receive the data packet at the receiver */
                                occupancy_flag = 1;
                                occupancy_data_bincount ++;
				if(packet.flitnum == 4 && packet.retry_num == 0)
					data_packet_generate ++;

                                for(i=0; i<4; i++)
                                {
                                        if(cycle_conflict_check(packet, sim_cycle, i+1))
                                                data_cycle_conflicts_flag[i] = 1;
                                }
                                receive_msg_data(packet, sim_cycle);
                                break;
                        }
                        case WIRE_MSG:
                        {
                                receive_flit_msg(packet, sim_cycle);
                                break;
                        }
#ifdef CONF_RES_ACK
                        case ACK_MSG:
                        {
                                receive_ack_msg(packet, sim_cycle);
                                break;
                        }
#endif
			case MSG_BACKUP:
			{
				transfer_queue(packet, sim_cycle);
			}
                }
                current_msg = current_msg->next;
                /* after finish this message, free it from the message list */
                free_message(msg);
        }
#ifdef CONF_RES_ACK
	/* check input buffer and if there are some entry are not receiving confirmation bit then retry the packet */
	for(i=0; i<PROCESSOR; i++)
	{
		input_buffer_lookup(i, sim_cycle);                 
		if(input_buffer[i].packets_in_buffer == 0)
			queue_bincount[0] ++;
		else if(input_buffer[i].packets_in_buffer == 1)
			queue_bincount[1] ++;
		else if(input_buffer[i].packets_in_buffer == 2)
			queue_bincount[2] ++;
		else if(input_buffer[i].packets_in_buffer == 3)
			queue_bincount[3] ++;
		else if(input_buffer[i].packets_in_buffer == 4)
			queue_bincount[4] ++;
		else if(input_buffer[i].packets_in_buffer == 5)
			queue_bincount[5] ++;
		else if(input_buffer[i].packets_in_buffer > 5)
			queue_bincount[6] ++;
	}
	/* conflict bincount collection function */
	conlict_bincount_collect(sim_cycle);
#endif
	
	if(sim_cycle%100 == 0)
	{
		//fprintf(fp_trace, "sim_cycle: %lld, data_packet_generate is %d\n", sim_cycle, data_packet_generate);
		//printf("sim_cycle: %lld, data_packet_generate is %d\n", sim_cycle, data_packet_generate);
		if(data_packet_generate == 0)
			data_distribution[0] ++;
		else if(data_packet_generate == 1)
			data_distribution[1] ++;
		else if(data_packet_generate > 1 && data_packet_generate <= 5)
			data_distribution[2] ++;
		else if(data_packet_generate > 5 && data_packet_generate <= 10)
			data_distribution[3] ++;
		else if(data_packet_generate > 10 && data_packet_generate <= 20)
			data_distribution[4] ++;
		else if(data_packet_generate > 20 && data_packet_generate <= 30)
			data_distribution[5] ++;
		else if(data_packet_generate > 30 && data_packet_generate <= 40)
			data_distribution[6] ++;
		else if(data_packet_generate > 40)
			data_distribution[7] ++;
		if(data_packet_generate > data_packet_generate_max)
			data_packet_generate_max = data_packet_generate;
		data_packet_generate = 0;
	}
#ifdef PSEUDO_SCHEDULE_REORDER
	for(i=0; i<PROCESSOR; i++)
	{
		if(backup_transfer_waittime[i] != 0)
			backup_transfer_waittime[i] --;
	}
#endif
                                                                                                                                                                 
        /* output flits from output buffer each cycle */
        for(i = 0; i<PROCESSOR; i++)
        {
		if(packet_queue_num[i] > 0)
		{
                	packet = packet_queue[i][packet_queue_head[i]];
                	if(packet.msgno != (out_buffer[i].head)->Num)
                	        panic("NETWORK: output flits buffer unmatch with output packet!");
                	if(finishMsg(packet.src, packet.des, packet.start_time, packet.msgno, packet.packet_type, packet.transfer_time, packet.meta_delay))
			{
				if(packet.packet_type == DATA)
				{
					if(packet.retry == 1)
					{
#ifndef PER_SCHEDULE
						data_confliction_delay += (sim_cycle - packet.start_time) - (data_packet_size/flit_tranfer_time);
						confliction_delay += (sim_cycle - packet.start_time) - (data_packet_size/flit_tranfer_time);
#else
						printf("should not be here\n");
#endif
					}
					network_delay_in_fiber += data_packet_size/flit_tranfer_time;
				}
				else
				{
					if(packet.retry == 1)
					{
						meta_confliction_delay += (sim_cycle - packet.start_time) - (meta_packet_size/flit_tranfer_time);
						confliction_delay += (sim_cycle - packet.start_time) - (data_packet_size/flit_tranfer_time);
					}
					network_delay_in_fiber += meta_packet_size/flit_tranfer_time;
				}
			}
        	}
	}
	
	/* Laser switch on/off */
	for(i = 0; i<PROCESSOR; i++)
	{
		if(laser_power[i] == LASER_ON)
		{
			total_cycle_laser_power_on_flag = 1;
			if((sim_cycle - laser_power_on_cycle[i]) > laser_switchoff_timeout)
			{
				laser_power[i] = LASER_OFF;
				laser_power_switchoff_counter ++;
			
				/* data packets sending during laser switch on and off */
				if(packets_sending[i]!= 0 && packets_sending[i]<5)
				{
					packets_sending_bin_count[packets_sending[i]-1] ++;
					time_spending[packets_sending[i]-1] += last_packet_time[i] - first_packet_time[i];	
				}
				else if(packets_sending[i] != 0 && packets_sending[i] >= 5)
				{
					packets_sending_bin_count[4] ++;
					time_spending[4] += last_packet_time[i] - first_packet_time[i];	
				}
				packets_sending[i] = 0;
				last_packet_time[i] = 0;
				first_packet_time[i] = 0;
			}
		}
	}
	if (total_cycle_laser_power_on_flag == 1)
            total_cycle_laser_power_on ++;                                                                                                                                                                                         
        /* statistic collection */
        for(i=0; i<4; i++)
        {
                if(req_cycle_conflicts_flag[i] == 1)
                        req_cycle_conflicts[i] ++;
                                                                                                                                                                                                     
                if(data_cycle_conflicts_flag[i] == 1)
                        data_cycle_conflicts[i] ++;
        }
                                                                                                                                                                                                     
        if(occupancy_flag == 1)
        {
                channel_occupancy ++;
                if(occupancy_data_bincount == 1)
                        data_occ_bincount[0] ++;
                else if(occupancy_data_bincount == 2)
                        data_occ_bincount[1] ++;
                else if(occupancy_data_bincount == 3)
                        data_occ_bincount[2] ++;
                else if(occupancy_data_bincount == 4)
                        data_occ_bincount[3] ++;
                else if(occupancy_data_bincount >= 5)
                        data_occ_bincount[4] ++;
                                                                                                                                                                                                     
                if(occupancy_meta_bincount == 1)
                        meta_occ_bincount[0] ++;
                else if(occupancy_meta_bincount == 2)
                        meta_occ_bincount[1] ++;
                else if(occupancy_meta_bincount == 3)
                        meta_occ_bincount[2] ++;
                else if(occupancy_meta_bincount == 4)
                        meta_occ_bincount[3] ++;
                else if(occupancy_meta_bincount >= 5)
                        meta_occ_bincount[4] ++;
        }

}
                                                                                                                                                                                                     
                                                                                                                                                                                                     
int opticalBufferSpace(long des1, long des2)
{
        if(input_buffer_num[des1*MESHNUM+des2] > (input_buffer_size - PROCESSOR*meta_packet_size))
                return 1;
        else
                return 0;
}
                                                                                                                                                                                                     
/* when it receives its packet successfully, call the finishMsg function to wake up the event */
int finishMsg(long src, long des, counter_t start_time, counter_t popnetmsgno, int packet_type, counter_t transfer_time, int meta_delay)
{
        long src1, src2, des1, des2;
        int iteration;                                                                                                                                                                       
        src1 = src/MESHNUM;
        src2 = src%MESHNUM;
        des1 = des/MESHNUM;
        des2 = des%MESHNUM;

        if(MsgComplete(src1, src2, des1, des2, start_time, popnetmsgno, transfer_time, meta_delay))
        {
                if(packet_type == DATA)
                        iteration = data_packet_size;
                else
                        iteration = meta_packet_size;
                                                                                                                                                                                                     
                free_out_buffer_flit(iteration, des);
                free_packet(des);
		return 1;
        }
	return 0;
}

void free_out_buffer_flit(int iteration, long des)
{
        int i=0;
        Flit *flit;
        for(i = 0; i<iteration; i++)
        {
                flit = out_buffer[des].head;
                free_flit(flit, &out_buffer[des]);
        }
}

/* *****************************************************************************/
