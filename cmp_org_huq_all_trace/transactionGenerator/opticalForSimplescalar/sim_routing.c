#include "point-point.h"
/* ***************************** SIM_ROUTING.C *********************************/
/* sending the flits */
void sending_flit(Packet packet, counter_t sim_cycle)
{
        long src, des;
        int meta_id, data_id;
        src = packet.src;
        des = packet.des;
        int p_type = packet.packet_type;
                                                                                                                                                                                                     
        meta_id = packet.meta_id;
	data_id = packet.data_id;
	Flit_list *receiver;
	//if(p_type == META)
	if(packet.channel == 0)
		receiver = meta_receiver[des];
	else 
		receiver = data_receiver[des];
			
                                                                                                                                                                                                     
        Flit *flit;
                                                                                                                                                                                                     
        int i, iteration = 0;
	int conflict_flag = 0;
                                                                                                                                                                                                     
#ifdef CONF_RES_ACK
	/* Conflict at receiver detection check */
        if((p_type == DATA && packet.flitnum == data_packet_size - 1))
        {
	        if(receiver[data_id].flits_in_list != 0)
                {
		        conflict_flag = 1;
                	conflict_type_change(&receiver[data_id]);
		}
        }
	else if(p_type == DATA && packet.flitnum != data_packet_size - 1)
	{
		if((receiver[data_id].tail)->flit_type == CONF_TYPE)
			conflict_flag = 1;	
	}
        else if(p_type == META)
        {
                if(receiver[meta_id].flits_in_list != 0)
                {       
			conflict_flag = 1;
                	conflict_type_change(&receiver[meta_id]);
		}
	}

        /* sending a flit at a cycle */
        flit = transmitter[src].head;
	flits_transmit_counter ++;
                                                                                                                                                                                             
        if(flit == NULL)
                panic("Network: No meta packet at the sender ");
                                                                                                                                                                                             
        if (flit->Num != packet.msgno)
                panic("Network: packet %d conflits with packet %d\n", packet.msgno, flit->Num);
                                                                                                                                                                                             
        Flit *new_flit;
        new_flit = (Flit *)malloc(sizeof(new_flit));
        new_flit->next = NULL;
        new_flit->Num = flit->Num;
	if(conflict_flag == 1)
		new_flit->flit_type = CONF_TYPE;
	else
        	new_flit->flit_type = flit->flit_type;
                                                                                                                                                                                             
        /* transfer the flits into receiver flit list througn point to point network */
        if(p_type == META)
                add_flit(new_flit, &receiver[meta_id]);
        else
                add_flit(new_flit, &receiver[data_id]);

	free_flit(flit, &transmitter[src]);
        
#endif
                                                                                                                                                                                                     
#ifdef IDEALCASE
        if(p_type == META)
        	iteration = meta_packet_size;
	else
		iteration = data_packet_size;
	
        for (i=0; i<iteration; i++)
        {
                flit = transmitter[src].head;
                                                                                                                                                                                             
                if(flit == NULL)
                        panic("Network: No meta packet at the sender ");
                                                                                                                                                                                             
                if (flit->Num != packet.msgno)
                        panic("Network: packet %d conflits with packet %d\n", packet.msgno, flit->Num);
                                                                                                                                                                                             
                Flit *new_flit;
                new_flit = (Flit *)malloc(sizeof(new_flit));
                new_flit->flit_type = flit->flit_type;
                new_flit->next = NULL;
                new_flit->Num = flit->Num;
                                                                                                                                                                                             
                /* transfer the flits into receiver flit list througn point to point network */
        	if(p_type == META)
	                add_flit(new_flit, &meta_receiver[des][meta_id]);
		else
			add_flit(new_flit, &data_receiver[des][data_id]);

                free_flit(flit, &transmitter[src]);

		flits_transmit_counter ++;
        }

	input_buffer_free(packet, sim_cycle);	
#endif
	/* scheduling network to receive the packet */
        if(packet.flitnum == 0)
                add_message(sim_cycle , WIRE_MSG, packet);
}
                                                                                                                                                                                                     
