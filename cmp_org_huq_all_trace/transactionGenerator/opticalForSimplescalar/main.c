/* when the sim_main started, call maindirectsim function to initialize the message list and flit list */
#include "point-point.h"
void maindirectsim(int map_algr, int net_algr)
{
        int i = 0, n=0, m=0;

	/* fot statistics counter intialization */
        channel_occupancy = 0;
        meta_packets = 0;
        data_packets = 0;
        data_packet_transfer = 0;
        meta_packet_transfer = 0;

	laser_switch_on_delay = 0;
	confliction_delay = 0;
	data_confliction_delay = 0;
	meta_confliction_delay = 0;
	network_delay_in_fiber = 0;
	steering_delay = 0;
	pending_delay = 0;
	network_queue_delay = 0;
	data_network_queue_delay = 0;
	meta_network_queue_delay = 0;
	data_retry_packet_num = 0;
	meta_retry_packet_num = 0;
	pending_packet_num = 0;
	data_conflict_packet_num = 0;
	meta_conflict_packet_num = 0;
	timeslot_wait_time = 0;	
	meta_timeslot_wait_time = 0;	
	write_back_packets = 0;
	reply_data_packets = 0;
	original_reply_data_packets = 0;
	original_write_back_packets = 0;
	retry_data_queue = 0;
	retry_meta_queue = 0;
	max_conf = 0;
	max_packets_inbuffer = 0;
	max_flits_inbuffer = 0;
	max_packet_retry = 0;
	meta_delay_packets = 0;
	meta_packets_forward = 0;
	data_packets_forward = 0;
	RTCC = 0;
	RTCC_miss = 0;
	RTCC_reduced = 0;
	meta_close_packets = 0;
	meta_reduced_packets = 0;
	data_packets_delay = 0;
	data_packets_no_delay = 0;
	data_delay = 0;
	data_packet_generate_max = 0;
	data_packet_generate = 0;
//	srand(time(NULL));	
	for(n=0;n<7;n++)
		queue_bincount[n] = 0;
	for(n=0;n<8;n++)
		data_distribution[n] = 0;
	for(n=0;n<6;n++)
		data_conf_inv_bincount[n] = 0;
	for(n=0;n<3;n++)
	{
		write_back_inv_bincount[n] = 0;
		pending_num[n] = 0;
	}
        for(n=0;n<5;n++)
        {
                meta_occ_bincount[n] = 0;
                data_occ_bincount[n] = 0;
		packets_sending_bin_count[n] = 0;
		data_retry_num[n] = 0;
		meta_retry_num[n] = 0;
		time_spending[n] = 0;
        }
        for(n=0;n<PROCESSOR;n++)
        {      
		delay_counter[n] = 0;
		RTCC_flag[n] = 0;
		delay_counter_backup[n] = 0;
		last_meta_packet[n] = 0;
		last_meta_packet_fake[n] = 0;
		last_packet_time[n] = 0;
		last_packet_des[n] = 0;
		last_packet_des_backup[n] = 0;
		meta_wait_time[n] = 0;
		backup_transfer_waittime[n] = 0;
		last_delay_for_data[n] = 0;
		transmitter_backup_schedule[n] = 0;
		last_meta_packet_des[n] = 0;
		last_meta_packet_des_fake[n] = 0;
		first_packet_time[n] = 0;
		packets_sending[n] = 0;
		input_buffer_num[n] = 0; 
		laser_power[n] = LASER_OFF;
		total_cycle_laser_power_on = 0;
		laser_power_switchoff_counter = 0;
		laser_power_switchon_counter = 0;
		laser_direction_switch_counter = 0;
		flits_transmit_counter = 0;
		flits_receiving_counter = 0;
		last_data_time[n] = 0;
		last_time[n] = 0;

                req_conf[n] = 0;
                data_conf[n] = 0;
                req_cycle_conflicts[n] = 0;
                data_cycle_conflicts[n] = 0;
                channel_busy_meta[n] = 0;
                channel_busy_data[n] = 0;
                                                                                                                                                                                                     
        }

	/* for the input buffer and output buffer*/
	for(n=0;n<PROCESSOR;n++)
	{       
                meta_receiver[n] = (Flit_list *)malloc(256*sizeof(Flit_list));
                data_receiver[n] = (Flit_list *)malloc(256*sizeof(Flit_list));
                                                                                                                                                                                                     
                flit_list_init(transmitter[n]);
                flit_list_init(transmitter_backup[n]);
                                                                                                                                                                                                     
                for(i=0; i<meta_receivers; i++)
                        flit_list_init(meta_receiver[n][i]);
                
		for(i=0; i<data_receivers; i++)
		{
			flit_list_init(data_receiver[n][i]);
			receiver_occ_time[n][i] = 0;
			data_conf_involves[n][i] = 0;
			bin_count_start[n][i] = 0;
			bin_count_end[n][i] = 0;
			write_back_involves[n][i] = 0;
		}
		for(i=0; i<PROCESSOR; i++)
			RT_time[n][i] = 0;
	}																							     
        /* Message list intialization */
        msg_list = (Msg_list *)malloc(sizeof(Msg_list));
        Msg_list_init(msg_list);
                                                                                                                                                                                                     
        /* algorisms read from configuration file */
        Map_algr = map_algr;
        network_algr = net_algr;
}

void Msg_list_init(Msg_list *msg_list)
{
        msg_list->head = NULL;
        msg_list->tail = NULL;
}
void flit_list_init(Flit_list flit_alist)
{
        flit_alist.head = NULL;
        flit_alist.tail = NULL;
        flit_alist.flits_in_list = 0;
}


