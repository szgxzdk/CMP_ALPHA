#include <fstream>  
#include <stdio.h>


#include "sim_router.h"
#include "sim_foundation.h"
#include "mess_queue.h"
#include "mess_event.h"
#include "SRGen.h"
#include "SStd.h"

//--------Tengfei Wang 20140224-----//

extern char * routerFile;
//FILE* fRouter;
ofstream fRouter;
static int count_wxh = 0;
//***************************************************************************/
void sim_router_template::TXY_algorithm(const add_type & des_t,
		const add_type & sor_t, long s_ph, long s_vc)
{
	long xoffset = des_t[0] - address_[0];
	long yoffset = des_t[1] - address_[1];
	bool xdirection = (abs(static_cast<int>(xoffset)) * 2 
					<= ary_size_)? true: false; 
	bool ydirection = (abs(static_cast<int>(yoffset)) * 2 
					<= ary_size_)? true: false; 

	if(xdirection) {
		if(xoffset < 0) {
			input_module_.add_routing(s_ph, s_vc, VC_type(1, 0));
		}else if(xoffset > 0) {
			input_module_.add_routing(s_ph, s_vc, VC_type(2, 0));
			//input_module_.add_routing(s_ph, s_vc, VC_type(2, 1));
		}else {
			if(ydirection) {
				if(yoffset < 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(3, 0));
				}else if(yoffset > 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(4, 0));
					//input_module_.add_routing(s_ph, s_vc, VC_type(4, 1));
				}
			}else {
				if(yoffset < 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(4, 0));
				}else if(yoffset > 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(3, 0)); 
					//input_module_.add_routing(s_ph, s_vc, VC_type(3, 1)); 
				}
			}
		}
	}else  {
		if(xoffset < 0) {
			input_module_.add_routing(s_ph, s_vc, VC_type(2, 0));
		}else if(xoffset > 0) {
			input_module_.add_routing(s_ph, s_vc, VC_type(1, 0));
			//input_module_.add_routing(s_ph, s_vc, VC_type(1, 1));
		}else {
			if(ydirection) {
				if(yoffset < 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(3, 0));
				}else if(yoffset > 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(4, 0));
					//input_module_.add_routing(s_ph, s_vc, VC_type(4, 1));
				}
			}else {
				if(yoffset < 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(4, 0));
				}else if(yoffset> 0) {
					input_module_.add_routing(s_ph, s_vc, VC_type(3, 0)); 
					//input_module_.add_routing(s_ph, s_vc, VC_type(3, 1)); 
				}
			}
		}
	}
}

//***************************************************************************//
void sim_router_template::XY_algorithm(const add_type & des_t,
		const add_type & sor_t, long s_ph, long s_vc)
{
	long xoffset = des_t[0] - address_[0];
	long yoffset = des_t[1] - address_[1];

	if(yoffset < 0) {
			input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
	}else if(yoffset > 0) {
			input_module_.add_routing(s_ph, s_vc, VC_type(4,s_vc));
	}else {
		if(xoffset < 0) {
				input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
		}else if (xoffset > 0) {
				input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
		}
	}
}

