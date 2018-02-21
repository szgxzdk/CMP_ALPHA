/******************************************************************/
/* This function is used for optical point to point communication */
/* March 04 2008           by Jing  */
/******************************************************************/


/******************************************************************/
/* Data Stucture */
/******************************************************************/


#include "host.h"
#include "smt.h"
#include "machine.h"
#include "misc.h"
#include "options.h"
//#include "./../cache.h"
//#include "../header.h"
#include <stdlib.h>
#include <time.h>
//#define IDEALCASE
#define CONF_RES_ACK 
//#define PER_SCHEDULE
//#define PSEUDO_SCHEDULE
//#define PSEUDO_SCHEDULE_REORDER
//#define PSEUDO_SCHEDULE_DATA
#define PREORITY_PACKETS

/* network configuration parameters */
#define MESHNUM 		4   		//mesh dimentional e.g. 4 is 4*4 mesh
#define PROCESSOR 		MAXTHREADS	//Processors

/* flit unit */
#define HEADER 			1		//flit unit header
#define TAIL 			2		//flit unit tail
#define BODY 			3		//flit unit body
#define CONF_TYPE		4

/* packet type */
#define META 			0		//Meta packet type
#define DATA 			1		//data packet type

#define FIXED			0
#define RANDOM			1

/* message type */
#define EVG_MSG 		0		//adding packet into sender
#define META_MSG 		1		//receiving meta packet
#define DATA_MSG 		2		//receiving data packet
#define WIRE_MSG		3
#define ACK_MSG			4
#define MSG_BACKUP		5

/* laser on or off */
#define LASER_ON		1
#define LASER_OFF		0

#define BUFFER_SIZE		128  		//Input buffer packet entries
#define WAIT_TIME		200000000
#define PENDING_SIZE		10		//how many address are going to the same cache line
int cache_dl1_set_shift;

FILE *fp_trace;
/* network delay */
int laser_warmup_time;

int laser_setup_time;

int laser_switchoff_timeout;

int flit_tranfer_time;

int laser_cycle;

int transmitter_size;

int flit_size;

extern int data_packet_size;

extern int meta_packet_size;

int confirmation_bit;

int input_buffer_size;

int output_buffer_size;

int packet_queue_size;

int meta_receivers;

int data_receivers;

int transmitters;

int confirmation_trans;

int confirmation_receiver;

int time_slot_data;

int data_chan_timeslot;

int exp_backoff;

int first_slot_num;

int retry_algr;

int TBEB_algr;

int confirmation_time;
/* *********************************data structure ***********************/
/* flit unit */
typedef struct Flit{
int flit_type;  
struct Flit *next;
int Num;
}Flit;

/* flit list */
typedef struct Flit_list{
Flit *head;
Flit *tail;
int flits_in_list;
}Flit_list;

/* packet unit */
typedef struct Packet{
counter_t start_time;
counter_t transfer_time;
counter_t transmitter_time;
md_addr_t addr;
int packet_type;
unsigned long packet_size;
long src;
long des;
int flitnum;
int msgno;
int meta_id;
int data_id;
int retry;
int pending[PENDING_SIZE];
int con_flag[4];
int retry_num;
int operation;
int channel;
int meta_delay;
int data_delay;
int miss_flag;
int prefetch;
}Packet;

/* message struct */
typedef struct Msg{
int msg_type;
counter_t when;
struct Msg *next;
struct Msg *pre;
struct Packet packet;
}Msg;

/* message list */
typedef struct Msg_list{
Msg *head;
Msg *tail;
}Msg_list;

/* two alogism parameters from configuration file */
int Map_algr;
int network_algr;

struct Input_entry_t{
int isValid;
Packet packet;
counter_t time_stamp;
};

struct Input_buffer{
int flits_in_buffer; 
int packets_in_buffer; 
struct Input_entry_t InputEntry[BUFFER_SIZE];
};

