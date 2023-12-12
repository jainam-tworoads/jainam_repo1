#include <vector>
#include <iostream>
#include <signal.h>
#include <math.h>
#include "dvccode/CDef/ttime.hpp"
#include "dvccode/Utils/multicast_receiver_socket.hpp"
#include "infracore/Tools/simple_multicast_test_data.hpp"
#include "dvccode/CDef/eobi_mds_defines.hpp"
#include<bits/stdc++.h>
#include<chrono>
#include<iomanip>
#include "dvccode/Utils/bulk_file_writer.hpp"

int no_packets_recv_ = 0;
std::vector<HFSAT::ttime_t> times_vec_;
HFSAT::BulkFileWriter bulk_file_writer, bulk_file_writer_sensex, bulk_file_writer_bankex; 

std::pair<uint16_t,std::pair<uint16_t,uint16_t>> get_current_date(){
        std::time_t t = std::time(0);   // get time now
        std::tm* now = std::localtime(&t);
        uint16_t year=now->tm_year+1900;
        uint16_t month=(now->tm_mon+1);
        uint16_t day=now->tm_mday;
        return {year,{month,day}};
}


// Function to swap endianness for a 32-bit integer
uint32_t swapEndian32(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x000000FF) << 24);
}

// Function to swap endianness for a 16-bit integer
uint16_t swapEndian16(uint16_t value) {
    return ((value & 0xFF00) >> 8) |
           ((value & 0x00FF) << 8);
}

std::time_t get_unix_time(uint16_t year,uint16_t month,uint16_t day,uint16_t hour,uint16_t minute,uint16_t second){
    std::tm timeInfo = {0};
    timeInfo.tm_year = year - 1900; // Years since 1900
    timeInfo.tm_mon = month - 1;    // Months are 0-based
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = minute;
    timeInfo.tm_sec = second;

    std::chrono::system_clock::time_point tp=std::chrono::system_clock::from_time_t(std::mktime(&timeInfo));
    std::time_t unixTime = std::chrono::system_clock::to_time_t(tp);
    return unixTime;
}