/* receiving the flits */
void receive_flit(Packet packet, counter_t sim_cycle)
{
        long src, des;
        counter_t start_time;
                                                                                                                                                                                                     
        src = packet.src;
        des = packet.des;
        start_time = packet.start_time;
        int p_type = packet.packet_type;
                                                                                                                                                                                                     
        int meta_id = packet.meta_id;
        int data_id = packet.data_id;
                                                                                                                                                                                                     
        Flit *flit;
        int i;
        int iteration, is_buffer_full = 0;   
	Flit_list *receiver;
	if(packet.channel == 0)
		receiver = meta_receiver[des];
	else 
		receiver = data_receiver[des];

#ifdef CONF_RES_ACK
        if(p_type == DATA)
                iteration = data_packet_size;
        else
                iteration = meta_packet_size;
                                                                                                                                                                                                     
        for(i=0; i<iteration; i++)
        {
                if(p_type == DATA)
                        flit = receiver[data_id].head;
                else
                        flit = receiver[meta_id].head;
                                                                                                                                                                                                     
                if(flit == NULL)
                        panic("Network: No data packet at receiver");
                                                                                                                                                                                                     
                if(i == 0 && (out_buffer[des].flits_in_list > (output_buffer_size - iteration)))
                {
		        is_buffer_full = 1;
                	//fprintf(fp_trace, "outbuffer is full at sim_cycle %d\n", sim_cycle);
		}
                if(!is_buffer_full && flit->flit_type != CONF_TYPE)
                { /* if the output buffer is not full, add flits into the output buffer*/
                                                                                                                                                                                                     
                        Flit *new_flit;
                        new_flit = (Flit *)malloc(sizeof(Flit));
                        new_flit->flit_type = flit->flit_type;
                        new_flit->next = NULL;
                        new_flit->Num = flit->Num;
                        add_flit(new_flit, &out_buffer[des]);
                                                                                                                                                                                                     
                        if((flit->flit_type == TAIL))
                        {
				/* confirmation delay here */
                                add_message(sim_cycle + laser_setup_time + flit_tranfer_time, ACK_MSG, packet);
                                add_packet(packet, des);
                        }
                }
                if(p_type == DATA)
                {
		        free_flit(flit, &receiver[data_id]);
			if(receiver[data_id].flits_in_list == 0)
				flits_receiving_counter += data_packet_size;
                }
		else
                {
		        free_flit(flit, &receiver[meta_id]);
			if(receiver[meta_id].flits_in_list == 0)
				flits_receiving_counter ++;
        	}
	}
#endif
                                                                                                                                                                                                     
#ifdef IDEALCASE
        if(p_type == DATA)
                iteration = data_packet_size;
        else
                iteration = meta_packet_size;
                                                                                                                                                                                                     
        for(i=0; i<iteration; i++)
        {
                if(p_type == DATA)
                        flit = data_receiver[des][data_id].head;
                else
                        flit = meta_receiver[des][meta_id].head;
                                                                                                                                                                                                     
                if(flit == NULL)
                        panic("Network: No data packet at receiver");
                
		if (flit->Num != packet.msgno)
			panic("Network: packet %d conflits with packet %d\n", packet.msgno, flit->Num);
                
		if(i == 0 && (out_buffer[des].flits_in_list > (output_buffer_size - iteration)))
                        is_buffer_full = 1;
                                                                                                                                                                                                     
                if(!is_buffer_full)
                { /* if the output buffer is not full, add flits into the output buffer*/                                                                                                                                                                                                      
                        Flit *new_flit;
                        new_flit = (Flit *)malloc(sizeof(Flit));
                        new_flit->flit_type = flit->flit_type;
                        new_flit->next = NULL;
                        new_flit->Num = flit->Num;
                        add_flit(new_flit, &out_buffer[des]);
                                                                                                                                                                                                     
                        if((flit->flit_type == TAIL))
                                add_packet(packet,des);
                }
		else
			panic("NETWORK: Output buffer has not space!");
                if(p_type == DATA)
                {
		        free_flit(flit, &data_receiver[des][data_id]);
			if(data_receiver[des][data_id].flits_in_list == 0)
				flits_receiving_counter += data_packet_size;
                }
		else
                {
		        free_flit(flit, &meta_receiver[des][meta_id]);
			if(meta_receiver[des][meta_id].flits_in_list == 0)
				flits_receiving_counter ++;
        	}
        }
                                                                                                                                                                                                     
#endif
}
                                                                                                                                                                                                     
/* *****************************************************************************/