/* statistics counters */
counter_t meta_occ_bincount[5];
counter_t data_occ_bincount[5];
counter_t channel_occupancy;
counter_t meta_packets;
counter_t data_packets;
counter_t meta_packet_transfer;
counter_t data_packet_transfer;
counter_t channel_busy_data[PROCESSOR];
counter_t channel_busy_meta[PROCESSOR];
counter_t queue_bincount[7];

counter_t req_conf[PROCESSOR];
counter_t data_conf[PROCESSOR];
counter_t data_conf_involves[PROCESSOR][PROCESSOR];
counter_t write_back_involves[PROCESSOR][PROCESSOR];
counter_t write_back_inv_bincount[3];
counter_t data_conf_inv_bincount[7];
counter_t bin_count_end[PROCESSOR][PROCESSOR];
counter_t bin_count_start[PROCESSOR][PROCESSOR];
counter_t req_cycle_conflicts[PROCESSOR];
counter_t data_cycle_conflicts[PROCESSOR];
counter_t last_cycle[PROCESSOR][4];
Msg *conf_list[PROCESSOR][4];
int max_conf;
int max_packet_retry;
int max_packets_inbuffer;
int max_flits_inbuffer;
counter_t last_data_time[PROCESSOR];
counter_t last_time[PROCESSOR];
counter_t RT_time[PROCESSOR][PROCESSOR]; /* Request transfer time */
counter_t RTCC; /* Request time clost to each other counter */
counter_t RTCC_miss; /* Request time clost to each other counter */
counter_t RTCC_reduced; /* Request time clost to each other counter */
counter_t RTCC_flag[PROCESSOR];
counter_t meta_close_packets;
counter_t meta_reduced_packets;
counter_t data_packet_generate;
counter_t data_packet_generate_max;
counter_t data_distribution[8];

/* network delay statistics counters */
counter_t network_queue_delay;
counter_t data_network_queue_delay;
counter_t meta_network_queue_delay;
counter_t pending_delay;
counter_t steering_delay;
counter_t network_delay_in_fiber;
counter_t confliction_delay;
counter_t data_confliction_delay;
counter_t meta_confliction_delay;
counter_t laser_switch_on_delay;
counter_t data_conflict_packet_num;
counter_t meta_conflict_packet_num;
counter_t reply_data_packets;
counter_t original_reply_data_packets;
counter_t write_back_packets;
counter_t original_write_back_packets;
counter_t data_retry_packet_num;
counter_t meta_retry_packet_num;
counter_t pending_packet_num;
counter_t timeslot_wait_time;
counter_t meta_timeslot_wait_time;
counter_t packets_sending[PROCESSOR];
counter_t packets_sending_bin_count[5];
counter_t last_packet_time[PROCESSOR];
counter_t last_meta_packet[PROCESSOR];
counter_t last_meta_packet_fake[PROCESSOR];
counter_t meta_wait_time[PROCESSOR];
counter_t backup_transfer_waittime[PROCESSOR];
counter_t last_delay_for_data[PROCESSOR];
counter_t transmitter_backup_schedule[PROCESSOR];
counter_t first_packet_time[PROCESSOR];
counter_t time_spending[5];
counter_t data_retry_num[5];
counter_t meta_retry_num[5];
counter_t pending_num[3];
counter_t retry_data_queue;
counter_t retry_meta_queue;
counter_t receiver_occ_time[PROCESSOR][PROCESSOR];
counter_t meta_delay_packets;
counter_t meta_packets_forward;
counter_t data_packets_forward;

/* power statistics */
counter_t total_cycle_laser_power_on;
counter_t laser_power_switchon_counter;
counter_t laser_power_switchoff_counter;
counter_t laser_direction_switch_counter;
counter_t flits_transmit_counter;
counter_t flits_receiving_counter;
counter_t data_packets_delay;
counter_t data_packets_no_delay;
counter_t data_delay;

/* central message list */
Msg_list *msg_list;

/* input buffer */
Flit_list  transmitter[PROCESSOR];
Flit_list  transmitter_backup[PROCESSOR];
struct Input_buffer input_buffer[PROCESSOR];
int last_packet_des[PROCESSOR];
int last_meta_packet_des[PROCESSOR];
int last_meta_packet_des_fake[PROCESSOR];
int last_packet_des_backup[PROCESSOR];
counter_t delay_counter[PROCESSOR];
counter_t delay_counter_backup[PROCESSOR];