//*********************************************************************//
//---------------adaptive XY roting algorithm--------------------------//
//-------------------Tengfei Wang 20140225-----------------------------//
void sim_router_template::Adaptive_algorithm(const add_type & des_t,
		const add_type & sor_t, long s_ph, long s_vc)
{
	long xoffset = des_t[0] - address_[0];
	long yoffset = des_t[1] - address_[1];
    add_type west  = address_;
    add_type east  = address_;
    add_type north = address_;
    add_type south = address_;
    west[0]  -= 1;
    east[0]  += 1;
    north[1] -= 1;
    south[1] += 1;
//cout<<"xxxxxxxxxxxxxxx"<<endl;
	if(yoffset < 0) {
            if(xoffset>0&&(sim_foundation::wsf().router(east).counter_[1][s_vc] >= sim_foundation::wsf().router(north).counter_[4][s_vc])){
       			input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
			}
			else{
				input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
			}
	}else if(yoffset > 0) {
            if(xoffset<0&&(sim_foundation::wsf().router(west).counter_[2][s_vc] >= sim_foundation::wsf().router(south).counter_[3][s_vc])){
       			input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
			}
			else{
				input_module_.add_routing(s_ph, s_vc, VC_type(4,s_vc));
			}
	}else {
		if(xoffset < 0) {
				input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
		}else if (xoffset > 0) {
				input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
		}
	}

/*
	if(yoffset < 0) {
            if(xoffset>0){
              if(sim_foundation::wsf().router(west).counter_[2][s_vc] >= sim_foundation::wsf().router(north).counter_[4][s_vc] )
              {input_module.add_routing(s_ph, s_vc, VC_type(1,s_vc)); printf("xxxxxxxxxx\n");}
              else  input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
            }
            else
			    input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
	}else if(yoffset > 0) {
            if(xoffset>0){
               if(sim_foundation::wsf().router(east).counter_[1][s_vc] >= sim_foundation::wsf().router(south).counter_[3][s_vc] )
                    
               else

            }
            else
			    input_module_.add_routing(s_ph, s_vc, VC_type(4,s_vc));
	}else {
		if(xoffset < 0) {
				input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
		}else if (xoffset > 0) {
				input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
		}
	}
    */
}
//void sim_router_template::XY_adaptive(const add_type & des_t,
//		const add_type & sor_t, long s_ph, long s_vc)
//{
//	long xoffset = des_t[0] - address_[0];
//	long yoffset = des_t[1] - address_[1];
//    add_type west  = address_;
//    add_type east  = address_;
//    add_type north = address_;
//    add_type south = address_;
//    west[0]  -= 1;
//    east[0]  += 1;
//    north[1] -= 1;
//    south[1] += 1;

//    if(yoffset < 0){
//        if(xoffset < 0){//1
//	  if(sim_foundation::wsf().router(west).counter_[2][s_vc] <= sim_foundation::wsf().router(north).counter_[4][s_vc] )
//	    input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
//	  else
//	    input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
//        }
//        else if(xoffset > 0){//2
//	  if(sim_foundation::wsf().router(east).counter_[1][s_vc] <= sim_foundation::wsf().router(north).counter_[4][s_vc] )
//	    input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
//	  else
//	    input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
//        }
//        else{
//                input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
//        }
//    }
//    else if(yoffset > 0){
//        if(xoffset < 0){//3
//	  if(sim_foundation::wsf().router(west).counter_[2][s_vc] <= sim_foundation::wsf().router(south).counter_[3][s_vc] )
//	    input_module_.add_routing(s_ph, s_vc, VC_type(4,s_vc));
//	  else
//	    input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
//        }
//        else if(xoffset > 0){//4
//	  if(sim_foundation::wsf().router(east).counter_[1][s_vc] <= sim_foundation::wsf().router(south).counter_[3][s_vc] )
//	     input_module_.add_routing(s_ph, s_vc, VC_type(4,s_vc));
//	  else
//	     input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
 //       }
 //       else{
//                input_module_.add_routing(s_ph, s_vc, VC_type(4,s_vc));
//        }
//    }
//    else {
//        if(xoffset < 0){
//                input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
//         }
//         else if(xoffset > 0){
//                 input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
//         }
//    }
//	if(yoffset < 0) {
//			input_module_.add_routing(s_ph, s_vc, VC_type(3,s_vc));
//	}else if(yoffset > 0) {
//			input_module_.add_routing(s_ph, s_vc, VC_type(4,s_vc));
//	}else {
//		if(xoffset < 0) {
//				input_module_.add_routing(s_ph, s_vc, VC_type(1,s_vc));
//		}else if (xoffset > 0) {
//				input_module_.add_routing(s_ph, s_vc, VC_type(2,s_vc));
//		}
//	}
//}
//---------------------------------------------------------------------//

