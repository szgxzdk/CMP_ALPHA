#include <stdio.h>
#include <time.h>
#include "header.h"


static int exec_index = -1;
struct opt_odb_t *sim_odb = NULL;
struct stat_sdb_t *sim_sdb = NULL;

static int
orphan_fn (int i, int argc, char **argv)
{
    exec_index = i;
    return /* done */ FALSE;
}
static int running = FALSE;

/* print all simulator stats */
void
sim_print_stats (FILE * fd)	/* output stream */
{
    if (!running)
	return;

    /* print simulation stats */
    fprintf (fd, "\nsim: ** simulation statistics **\n");
    stat_print_stats (sim_sdb, fd);
    fprintf (fd, "\n");
    fflush(fd);
}


int main(int argc, char *argv[])
{
	int i, PrintLimit;
	char *sim_profile, *sim_simout, *configuration;
	FILE *fp_out, *fp_profile, *fp_configuration;
	
        sim_sdb = stat_new ();
	sim_odb = opt_new (orphan_fn);

	/* reading options from the command line: o is for output file, p is for profile input file */
    	opt_reg_string (sim_odb, "-redir:sim", "redirect simulator output to file (non-interactive only)", &sim_simout,
		    /* default */ NULL, /* !print */ FALSE, NULL);
	
	
#ifdef OPTICAL_INTERCONNECT
	optical_options(sim_odb);	
#endif
	profile_options(sim_odb);
	opt_process_options (sim_odb, argc, argv);
	assign_value();	
	/* redirect I/O? */
	if (sim_simout != NULL)
	{
	    /* send simulator non-interactive output (STDERR) to file SIM_SIMOUT */
	    if (fflush (stderr))
		fatal ("unable to flush stderr ");
	    else
	    {
		if (!freopen (sim_simout, "w", stderr))
		    fatal ("unable to redirect simulator output to file `%s'", sim_simout);
	    }

	}
	sim_reg_stats(sim_sdb);
#ifdef OPTICAL_INTERCONNECT
	optical_reg_stats (sim_sdb);
#endif


	/* initalization */
	event_queue = NULL;
	msgno = 0;
#ifdef OPTICAL_INTERCONNECT
	maindirectsim();
#endif
#ifdef POPNET
	mainPopnet();
#endif
	PrintLimit = 1;
	for(i=0; i<total_num_node; i++)
	{
		dir_fifo_head[i] = 0;
		dir_fifo_tail[i] = 0;
		dir_fifo_num[i] = 0;
		dir_fifo_portuse[i] = 0;
		l1_fifo_head[i] = 0;
		l1_fifo_tail[i] = 0;
		l1_fifo_num[i] = 0;
		l1_fifo_portuse[i] = 0;
		mshr_entry[i] = 0;
	}
	running = TRUE;	
	/* main loop */
	for(i=0; i<total_cycles; i++)
	{	
		/* transaction generator */
		transaction_start(sim_cycle);	
		
		dir_eventq_nextevent();
		dir_fifo_dequeue();

#ifdef OPTICAL_INTERCONNECT
		directRunSim(sim_cycle);
#endif
#ifdef POPNET
		popnetRunSim(sim_cycle);
#endif
			
		sim_cycle ++;
		reset_ports();
		if(sim_cycle >= (PrintLimit * (total_cycles/10)))
		{
			printf("[INFO]: sim_cycle=%lld\tProgress=%d%\n", sim_cycle, PrintLimit*10);
			fprintf(stderr, "[INFO]: sim_cycle=%lld\tProgress=%d\n", sim_cycle, PrintLimit*10);
			fflush(stderr);
			PrintLimit ++;	
		}
	}
	sim_print_stats (stderr);

}
