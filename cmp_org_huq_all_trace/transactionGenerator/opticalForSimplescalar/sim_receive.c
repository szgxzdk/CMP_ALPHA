/* map the receiver for packet */
#include "point-point.h"
int receiver_map(long src, long des, int receiver)
{ /* Decide which receiver on the destination should be used to receiver the packet */
        int receiver_id;
        if(Map_algr == RANDOM)
        { /* random algorism: randomly mapping a receiver at each sending time */
                srand(time(NULL));
                receiver_id = rand()%receiver;
        }
        else if(Map_algr == FIXED)
        { /* Fixed algorism: round robin */
                receiver_id = src % receiver;
        }
        return receiver_id;
}
