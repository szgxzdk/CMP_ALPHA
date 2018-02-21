CC = gcc
CCPP = g++

CFLAGS = -O0 -g  -w -Wall -Wno-unused 


.SUFFIXES: .o .c

HEADERS = header.h host.h machine.h eval.h stats.h
TARGET = sim-tg

SIM_SRCS =  transaction.c event_queue.c generator.c sim_file.c main.c misc.c stats.c eval.c options.c

SIM_OBJS = $(SIM_SRCS:.c=.o)


$(TARGET):$(SIM_OBJS) 
	$(CC) -o sim-tg $(SIM_OBJS) opticalForSimplescalar/*.o

#	$(CC) -o sim-tg $(SIM_OBJS) opticalForSimplescalar/*.o
#	$(CCPP) -o sim-tg $(SIM_OBJS) popnetForSimplescalar/*.o  popnetForSimplescalar/orion_power_beta_mar2003/power/*.o

transaction.o: transaction.c $(HEADERS)
event_queue.o: event_queue.c $(HEADERS)
generator.o: generator.c $(HEADERS)
sim_file.o: sim_file.c $(HEADERS)
main.o: main.c $(HEADERS)
misc.o: misc.c $(HEADERS)
stats.o: stats.c $(HEADERS)
options.o: options.c $(HEADERS)

.c.o:
	$(CC) -c $(CFLAGS) $<


clean:
	rm -rf *.o sim-tg core
