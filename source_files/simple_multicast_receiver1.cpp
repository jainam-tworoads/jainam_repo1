
#include <vector>
#include <iostream>
#include <signal.h>
#include <math.h>
#include <bits/stdc++.h>
#include "dvccode/CDef/ttime.hpp"
#include "dvccode/Utils/multicast_receiver_socket.hpp"
#include "infracore/Tools/simple_multicast_test_data.hpp"
#include <netinet/in.h>

int no_packets_recv_ = 0;
std::vector<HFSAT::ttime_t> times_vec_;

void termination_handler(int signal_num_) {
  int sum_ = 0;
  int max_ = -1, min_ = 65553;
  no_packets_recv_ = 0;

  for (unsigned int i = 0; i < times_vec_.size(); ++i) {
    if (times_vec_[i].tv_sec == 0 && times_vec_[i].tv_usec > 0) {
      sum_ += times_vec_[i].tv_usec;
      ++no_packets_recv_;

      max_ = (max_ > times_vec_[i].tv_usec) ? max_ : times_vec_[i].tv_usec;
      min_ = (min_ < times_vec_[i].tv_usec) ? min_ : times_vec_[i].tv_usec;
    }
  }

  double avg_ = sum_ / (double)no_packets_recv_;

  double std_dev_ = 0.0;
  for (unsigned int i = 0; i < times_vec_.size(); ++i) {
    if (times_vec_[i].tv_sec == 0 && times_vec_[i].tv_usec > 0) {
      std_dev_ += ((avg_ - times_vec_[i].tv_usec) * (avg_ - times_vec_[i].tv_usec));
    }
  }

  std_dev_ /= no_packets_recv_;

  std_dev_ = sqrt(std_dev_);

  // std::cout << "TimeStamp  No_Of_Packets  Min  Max  Avg  StdDev" << std::endl;
  std::cout << HFSAT::GetTimeOfDay() << " " << no_packets_recv_ << " " << (min_ / 1000.0) << " " << (max_ / 1000.0)
            << " " << (avg_ / 1000.0) << " " << (std_dev_ / 1000.0) << std::endl;

  exit(0);
}

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cerr << " Usage: " << argv[0] << "  <interface>  <mcast-ip>  <mport>" << std::endl;
    exit(0);
  }

  signal(SIGINT, termination_handler);

  std::string interface_ = argv[1];
  std::string mcast_ip_ = argv[2];
  int mcast_port_ = atoi(argv[3]);

  HFSAT::MulticastReceiverSocket abc_(mcast_ip_, mcast_port_, interface_);
  char* u_buf = new char[1550];
  while (1) {
    abc_.ReadN(65536, u_buf);
    //std::cout << "Message Type " << htonl(u_buf) << std::endl;

    //TestOrderDatagram* test_order_dgram = reinterpret_cast<TestOrderDatagram*>(u_buf);

    //HFSAT::ttime_t difftime = HFSAT::GetTimeOfDay() - test_order_dgram->time_;

    //std::cout << " Received seq: " << test_order_dgram->seq_ << " Received Data: " << test_order_dgram->reserve_data << " after : " << difftime << " on " << interface_ << " "
             // << mcast_ip_ << ":" << mcast_port_ << std::endl;

    //++no_packets_recv_;

   // times_vec_.push_back(difftime);
   //std::string msg_type="";
   //for(int i=0;i<8;i++){
       // msg_type=msg_type+u_buf[i];
   //}
   //char *msg_type_char=new char[8];
   //for(int i=0;i<8;i++){
       // msg_type_char[i]=u_buf[i];
   //}
   //long* msg_type_long = reinterpret_cast<long*>(msg_type_char);    
   //std::cout<<"Received Data:" << u_buf<<"Msg Type:"<<*msg_type_long<<std::endl;
   //uint64_t msg_seq_no=*(uint64_t*)(u_buf+8);
   //std::cout<<"Hello "<<msg_seq_no<<std::endl;
   uint16_t body_len_ = *(uint16_t*)(u_buf + 0);
   uint16_t template_id_ = *(uint16_t*)(u_buf + 2);
   uint32_t msg_seq_no = *(uint32_t*)(u_buf + 8);
   int32_t mkt_seg_id_ = *(int32_t*)(u_buf + 12);
  // uint32_t msg_type=*(uint32_t*)(u_buf+0);
   //uint32_t idx_code=*(uint32_t*)(u_buf+28);
   std::cout<<"Body len : "<<body_len_<<std::endl;
   std::cout<<"Template id : "<<template_id_<<std::endl;
   std::cout<<"Msg Seq no : "<<msg_seq_no<<std::endl;  
   std::cout<<"Mkt Seg id : "<<mkt_seg_id_<<std::endl;
   //std::cout<<"Msg Type : "<<msg_type<<std::endl;
   //std::cout<<"Index Code : "<<idx_code<<std::endl;
  }
}
