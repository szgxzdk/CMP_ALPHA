
/* *********************** CONFINGURATION.C **************************************/

#include "point-point.h"

/* network configuration inputfile */
void optical_options(struct opt_odb_t *odb)
{
    opt_reg_int (odb, "-network:input_buffer_size", "input buffer size (flit unit)", &input_buffer_size, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:output_buffer_size", "output buffer size (flit unit)", &output_buffer_size, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:packet_queue_size", "packet queue size (packet unit, used for output buffer)", &packet_queue_size, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:meta_receivers", "total number of receivers for meta packets", &meta_receivers, /* default */ 1,
                        /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:data_receivers", "total number of receivers for data packets", &data_receivers, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:transmitters", "total numbers of transmitters", &transmitters, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:confirmation_trans", "number of confirmation transmitters", &confirmation_trans, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:confirmation_receiver", "number of confirmation receiver", &confirmation_receiver, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:laser_cycle", "laser cycle (1 CPU cycle)", &laser_cycle, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:transmitter_size", "how many bits can be transfered in one laser cycle", &transmitter_size, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:flit_size", "flit size (bits)", &flit_size, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    //opt_reg_int (odb, "-network:data_packet_size", "Data packet size", &data_packet_size, /* default */ 1,
     //               /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    //opt_reg_int (odb, "-network:meta_packet_size", "meta packet size", &meta_packet_size, /* default */ 1,
     //               /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:confirmation_bit", "confirmation bit", &confirmation_bit, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:laser_warmup_time", "laser warm up time", &laser_warmup_time, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:laser_setup_time", "laser set up time", &laser_setup_time, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:laser_switchoff_timeout", "", &laser_switchoff_timeout, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:data_chan_timeslot", "", &data_chan_timeslot, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
                                                                                                                                                                                                     
    opt_reg_int (odb, "-network:exp_backoff", "", &exp_backoff, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);

    opt_reg_int (odb, "-network:first_slot_num", "", &first_slot_num, /* default */ 5,
                    /* print */ TRUE, /* format */ NULL);

    opt_reg_int (odb, "-network:retry_algr", "", &retry_algr, /* default */ 0,
                    /* print */ TRUE, /* format */ NULL);

    opt_reg_int (odb, "-network:TBEB_algr", "", &TBEB_algr, /* default */ 0,
                    /* print */ TRUE, /* format */ NULL);

    opt_reg_int (odb, "-network:confirmation_time", "", &confirmation_time, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);

    flit_tranfer_time = flit_size/(laser_cycle * transmitter_size);
                                                                                                                                                                                                     
                                                                                                                                                                                                     
    time_slot_data = data_packet_size/flit_size;
}









