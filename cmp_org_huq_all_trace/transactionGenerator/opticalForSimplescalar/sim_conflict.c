/* SIM_CONFLICTION_CHECK.C */

#include "point-point.h"

void conflict_type_change(Flit_list *flit_list)
{	
	if(flit_list->tail == NULL)
		panic("NETWORK: CONFLICTION TYPE CHANGE: No confliction packet here!");

	if((flit_list->tail)->flit_type == CONF_TYPE)
		return;
	else
	{
		Flit *flit;
		flit = flit_list->head;
		while(flit)
		{
			flit->flit_type = CONF_TYPE;
			flit = flit->next;
		}
	}
	return;
}


int cycle_conflict_check(Packet packet, counter_t when, int receivers)
{
        Msg *current_msg, *conflict_msg = NULL;
        current_msg = msg_list->head;
        int des = packet.des;
        int con_result = 0;
        int meta_id = packet.meta_id;
        int data_id = packet.data_id;
        Packet cur_pck;
                                                                                                                                                                                                     
        while(current_msg != NULL)
        {
                Packet current_pck;
                current_pck = current_msg->packet;
                if((current_msg->msg_type == EVG_MSG) || (current_pck.src == packet.src))
                {
                        current_msg = current_msg->next;
                        continue;
                }
                                                                                                                                                                                                     
                if ((current_msg->when == when) && (current_pck.des == packet.des) && (current_pck.packet_type == packet.packet_type))
                {  /* this is for collecting conflictions per each destination */
                                                                                                                                                                                                     
                        if(receiver_map(current_pck.src, current_pck.des, receivers) == receiver_map(packet.src, packet.des, receivers))
                        { /* two receivers */
                                con_result = 1;
                                break;
                        }
                }
                else
                        con_result = 0;
                current_msg = current_msg->next;
        }
        return con_result;
}


/* check if there is packet conflits at receiver */
int conflict_check(Packet *packet, counter_t when)
{
        Msg *current_msg, *conflict_msg = NULL;
        current_msg = msg_list->head;
        int des = packet->des;
        int con_result = 0;
        int meta_id = packet->meta_id;
        int data_id = packet->data_id;
        Packet cur_pck;
        int i = 0;
        for(i=0; i<4; i++)
        {
                current_msg = msg_list->head;
                                                                                                                                                                                                     
                while(current_msg != NULL)
                {
                        Packet current_pck;
                        current_pck = current_msg->packet;
                        if((current_msg->msg_type == EVG_MSG) || (current_pck.src == packet->src))
                        {
                                current_msg = current_msg->next;
                                continue;
                        }
                                                                                                                                                                                                     
                        if ((current_msg->when == when) && (current_pck.des == packet->des) && (current_pck.packet_type == packet->packet_type))
                        {  /* this is for collecting conflictions per each destination */
                                if(receiver_map(current_pck.src, current_pck.des, i+1) == receiver_map(packet->src, packet->des, i+1))
                                { /* two receivers */
					if(current_pck.con_flag[i] == 1)
                                        {
						if (current_pck.packet_type == META)
                                                	req_conf[i] ++;
                                        	else
                                                	data_conf[i] ++;
					}
					else
					{
						if (current_pck.packet_type == META)
                                                	req_conf[i] += 2;
                                        	else
						{
                                                	data_conf[i] += 2;

							Msg *msg = current_msg;
							int num = current_msg->packet.msgno;
							while(msg->packet.flitnum)
							{
								msg = msg->next; 
								if(msg == NULL)
									break;
								if(msg->packet.msgno == num)
									(msg->packet).con_flag[i] = 1;	
							}	
						}
						(current_msg->packet).con_flag[i] = 1;
					}
                                        con_result = 1;
					packet->con_flag[i] = 1;
                                        break;
                                }
                        }
                        else
                                con_result = 0;
                        current_msg = current_msg->next;
                }
        }
        return con_result;
}
