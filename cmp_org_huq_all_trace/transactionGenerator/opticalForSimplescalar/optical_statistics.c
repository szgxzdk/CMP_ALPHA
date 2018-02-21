
#include "point-point.h"
/* *************************** OPTICAL_STATISTICS.C ****************************/
void optical_reg_stats (struct opt_odb_t *sdb)
{   
    int i = 0;
    stat_reg_counter (sdb, "total_req_msg", "total request messages in network", &meta_packets, 0, NULL);
    stat_reg_counter (sdb, "total_data_msg", "total data messages in network", &data_packets, 0, NULL);
    stat_reg_counter (sdb, "total_req_msg_sending", "total request messages sending in network", &meta_packet_transfer, 0, NULL);
    stat_reg_counter (sdb, "total_data_msg_sending", "total data messages sending in network", &data_packet_transfer, 0, NULL);
    stat_reg_counter (sdb, "total_network_occupancy", "total cycles spend in network", &channel_occupancy, 0, NULL);

    /* power statistics  */
    stat_reg_counter (sdb, "laser_power_switch_off", "total numbers of laser power is switch off", &laser_power_switchoff_counter, 0, NULL);
    stat_reg_counter (sdb, "laser_power_on_cycle", "total cycles of laser power on", &total_cycle_laser_power_on, 0, NULL);
    stat_reg_counter (sdb, "laser_power_switch_on", "total numbers of laser power is switch on", &laser_power_switchon_counter, 0, NULL);
    stat_reg_counter (sdb, "laser_direction_switch", "total times of laser steering is required", &laser_direction_switch_counter, 0, NULL);
    stat_reg_counter (sdb, "flits_transmit", "total numbers of flits transmittion", &flits_transmit_counter, 0, NULL);
    stat_reg_counter (sdb, "flits_receiving", "total numbers of flits are receiving", &flits_receiving_counter, 0, NULL);
	
    /* network delay statistics */
#ifdef CONF_RES_ACK
    stat_reg_counter (sdb, "data_conflict_packet_num", "total numbers of data packet in confliction", &data_conflict_packet_num, 0, NULL);
    stat_reg_counter (sdb, "meta_conflict_packet_num", "total numbers of meta packet in confliction", &meta_conflict_packet_num, 0, NULL);
    stat_reg_counter (sdb, "data_retry_packet_num", "total times of data packet retry", &data_retry_packet_num, 0, NULL);
    stat_reg_counter (sdb, "meta_retry_packet_num", "total times of meta packet retry", &meta_retry_packet_num, 0, NULL);
    stat_reg_counter (sdb, "pending_packet_num", "total numbers of pending packet", &pending_packet_num, 0, NULL);
    stat_reg_counter (sdb, "confliction_delay", "total delay of confliction", &confliction_delay, 0, NULL);
    stat_reg_counter (sdb, "data_confliction_delay", "total delay of data confliction", &data_confliction_delay, 0, NULL);
    stat_reg_counter (sdb, "meta_confliction_delay", "total delay of meta confliction", &meta_confliction_delay, 0, NULL);
    stat_reg_counter (sdb, "pending_delay", "total delay of pending packet", &pending_delay, 0, NULL);
#endif
    stat_reg_counter (sdb, "laser_switch_on_delay", "total delay of laser warmup", &laser_switch_on_delay, 0, NULL);
    stat_reg_counter (sdb, "network_delay_in_fiber", "total network delay in fiber", &network_delay_in_fiber, 0, NULL);
    stat_reg_counter (sdb, "steering_delay", "total delay of laser steering", &steering_delay, 0, NULL);
    stat_reg_counter (sdb, "net_work_queue_delay", "total delay due to the queue effect", &network_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "data_network_queue_delay", "total data delay due to the queue effect", &data_network_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "meta_network_queue_delay", "total meta delay due to the queue effect", &meta_network_queue_delay, 0, NULL);
    stat_reg_counter (sdb, "timeslot_wait_delay", "total delay due to data channel time slot", &timeslot_wait_time, 0, NULL);
    stat_reg_counter (sdb, "meta_wait_delay", "total delay due to meta channel time slot", &meta_timeslot_wait_time, 0, NULL);
    stat_reg_counter (sdb, "reply_data_packets", "total data packets which are reply", &reply_data_packets, 0, NULL);
    stat_reg_counter (sdb, "original_replydata_packets", "total data packets which are reply", &original_reply_data_packets, 0, NULL);
    stat_reg_counter (sdb, "write_back_packets", "total data packets which are write back", &write_back_packets, 0, NULL);
    stat_reg_counter (sdb, "original_writeback_packets", "total data packets which are write back", &original_write_back_packets, 0, NULL);
    stat_reg_counter (sdb, "data_retry_queue", "total retried data packets queued", &retry_data_queue, 0, NULL);
    stat_reg_counter (sdb, "meta_retry_queue", "total retried meta packets queued", &retry_meta_queue, 0, NULL);
    stat_reg_counter (sdb, "max_conflict_involves", "maximum packets involves in one conflict", &max_conf, 0, NULL);
    stat_reg_counter (sdb, "max_packet_retry", "maximum number packets retry", &max_packet_retry, 0, NULL);
    stat_reg_counter (sdb, "max_packets_inbuffer", "maximum number packets in the input buffer", &max_packets_inbuffer, 0, NULL);
    stat_reg_counter (sdb, "max_flits_inbuffer", "maximum number flits in the input buffer", &max_flits_inbuffer, 0, NULL);
    stat_reg_counter (sdb, "meta_delay_packets", "number of meta packets delay", &meta_delay_packets, 0, NULL);
    stat_reg_counter (sdb, "meta_packets_forward", "number of meta packets forward", &meta_packets_forward, 0, NULL);
    stat_reg_counter (sdb, "data_packets_forward", "number of data packets forward", &data_packets_forward, 0, NULL);
    stat_reg_counter (sdb, "RTCC", "data_conflict due to Request Time Close to each other Counter", &RTCC, 0, NULL);
    stat_reg_counter (sdb, "RTC_C_miss", "data_conflict due to Request Time Close to each other Counter", &RTCC_miss, 0, NULL);
    stat_reg_counter (sdb, "RTC_counter_reduced", "data_conflict due to Request Time Close to each other Counter", &RTCC_reduced, 0, NULL);
    stat_reg_counter (sdb, "meta_reduced_packets", "Request Time Close to each other Counter", &meta_reduced_packets, 0, NULL);
    stat_reg_counter (sdb, "meta_close_packets", "Request Time Close to each other Counter", &meta_close_packets, 0, NULL);
    stat_reg_counter (sdb, "data_packets_delay", "data packets are actually delayed to avoid conflict", &data_packets_delay, 0, NULL);
    stat_reg_counter (sdb, "data_packets_no_delay", "data packets are not delayed to avoid conflict", &data_packets_no_delay, 0, NULL);
    stat_reg_counter (sdb, "data_delay", "data packets delay time", &data_delay, 0, NULL);
    stat_reg_counter (sdb, "data_packet_generate_max", "data packets generation max per 100 cycles", &data_packet_generate_max, 0, NULL);

	
    /* conflicts statistics */
//    for(i=0; i<MAXTHREADS; i++)
//    {
//        char ch2[32];
//        sprintf(ch2, "total_%d.channel_busy", i);
//        stat_reg_counter(sdb, ch2, "total number of channel busy for data", &channel_busy_data[i], /* intital value */0, /* format */NULL);
//    }
//    for(i=0; i<MAXTHREADS; i++)
//    {
//        char ch2[32];
//        sprintf(ch2, "total_%d.channel_busy", i);
//        stat_reg_counter(sdb, ch2, "total number of channel busy for meta", &channel_busy_meta[i], /* intital value */0, /* format */NULL);
//    }
    for(i=0; i<8; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_distribution_%d.bincount", i);
        stat_reg_counter(sdb, ch2, "data_distribution", &data_distribution[i], 0, /* format */NULL);
    }
    for(i=0; i<7; i++)
    {
        char ch2[32];
        sprintf(ch2, "queue_%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bincount for input buffer", &queue_bincount[i], 0, /* format */NULL);
    }
    for(i=0; i<7; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_conf_inv%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bincount for packets involved in a conflict", &data_conf_inv_bincount[i], 0, /* format */NULL);
    }
    for(i=0; i<3; i++)
    {
        char ch2[32];
        sprintf(ch2, "wb_conf_inv%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bincount for wb packets involves in a conflict", &write_back_inv_bincount[i], 0, /* format */NULL);
    }
    for(i=0; i<3; i++)
    {
        char ch2[32];
        sprintf(ch2, "packet_pending%d.bincount", i);
        stat_reg_counter(sdb, ch2, "packet pending number", &pending_num[i], 0, /* format */NULL);
    }
    for(i=0; i<5; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_packet_retry%d.bincount", i);
        stat_reg_counter(sdb, ch2, "data packet retry number", &data_retry_num[i], 0, /* format */NULL);
    }
    for(i=0; i<5; i++)
    {
        char ch2[32];
        sprintf(ch2, "meta_packet_retry%d.bincount", i);
        stat_reg_counter(sdb, ch2, "meta packet retry number", &meta_retry_num[i], 0, /* format */NULL);
    }
    for(i=0; i<5; i++)
    {
        char ch2[32];
        sprintf(ch2, "time_spending%d.bincount", i);
        stat_reg_counter(sdb, ch2, "time spending between last packet and first packet in one laser switch", &time_spending[i], 0, /* format */NULL);
    }
    for(i=0; i<5; i++)
    {
        char ch2[32];
        sprintf(ch2, "packets_spending_%d.bincount", i);
        stat_reg_counter(sdb, ch2, "packets sending bin count during the laser switch on and off", &packets_sending_bin_count[i], 0, /* format */NULL);
    }
    for(i=0; i<4; i++)
    {
        char ch2[32];
        sprintf(ch2, "request_%d.conflict", i);
        stat_reg_counter(sdb, ch2, "request conflict on destination per event", &req_conf[i], /* intital value */0, /* format */NULL);
    }
    for(i=0; i<4; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_%d.conflict", i);
        stat_reg_counter(sdb, ch2, "data conflict on destination per event", &data_conf[i], /* intital value */0, /* format */NULL);
    }
    for(i=0; i<4; i++)
    {
        char ch2[32];
        sprintf(ch2, "req_cycle_%d.conflict", i);
        stat_reg_counter(sdb, ch2, "how many cycles see a req conflicts", &req_cycle_conflicts[i], /* intital value */0, /* format */NULL);
    }
    for(i=0; i<4; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_cycle_%d.conflict", i);
        stat_reg_counter(sdb, ch2, "how many cycles see a data conflicts", &data_cycle_conflicts[i], /* intital value */0, /* format */NULL);
    }
    for(i=0; i<5; i++)
    {
        char ch2[32];
        sprintf(ch2, "req_%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bin counter for request", &meta_occ_bincount[i], /* intital value */0, /* format */NULL);
    }
    for(i=0; i<5; i++)
    {
        char ch2[32];
        sprintf(ch2, "data_%d.bincount", i);
        stat_reg_counter(sdb, ch2, "bin counter for data", &data_occ_bincount[i], /* intital value */0, /* format */NULL);
    }
}

/* *****************************************************************************/