/*void store_data(char* u_buf, char* output_fname, HFSAT::BulkFileWriter bulk_file_writer){
    //HFSAT::BulkFileWriter bulk_file_writer;
    //bulk_file_writer.Open(output_fname);

    //int32_t msg_type = 0;
    //uint32_t reserved_field1 = 0;
    //uint32_t reserved_field2 = 0;
    //uint16_t reserved_field3 = 0;
    uint16_t hour = 0;
    uint16_t minute = 0;
    uint16_t second = 0;
    uint16_t msec = 0;
    //uint16_t reserved_field4 = 0;
    //uint16_t reserved_field5 = 0;
    uint16_t total_records = 0;
    
    //memcpy((void*)&msg_type, (void*)u_buf, sizeof(int32_t));
    //memcpy((void*)&reserved_field1, (void*)u_buf+4, sizeof(int32_t));
    //memcpy((void*)&reserved_field2, (void*)u_buf+8, sizeof(int32_t));
    //memcpy((void*)&reserved_field3, (void*)u_buf+12, sizeof(int16_t));
    memcpy((void*)&hour, (void*)u_buf+14, sizeof(int16_t));
    memcpy((void*)&minute, (void*)u_buf+16, sizeof(int16_t));
    memcpy((void*)&second, (void*)u_buf+18, sizeof(int16_t));
    memcpy((void*)&msec, (void*)u_buf+20, sizeof(int16_t));
    //memcpy((void*)&reserved_field4, (void*)u_buf+22, sizeof(int16_t));
    //memcpy((void*)&reserved_field5, (void*)u_buf+24, sizeof(int16_t));
    memcpy((void*)&total_records, (void*)u_buf+26, sizeof(int16_t));

    total_records = swapEndian16(total_records);
    
    std::time_t unixTime = get_unix_time(year,month,day,hour,minute,second); 

    int records_no = total_records;
    records_no = std::min(records_no,24);
    for(int i=0;i<records_no;i++){
        //uint32_t index_code = 0;
        //uint32_t index_high = 0;
        //uint32_t index_low = 0;
        //uint32_t index_open = 0;
        //uint32_t prev_index_close = 0;
        uint32_t index_val = 0;
        //char index_id[7];
        //memset((void*)index_id,0,sizeof(index_id));
        //char reserved_field6 = 'a';
        //char reserved_field7 = 'a';
        //char reserved_field8 = 'a';
        //char reserved_field9[2];
        //memset((void*)reserved_field9,0,sizeof(reserved_field9));
        //uint16_t index_close_value_indicator = 0;
        //uint16_t reserved_field10 = 0;
        //memcpy((void*)&index_code, (void*)u_buf+28+40*i, sizeof(int32_t));
        //memcpy((void*)&index_high, (void*)u_buf+32+40*i, sizeof(int32_t));
        //memcpy((void*)&index_low, (void*)u_buf+36+40*i, sizeof(int32_t));
        //memcpy((void*)&index_open, (void*)u_buf+40+40*i, sizeof(int32_t));
        //memcpy((void*)&prev_index_close, (void*)u_buf+44+40*i, sizeof(int32_t));
        memcpy((void*)&index_val, (void*)u_buf+48+40*i, sizeof(int32_t));
        //memcpy((void*)&index_id, (void*)u_buf+52+40*i, sizeof(index_id));
        //memcpy((void*)&reserved_field6, (void*)u_buf+59+40*i, sizeof(char));
        //memcpy((void*)&reserved_field7, (void*)u_buf+60+40*i, sizeof(char));
        //memcpy((void*)&reserved_field8, (void*)u_buf+61+40*i, sizeof(char));
        //memcpy((void*)&reserved_field9, (void*)u_buf+62+40*i, sizeof(reserved_field9));
        //memcpy((void*)&index_close_value_indicator, (void*)u_buf+64+40*i, sizeof(int16_t));
        //memcpy((void*)&reserved_field10, (void*)u_buf+66+40*i, sizeof(int16_t));

        EOBI_MDS::EOBICommonStruct* next_event_ = new EOBI_MDS::EOBICommonStruct();
        next_event_->time_.tv_sec=unixTime;
        next_event_->time_.tv_usec=msec*1000;
        next_event_->order_.action_=7;
        next_event_->order_.price=swapEndian32(index_val);
        
        bulk_file_writer.Write(next_event_, sizeof(EOBI_MDS::EOBICommonStruct));
        bulk_file_writer.CheckToFlushBuffer();               
    }

}*/


