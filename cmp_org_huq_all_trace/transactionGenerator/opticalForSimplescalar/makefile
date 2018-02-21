CC = gcc
CCPP = g++

#MFLAGS = `./sysprobe -flags`
#FLAGS = -DDEBUG  -DMTA -DDEBUG1 -w -DREMOVE
CFLAGS = -O0 -g  -w -Wall -Wno-unused



.SUFFIXES: .o .c

HEADERS = point-point.h
TARGET = point-point

SIM_SRCS =  packet_retry_algr.c configuration.c flit.c input_buffer.c main.c mess_queue.c optical_statistics.c packet.c sim_conflict.c sim_main.c sim_mess.c sim_receive.c sim_routing.c 

SIM_OBJS = $(SIM_SRCS:.c=.o)

$(TARGET):$(SIM_OBJS) 
	$(CC) -o point-point $(SIM_OBJS) 

packet_retry_algr.o: packet_retry_algr.c point-point.h
configuration.o: configuration.c point-point.h ../smt.h ../host.h
flit.o: flit.c point-point.h
input_buffer.o:  input_buffer.c point-point.h
main.o: main.c point-point.h
mess_queue.o: mess_queue.c point-point.h
optical_statistics.o: optical_statistics.c point-point.h ../smt.h ../host.h
packet.o: packet.c point-point.h
sim_conflict.o: sim_conflict.c point-point.h 
sim_main.o: sim_main.c point-point.h
sim_mess.o: sim_mess.c point-point.h 
sim_receive.o: sim_receive.c point-point.h 
sim_routing.o: sim_routing.c point-point.h
#options.o: options.c point-point.h options.h
#misc.o: misc.c point-point.h misc.h
#eval.o: eval.c eval.h host.h
#stats.o: stats.c point-point.h stats.h eval.h

.c.o:
	$(CC) -c $(CFLAGS) $<  

clean:
	rm -rf *.o point-point core