/* output buffer */
Flit_list  out_buffer[PROCESSOR]; 
Packet packet_queue[PROCESSOR][128];
int packet_queue_tail[PROCESSOR];
int packet_queue_head[PROCESSOR];
int packet_queue_num[PROCESSOR];
int input_buffer_num[PROCESSOR];

/* data and meta receivers */
Flit_list  *data_receiver[PROCESSOR], *meta_receiver[PROCESSOR];

int laser_power[PROCESSOR];
counter_t laser_power_on_cycle[PROCESSOR];
/***********************************function **************************/
/* network configuration inputfile */
void optical_options(struct opt_odb_t *odb);

void optical_reg_stats (struct opt_odb_t *sdb);
int packet_retry_algr(Packet packet);

/* flit list handling functions */
int add_flit(Flit *flit, Flit_list *flit_list);
int free_flit(Flit *flit, Flit_list *flit_list);


/* adding message to central message list */
void add_message(counter_t start_time, int msg_type, Packet packet);
/* free message from central message list */
void free_message(Msg *oldmsg);

void add_packet(Packet packet, long des);
void free_packet(long des);

/* when the sim_main started, call maindirectsim function to initialize the message list and flit list */
void maindirectsim(int map_algr, int net_algr);
void Msg_list_init(Msg_list *msg_list);
void flit_list_init(Flit_list flit_alist);

/* check if there is packet conflits at receiver */
int conflict_check(Packet *packet, counter_t when);
void conflict_type_change(Flit_list *flit_list);
int cycle_conflict_check(Packet packet, counter_t when, int receivers);

void conlict_bincount_collect(counter_t sim_cycle);
/* insert packets into input buffer */
void input_buffer_add(Packet packet, counter_t time_stamp);
/* free packet free input buffer */
void input_buffer_free(Packet packet, counter_t sim_cycle);
int is_appending_addr(Packet packet);
void input_buffer_lookup(long src, counter_t sim_cycle);
void retry_packet(int msgno, counter_t sim_cycle, long src);

/* handling EVG_MSG, adding packet into sending flit list, adding receive message into the message list */
void receive_msg(Packet packet, counter_t sim_cycle);
/* Laser warm up and set up delay */
int setup_delay(int src, int des, counter_t sim_cycle, int p_type);
/* channel busy check */
int channel_busy_check(int src);
/* handing DATA_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_data(Packet pck, counter_t sim_cycle);
/* handing META_MSG, receiving one flit_size (3 flit units) from the sender */
void receive_msg_meta(Packet pck, counter_t sim_cycle);
/* handling RECIVEING FLIT MSG */
void receive_flit_msg(Packet pck, counter_t sim_cycle);
#ifdef CONF_RES_ACK
/* Handling acknowledge message */
void receive_ack_msg(Packet pck, counter_t sim_cycle);
#endif

/* first receive the packet from the processor, add EVG_MSG into message list */
void directMsgInsert(long s1, long s2, long d1, long d2, counter_t sim_cycle, long size, long long int msgNo, md_addr_t addr, int operation, int delay, int L2miss_flag, int prefetch);
/* call this function at every cycle, check it message list to operate message */
void directRunSim(counter_t sim_cycle);
int opticalBufferSpace(long des1, long des2);
/* when it receives its packet successfully, call the finishMsg function to wake up the event */
int finishMsg(long src, long des, counter_t start_time, counter_t popnetmsgno, int packet_type, counter_t transfer_time, int meta_delay);
#ifdef OPTICAL_INTERCONNECT
void free_out_buffer_flit(int iteration, long des);
#endif

/* sending the flits */
void sending_flit(Packet packet, counter_t sim_cycle);
/* receiving the flits */
void receive_flit(Packet packet, counter_t sim_cycle);

/* map the receiver for packet */
int receiver_map(long src, long des, int receiver);