void termination_handler(int signal_num_) {
  /*int sum_ = 0;
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

//  exit(0);
*/
        bulk_file_writer.Close();
        bulk_file_writer_sensex.Close();
        bulk_file_writer_bankex.Close();
        std::cout << "Ctrl-C pressed. Exiting..." << std::endl;         
        std::exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
  std::cout << "argc: " << argc << std::endl;
  if (argc < 4) {
    std::cerr << " Usage: " << argv[0] << "  <interface>  <mcast-ip>  <mport>" << std::endl;
    exit(0);
  }

  signal(SIGINT, termination_handler);

  std::string interface_ = argv[1];
  std::string mcast_ip_ = argv[2];
  int mcast_port_ = atoi(argv[3]);

  HFSAT::MulticastReceiverSocket abc_(mcast_ip_, mcast_port_, interface_);
  char* u_buf = new char[2000];

  std::pair<uint16_t,std::pair<uint16_t,uint16_t>> today=get_current_date();
  uint16_t year=today.first;
  uint16_t month=today.second.first;
  uint16_t day=today.second.second;
  std::string month_str=std::to_string(month);
  std::string day_str=std::to_string(day);
  if(month_str.size()==1)
     month_str="0"+month_str;
  if(day_str.size()==1)
     day_str="0"+day_str;
  std::string today_date=std::to_string(year)+month_str+day_str;
  std::string output_fname="/spare/local/MDSlogs/BSE_NFCAST_DATA/"+mcast_ip_+"_"+std::to_string(mcast_port_)+"_"+interface_+"_"+today_date;
  bulk_file_writer.Open(output_fname);
  
  std::string sensex_output_fname="/spare/local/MDSlogs/BSE_NFCAST_DATA/SENSEX_DATA/SENSEX30_"+today_date;
  bulk_file_writer_sensex.Open(sensex_output_fname);

  std::string bankex_output_fname="/spare/local/MDSlogs/BSE_NFCAST_DATA/BANKEX_DATA/BANKEX_"+today_date;
  bulk_file_writer_bankex.Open(bankex_output_fname);

  while (1) {
     int read_length = abc_.ReadN(2000, u_buf);
     std::cout << " Length : " << read_length << std::endl;
     
     int32_t msg_type = 0;
     memcpy((void*)&msg_type, (void*)u_buf, sizeof(int32_t));
     int32_t swapped_msg_type = swapEndian32(msg_type);

     std::cout << " Msg Type : " <<msg_type << " Swapped Msg Type: " << swapped_msg_type << std::endl;

     if((swapped_msg_type!=2011) && (swapped_msg_type!=2012))
     continue;

     uint16_t hour = 0;
     uint16_t minute = 0;
     uint16_t second = 0;
     uint16_t msec = 0;
     uint16_t total_records = 0;

     memcpy((void*)&hour, (void*)u_buf+14, sizeof(int16_t));
     memcpy((void*)&minute, (void*)u_buf+16, sizeof(int16_t));
     memcpy((void*)&second, (void*)u_buf+18, sizeof(int16_t));
     memcpy((void*)&msec, (void*)u_buf+20, sizeof(int16_t));
     memcpy((void*)&total_records, (void*)u_buf+26, sizeof(int16_t));
     
     hour = swapEndian16(hour);
     minute = swapEndian16(minute);
     second = swapEndian16(second);
     msec = swapEndian16(msec);
     total_records = swapEndian16(total_records);

     std::cout<<"Year : "<<year<<" Month : "<<month<<" Day : "<<day<<" Hour : "<<hour<<" MINUTE : "<<minute<<" SECOND : "<<second<<std::endl;
     std::time_t unixTime_ist = get_unix_time(year,month,day,hour,minute,second);
     std::time_t unixTime_gmt = unixTime_ist - (5*3600) - (30*60);

     int records_no = total_records;
     records_no = std::min(records_no,24);
         
     //store_data(u_buf,output_fname,bulk_file_writer);
     for(int i=0;i<records_no;i++){
        std::cout<<"Record number : "<<i<<std::endl;
        uint32_t index_code = 0;
        uint32_t index_val = 0;
        memcpy((void*)&index_val, (void*)u_buf+48+40*i, sizeof(int32_t));
        memcpy((void*)&index_code, (void*)u_buf+28+40*i, sizeof(int32_t));
        //index_val = swapEndian32(index_val);
        std::cout<<"UnixTime : "<<unixTime_ist<<" Microseconds : "<< msec*1000<< " Price : " << swapEndian32(index_val) << " Index_code/symbol : " << swapEndian32(index_code) <<std::endl;
        EOBI_MDS::EOBICommonStruct* next_event_ = new EOBI_MDS::EOBICommonStruct();
        next_event_->source_time.tv_sec=unixTime_gmt;
        next_event_->source_time.tv_usec=msec*1000;
        next_event_->segment_type='E';
        next_event_->token_=swapEndian32(index_code);
        next_event_->order_.action_='8';
        next_event_->order_.size=0;
        next_event_->order_.side=0;
        next_event_->order_.price=swapEndian32(index_val);
        next_event_->order_.price=(next_event_->order_.price)/100;
        
        if(next_event_->token_==1){
                bulk_file_writer_sensex.Write(next_event_, sizeof(EOBI_MDS::EOBICommonStruct));
                bulk_file_writer_sensex.CheckToFlushBuffer();
        }
        else if(next_event_->token_==12){
                bulk_file_writer_bankex.Write(next_event_, sizeof(EOBI_MDS::EOBICommonStruct));
                bulk_file_writer_bankex.CheckToFlushBuffer();
        }
        else{
                bulk_file_writer.Write(next_event_, sizeof(EOBI_MDS::EOBICommonStruct));
                bulk_file_writer.CheckToFlushBuffer();
        }                  
     }       
  }
  bulk_file_writer.Close();
  bulk_file_writer_sensex.Close();
  bulk_file_writer_bankex.Close();
}
