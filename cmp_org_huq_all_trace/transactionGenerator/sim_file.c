#include <math.h>
#include "header.h"
#include "smt.h"
#include "machine.h"

void profile_options(struct opt_odb_t *odb)
{
    opt_reg_int (odb, "-tg:total_num_node", "", &total_num_node, /* default */ 16, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-tg:total_cycles", "", &total_cycles, /* default */ 500000, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-tg:cache_il1_lat", "", &cache_il1_lat, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-tg:cache_dl1_lat", "", &cache_dl1_lat, /* default */ 2, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-tg:cache_dl2_lat", "", &cache_dl2_lat, /* default */ 15, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-tg:mem_lat", "", &mem_lat, /* default */ 200, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-tg:mshr_size", "", &mshr_size, /* default */ 32, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:miss_read_request", "", &read_miss, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:miss_write_request", "", &write_miss, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:write_upgrade", "", &write_upgrade, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:read_direct_reply", "", &read_direct_reply, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:modified_downgrade", "", &modified_downgrade, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:write_direct_reply", "", &write_direct_reply, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_one", "", &invalidation_one, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_two", "", &invalidation_two, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_three", "", &invalidation_three, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_four", "", &invalidation_four, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_five", "", &invalidation_five, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_six", "", &invalidation_six, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_seven", "", &invalidation_seven, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_eight", "", &invalidation_eight, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_nine", "", &invalidation_nine, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_ten", "", &invalidation_ten, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_eleven", "", &invalidation_eleven, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_twelve", "", &invalidation_twelve, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_thirteen", "", &invalidation_thirteen, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_fourteen", "", &invalidation_fourteen, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:invalidation_fifteen", "", &invalidation_fifteen, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-network:data_packet_size", "Data packet size", &data_packet_size, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
    opt_reg_int (odb, "-network:meta_packet_size", "meta packet size", &meta_packet_size, /* default */ 1,
                    /* print */ TRUE, /* format */ NULL);
    opt_reg_float (odb, "-tg:modified_invalidation", "", &modified_invalidation, /* default */ 1, 
		    /* print */ TRUE, /* format */ NULL);

    if(total_num_node > MAXTHREADS)
	panic("total thread number is smaller than node!");

}

void sim_reg_stats(struct opt_odb_t *sdb)
{
    stat_reg_counter (sdb, "total_network_access", "total network access", &totalHopCount, 0, NULL);
    stat_reg_counter (sdb, "total_network_delay", "total network delay", &totalHopDelay, 0, NULL);
    stat_reg_formula (sdb, "average_network_time", "cycles per network access", "total_network_delay / total_network_access", /* format */ NULL);

    stat_reg_counter (sdb, "total_read_misses", "total read requests", &total_read_misses, 0, NULL);
    stat_reg_counter (sdb, "total_write_misses", "total write requests", &total_write_misses, 0, NULL);
    stat_reg_counter (sdb, "total_write_upgrades", "total write upgrades", &total_write_upgrades, 0, NULL);

    stat_reg_counter (sdb, "total_meta_packets", "total meta packets in network", &total_meta_packets, 0, NULL);
    stat_reg_counter (sdb, "total_data_packets", "total data packets in network", &total_data_packets, 0, NULL);
}

void assign_value()
{
    forward_request = 1 - read_direct_reply;
    exclusive_downgrade = 1 - modified_downgrade;
    invalidate_others = 1 - write_direct_reply;
    invalidation_two += invalidation_one;
    invalidation_three += invalidation_two;
    invalidation_four += invalidation_three;
    invalidation_five += invalidation_four;
    invalidation_six += invalidation_five;
    invalidation_seven += invalidation_six;
    invalidation_eight += invalidation_seven;
    invalidation_nine += invalidation_eight;
    invalidation_ten += invalidation_nine;
    invalidation_eleven += invalidation_ten;
    invalidation_twelve += invalidation_eleven;
    invalidation_thirteen += invalidation_twelve;
    invalidation_fourteen += invalidation_thirteen;
    invalidation_fifteen += invalidation_fourteen;
    total_num_node = MAXTHREADS;
    mesh_size = MESH_SIZE;
}



/*
void sim_profile_read(char *file)
{
	FILE *profile;
	profile = fopen(file, "r"); 
	char buf[1024], *p1, *p2;
	char string_name[128];
	float value;
	while (!feof(profile))
	{
		fgets(buf, sizeof(buf), profile);
		sscanf(buf, "%s %f", &string_name, &value);
		printf("string_name %s, value %f\n", string_name, value);	

		assign_value(string_name, value);
	}
	forward_request = 1 - read_direct_reply;
	exclusive_downgrade = 1 - modified_downgrade;
	invalidate_others = 1 - write_direct_reply;
	invalidation_two += invalidation_one;
	invalidation_three += invalidation_two;
	invalidation_four += invalidation_three;
	invalidation_five += invalidation_four;
	invalidation_six += invalidation_five;
	invalidation_seven += invalidation_six;
	invalidation_eight += invalidation_seven;
	invalidation_nine += invalidation_eight;
	invalidation_ten += invalidation_nine;
	invalidation_eleven += invalidation_ten;
	invalidation_twelve += invalidation_eleven;
	invalidation_thirteen += invalidation_twelve;
	invalidation_fourteen += invalidation_thirteen;
	invalidation_fifteen += invalidation_fourteen;
	total_num_node = MAXTHREADS;
	mesh_size = MESH_SIZE;
}

void assign_value(char *string_name, float value)
{
	if(!strcmp(string_name, "-tg:total_num_node"))
		total_num_node = (int)value;	
	
	if(!strcmp(string_name, "-tg:total_cycles"))
		total_cycles = (counter_t)value;

	if(!strcmp(string_name, "-tg:miss_read_request"))
		read_miss = (float)value;
	
	if(!strcmp(string_name, "-tg:miss_write_request"))
		write_miss = (float)value;
	
	if(!strcmp(string_name, "-tg:write_upgrade"))
		write_upgrade = (float)value;

	if(!strcmp(string_name, "-tg:read_direct_reply"))
		read_direct_reply = (float)value;
	
	if(!strcmp(string_name, "-tg:modified_downgrade"))
		modified_downgrade = (float)value;
	
	if(!strcmp(string_name, "-tg:write_direct_reply"))
		write_direct_reply = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_one"))
		invalidation_one = (float)value;

	if(!strcmp(string_name, "-tg:invalidation_two"))
		invalidation_two = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_three"))
		invalidation_three = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_four"))
		invalidation_four = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_five"))
		invalidation_five = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_six"))
		invalidation_six = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_seven"))
		invalidation_seven = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_eight"))
		invalidation_eight = (float)value;	
	
	if(!strcmp(string_name, "-tg:invalidation_nine"))	
		invalidation_nine = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_ten"))
		invalidation_ten = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_eleven"))
		invalidation_eleven = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_twelve"))
		invalidation_twelve = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_thirteen"))
		invalidation_thirteen = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_fourteen"))
		invalidation_fourteen = (float)value;
	
	if(!strcmp(string_name, "-tg:invalidation_fifteen"))
		invalidation_fifteen = (float)value;
	
	if(!strcmp(string_name, "-tg:modified_invalidation"))
		modified_invalid = (float)value;
	
		
}
*/