//***************************************************************************//
//only two-dimension is supported
void sim_router_template::routing_decision()
{
	time_type event_time = mess_queue::m_pointer().current_time();
	//for injection physical port 0
	for(long j = 0; j < vc_number_; j++) {
		//for the HEADER_ flit
		flit_template flit_t;
		if(input_module_.state(0,j) == ROUTING_) {
			flit_t = input_module_.get_flit(0,j);
           //----write router trace------------------//
           //-----Tengfei Wang 20140224--------------//     
               //ofstream fRouter;
	     //  if (count_wxh == 0)
	     //  {
		//fRouter=fopen(routerFile, "w");
		//}
		//count_wxh++;
		ofstream fRouter;
			fRouter.open(routerFile,ios::app);
              fRouter<<address_[0]<<"   "<<address_[1]<<"   "<<flit_t.flit_id()<<"   "<<flit_t.flit_size()<<"   "<<event_time<<endl;
               //fprintf(fRouter,"%lld %lld %lld %lld %lld\n", address_[0],address_[1],flit_t.flit_id(),flit_t.flit_size(),event_time);
			   fRouter.close();

			add_type des_t = flit_t.des_addr();
			add_type sor_t = flit_t.sor_addr();
			if(address_ == des_t) {
				if(accept_flit(event_time, flit_t) == 0)
					continue;
				input_module_.remove_flit(0, j);
				input_module_.state_update(0, j, HOME_);
				counter_inc(0, j);
#if 1
				if(flit_t.flit_size() == 1) {
					if(input_module_.input(0, j).size() > 0) {
						input_module_.state_update(0, j, ROUTING_);
					}else {
						input_module_.state_update(0, j, INIT_);
					}
				}
#endif
			}else {
				input_module_.clear_routing(0,j);
				input_module_.clear_crouting(0,j);
				(this->*curr_algorithm)(des_t, sor_t, 0, j);
				input_module_.state_update(0, j, VC_AB_);
			}
		//the BODY_ or TAIL_ flits
		}else if(input_module_.state(0,j) == HOME_)  {
			if(input_module_.input(0, j).size() > 0) {
				flit_t = input_module_.get_flit(0, j);

           //----write router trace------------------//
           //-----Tengfei Wang 20140224--------------//     
				ofstream fRouter;
				fRouter.open(routerFile,ios::app);
              fRouter<<address_[0]<<"   "<<address_[1]<<"   "<<flit_t.flit_id()<<"   "<<flit_t.flit_size()<<"   "<<event_time<<endl;
               //fprintf(fRouter,"%lld %lld %lld %lld %lld\n", address_[0],address_[1],flit_t.flit_id(),flit_t.flit_size(),event_time);
			   fRouter.close();


				Sassert(flit_t.type() != HEADER_);
				if(accept_flit(event_time, flit_t) == 0)
					continue;
				input_module_.remove_flit(0, j);
				counter_inc(0, j);
				if(flit_t.type() == TAIL_) {
					if(input_module_.input(0, j).size() > 0) {
						input_module_.state_update(0, j, ROUTING_);
					}else {
						input_module_.state_update(0, j, INIT_);
					}
				}
			}
		}
	}

	//for other physical ports
	for(long i = 1; i < physic_ports_; i++) {
		for(long j = 0; j < vc_number_; j++) {
			//send back CREDIT message
			flit_template flit_t;

			/*if(input_module_.input(i,j).size() > 0) {
				flit_t = input_module_.get_flit(i,j);
				add_type des_t = flit_t.des_addr();
				if(address_ == des_t) {
					add_type cre_add_t = address_;
					long cre_pc_t = i;
					if((i % 2) == 0) {
						cre_pc_t = i - 1;
						cre_add_t[(i-1)/2] ++;
						if(cre_add_t[(i-1)/2] == ary_size_) {
							cre_add_t[(i-1)/2] = 0;
						}
					}else {
						cre_pc_t = i + 1;
						cre_add_t[(i-1)/2] --;
						if(cre_add_t[(i-1)/2] == -1) {
							cre_add_t[(i-1)/2] = ary_size_ - 1;
						}
					}
					mess_queue::wm_pointer().add_message(
						mess_event(event_time + CREDIT_DELAY_, 
						CREDIT_, address_, cre_add_t, cre_pc_t, j));
				}
			}*/

			//for HEADER_ flit
			if(input_module_.state(i, j) == ROUTING_) {
				flit_t = input_module_.get_flit(i, j);
            
           //----write router trace------------------//
           //-----Tengfei Wang 20140224--------------//     
			ofstream fRouter;
			fRouter.open(routerFile,ios::app);
              fRouter<<address_[0]<<"   "<<address_[1]<<"   "<<flit_t.flit_id()<<"   "<<flit_t.flit_size()<<"   "<<event_time<<endl;
               //fprintf(fRouter,"%lld %lld %lld %lld %lld\n", address_[0],address_[1],flit_t.flit_id(),flit_t.flit_size(),event_time);
			   fRouter.close();

				Sassert(flit_t.type() == HEADER_);
				add_type des_t = flit_t.des_addr();
				add_type sor_t = flit_t.sor_addr();
				if(address_ == des_t) {
					if(accept_flit(event_time, flit_t) == 0)
						continue;
					input_module_.remove_flit(i, j);
					input_module_.state_update(i, j, HOME_);
#if 1
					add_type cre_add_t = address_;
					long cre_pc_t = i;
					if((i % 2) == 0) {
						cre_pc_t = i - 1;
						cre_add_t[(i-1)/2] ++;
						if(cre_add_t[(i-1)/2] == ary_size_) {
							cre_add_t[(i-1)/2] = 0;
						}
					}else {
						cre_pc_t = i + 1;
						cre_add_t[(i-1)/2] --;
						if(cre_add_t[(i-1)/2] == -1) {
							cre_add_t[(i-1)/2] = ary_size_ - 1;
						}
					}
					mess_queue::wm_pointer().add_message(
						mess_event(event_time + CREDIT_DELAY_, 
						CREDIT_, address_, cre_add_t, cre_pc_t, j));

					if(flit_t.flit_size() == 1) {
						if(input_module_.input(i, j).size() > 0) {
							input_module_.state_update(i, j, ROUTING_);
						}else {
							input_module_.state_update(i, j, INIT_);
						}
					}
#endif
				}else {
					input_module_.clear_routing(i, j);
					input_module_.clear_crouting(i, j);
					(this->*curr_algorithm)(des_t, sor_t, i, j);
					input_module_.state_update(i, j, VC_AB_);
				}
			//for BODY_ or TAIL_ flits
			}else if(input_module_.state(i, j) == HOME_) {
				if(input_module_.input(i, j).size() > 0) {
					flit_t = input_module_.get_flit(i, j);

           //----write router trace------------------//
           //-----Tengfei Wang 20140224--------------//     
			ofstream fRouter;
			fRouter.open(routerFile,ios::app);
              fRouter<<address_[0]<<"   "<<address_[1]<<"   "<<flit_t.flit_id()<<"   "<<flit_t.flit_size()<<"   "<<event_time<<endl;
               //fprintf(fRouter,"%lld %lld %lld %lld %lld\n", address_[0],address_[1],flit_t.flit_id(),flit_t.flit_size(),event_time);
			   fRouter.close();


					Sassert(flit_t.type() != HEADER_);
					if(accept_flit(event_time, flit_t) == 0)
						continue;
					input_module_.remove_flit(i, j);
#if 1
					add_type cre_add_t = address_;
					long cre_pc_t = i;
					if((i % 2) == 0) {
						cre_pc_t = i - 1;
						cre_add_t[(i-1)/2] ++;
						if(cre_add_t[(i-1)/2] == ary_size_) {
							cre_add_t[(i-1)/2] = 0;
						}
					}else {
						cre_pc_t = i + 1;
						cre_add_t[(i-1)/2] --;
						if(cre_add_t[(i-1)/2] == -1) {
							cre_add_t[(i-1)/2] = ary_size_ - 1;
						}
					}
					mess_queue::wm_pointer().add_message(
						mess_event(event_time + CREDIT_DELAY_, 
						CREDIT_, address_, cre_add_t, cre_pc_t, j));
#endif
					if(flit_t.type() == TAIL_) {
						if(input_module_.input(i, j).size() > 0) {
							input_module_.state_update(i, j, ROUTING_);
						}else {
							input_module_.state_update(i, j, INIT_);
						}
					}
				}
			}
		}
	}
}

//***************************************************************************//
