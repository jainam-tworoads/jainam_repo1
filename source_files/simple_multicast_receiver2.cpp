#include <vector>
#include <iostream>
#include <signal.h>
#include <math.h>
#include "dvccode/CDef/ttime.hpp"
#include "dvccode/Utils/multicast_receiver_socket.hpp"
#include "infracore/Tools/simple_multicast_test_data.hpp"

int no_packets_recv_ = 0;
std::vector<HFSAT::ttime_t> times_vec_;

#pragma pack(push, 1)
struct Record {
        long IndexCode;     // Index Code assigned to the index
        long IndexHigh;     // Day’s highest index value
        long IndexLow;      // Day’s lowest index value
        long IndexOpen;     // Day’s opening index value
        long PrevIndexClose; // Previous day’s index closing value
        long IndexValue;    // Current index value

        // Index Identifier
        char IndexID[7];  // Index Identifier of the index

        // Reserved Fields for Internal Use
        char ReservedField6;
        char ReservedField7;
        char ReservedField8;

        // Additional Reserved Field for Internal Use
        char ReservedField9[2];

        // Index Close Value Indicator
        short IndexCloseValueIndicator;  // Identifier for the value published in "Previous Index Close" field
                                        // 0 = Previous Index Close value, 1 = Today’s Indicative closing value, 2 = Today’s Index Close Value

        // Reserved Field for Internal Use
        short ReservedField10;
}; 

#pragma pack(push, 1)
struct Message {
    // Message Type
    long MessageType;  // 2011 for Index broadcast 1, 2012 for Index broadcast 2

    // Reserved Fields for Internal Use
    long ReservedField1;
    long ReservedField2;
    unsigned short ReservedField3;

    // Time Stamp
    short Hour;         // The hour part of the time when the message was sent
    short Minute;       // The minute part of the time when the message was sent
    short Second;       // The second part of the time when the message was sent
    short Millisecond;  // The millisecond part of the time when the message was sent

    // More Reserved Fields for Internal Use
    short ReservedField4;
    short ReservedField5;

    // Number of Records
    short NumberOfRecords;  // Number of records, maximum = 24

    // Sub-structure (repeating for the number of records)
    struct Record Records[24];  // Maximum number of records

};


void printMessage(Message message) {
    std::cout << "MessageType: " << message.MessageType << std::endl;
    
    std::cout << "ReservedField1: " << message.ReservedField1 << std::endl;
    std::cout << "ReservedField2: " << message.ReservedField2 << std::endl;
    std::cout << "ReservedField3: " << message.ReservedField3 << std::endl;
    
    std::cout << "Hour: " << message.Hour << std::endl;
    std::cout << "Minute: " << message.Minute << std::endl;
    std::cout << "Second: " << message.Second << std::endl;
    std::cout << "Millisecond: " << message.Millisecond << std::endl;
    
    std::cout << "ReservedField4: " << message.ReservedField4 << std::endl;
    std::cout << "ReservedField5: " << message.ReservedField5 << std::endl;
    
    std::cout << "NumberOfRecords: " << message.NumberOfRecords << std::endl;

    // Print each record in the Records array
    int number_of_records = message.NumberOfRecords;
    number_of_records = std::min(24,number_of_records);
    for (int i = 0; i < number_of_records; ++i) {
        std::cout << "Record " << i + 1 << ":\n";
        std::cout << "  IndexCode: " << message.Records[i].IndexCode << std::endl;
        std::cout << "  IndexHigh: " << message.Records[i].IndexHigh << std::endl;
        std::cout << "  IndexLow: " << message.Records[i].IndexLow << std::endl;
        std::cout << "  IndexOpen: " << message.Records[i].IndexOpen << std::endl;
        std::cout << "  PrevIndexClose: " << message.Records[i].PrevIndexClose << std::endl;
        
        std::cout << "  IndexValue: " << message.Records[i].IndexValue << std::endl;
        
        std::cout << "  ReservedField6: " << message.Records[i].ReservedField6 << std::endl;
        std::cout << "  ReservedField7: " << message.Records[i].ReservedField7 << std::endl;
        std::cout << "  ReservedField8: " << message.Records[i].ReservedField8 << std::endl;
        
        std::cout << "  IndexCloseValueIndicator: " << message.Records[i].IndexCloseValueIndicator << std::endl;
    }
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

// Function to swap endianness for the entire Message structure
Message swapEndian(Message message) {
    // Swap endianness for each field that needs it
    message.MessageType = swapEndian32(message.MessageType);

    std::cout << "Decoded MessageType: " << message.MessageType << std::endl;

    if(message.MessageType == 2011  || message.MessageType == 2012){

    std::cout << "Converting to endinan\n";
    
    message.ReservedField1 = swapEndian32(message.ReservedField1);
    message.ReservedField2 = swapEndian32(message.ReservedField2);
    message.ReservedField3 = swapEndian16(message.ReservedField3);
    

    // std::cout << "HOUR\n";
    message.Hour = swapEndian16(message.Hour);
    //std::cout << "MIN\n";
    message.Minute = swapEndian16(message.Minute);
    //std::cout << "SECOND\n";
    message.Second = swapEndian16(message.Second);
    //std::cout << "MILLI SEC\n";
    message.Millisecond = swapEndian16(message.Millisecond);
    
    message.ReservedField4 = swapEndian16(message.ReservedField4);
    message.ReservedField5 = swapEndian16(message.ReservedField5);
    
    //std::cout << "Record\n";
    //message.NumberOfRecords = swapEndian16(message.NumberOfRecords);
    
    int number_of_records = message.NumberOfRecords;
    number_of_records = std::min(24,number_of_records);
    // Swap endianness for each record in the Records array
    for (int i = 0; i < number_of_records; ++i) {
        message.Records[i].IndexCode = swapEndian32(message.Records[i].IndexCode);
        message.Records[i].IndexHigh = swapEndian32(message.Records[i].IndexHigh);
        message.Records[i].IndexLow = swapEndian32(message.Records[i].IndexLow);
        message.Records[i].IndexOpen = swapEndian32(message.Records[i].IndexOpen);
        message.Records[i].PrevIndexClose = swapEndian32(message.Records[i].PrevIndexClose);
        message.Records[i].IndexValue = swapEndian32(message.Records[i].IndexValue);

        // Swap endianness for each 16-bit field in the record
        
        message.Records[i].ReservedField6 = swapEndian16(message.Records[i].ReservedField6);
        message.Records[i].ReservedField7 = swapEndian16(message.Records[i].ReservedField7);
        message.Records[i].ReservedField8 = swapEndian16(message.Records[i].ReservedField8);
        
        message.Records[i].IndexCloseValueIndicator = swapEndian16(message.Records[i].IndexCloseValueIndicator);
    }

    //std::cout << "Printing Spot data\n";
    //printMessage(message);
    }
    else{
        std::cout << "Ignoring message " << message.MessageType << std::endl;
    }
    return message;
    
    //std::cout << "Exiting now\n";
//    exit(1);
}

Message store_data(char* u_buf){
    int32_t msg_type = 0;
    uint32_t reserved_field1 = 0;
    uint32_t reserved_field2 = 0;
    uint16_t reserved_field3 = 0;
    uint16_t hour = 0;
    uint16_t minute = 0;
    uint16_t second = 0;
    uint16_t msec = 0;
    uint16_t reserved_field4 = 0;
    uint16_t reserved_field5 = 0;
    uint16_t total_records = 0;
    
    memcpy((void*)&msg_type, (void*)u_buf, sizeof(int32_t));
    memcpy((void*)&reserved_field1, (void*)u_buf+4, sizeof(int32_t));
    memcpy((void*)&reserved_field2, (void*)u_buf+8, sizeof(int32_t));
    memcpy((void*)&reserved_field3, (void*)u_buf+12, sizeof(int16_t));
    memcpy((void*)&hour, (void*)u_buf+14, sizeof(int16_t));
    memcpy((void*)&minute, (void*)u_buf+16, sizeof(int16_t));
    memcpy((void*)&second, (void*)u_buf+18, sizeof(int16_t));
    memcpy((void*)&msec, (void*)u_buf+20, sizeof(int16_t));
    memcpy((void*)&reserved_field4, (void*)u_buf+22, sizeof(int16_t));
    memcpy((void*)&reserved_field5, (void*)u_buf+24, sizeof(int16_t));
    memcpy((void*)&total_records, (void*)u_buf+26, sizeof(int16_t));

    total_records = swapEndian16(total_records);

    Message message;
    message.MessageType = msg_type;
    message.ReservedField1 = reserved_field1;
    message.ReservedField2 = reserved_field2;
    message.ReservedField3 = reserved_field3;
    message.Hour = hour;
    message.Minute = minute;
    message.Second = second;
    message.Millisecond = msec;
    message.ReservedField4 = reserved_field4;
    message.ReservedField5 = reserved_field5;
    message.NumberOfRecords = total_records;

    static struct Record empty_record;
    for(int i=0;i<24;i++){
        message.Records[i]=empty_record;
    }

    int records_no = total_records;
    records_no = std::min(records_no,24);
    for(int i=0;i<records_no;i++){
        uint32_t index_code = 0;
        uint32_t index_high = 0;
        uint32_t index_low = 0;
        uint32_t index_open = 0;
        uint32_t prev_index_close = 0;
        uint32_t index_val = 0;
        char index_id[7];
        memset((void*)index_id,0,sizeof(index_id));
        char reserved_field6 = 'a';
        char reserved_field7 = 'a';
        char reserved_field8 = 'a';
        char reserved_field9[2];
        memset((void*)reserved_field9,0,sizeof(reserved_field9));
        uint16_t index_close_value_indicator = 0;
        uint16_t reserved_field10 = 0;
        memcpy((void*)&index_code, (void*)u_buf+28+40*i, sizeof(int32_t));
        memcpy((void*)&index_high, (void*)u_buf+32+40*i, sizeof(int32_t));
        memcpy((void*)&index_low, (void*)u_buf+36+40*i, sizeof(int32_t));
        memcpy((void*)&index_open, (void*)u_buf+40+40*i, sizeof(int32_t));
        memcpy((void*)&prev_index_close, (void*)u_buf+44+40*i, sizeof(int32_t));
        memcpy((void*)&index_val, (void*)u_buf+48+40*i, sizeof(int32_t));
        memcpy((void*)&index_id, (void*)u_buf+52+40*i, sizeof(index_id));
        memcpy((void*)&reserved_field6, (void*)u_buf+59+40*i, sizeof(char));
        memcpy((void*)&reserved_field7, (void*)u_buf+60+40*i, sizeof(char));
        memcpy((void*)&reserved_field8, (void*)u_buf+61+40*i, sizeof(char));
        memcpy((void*)&reserved_field9, (void*)u_buf+62+40*i, sizeof(reserved_field9));
        memcpy((void*)&index_close_value_indicator, (void*)u_buf+64+40*i, sizeof(int16_t));
        memcpy((void*)&reserved_field10, (void*)u_buf+66+40*i, sizeof(int16_t));

        message.Records[i].IndexCode = index_code;
        message.Records[i].IndexHigh = index_high;
        message.Records[i].IndexLow = index_low;
        message.Records[i].IndexOpen = index_open;
        message.Records[i].PrevIndexClose = prev_index_close;
        message.Records[i].IndexValue = index_val;
        std::copy(std::begin(index_id),std::end(index_id),std::begin(message.Records[i].IndexID));
        message.Records[i].ReservedField6 = reserved_field6;
        message.Records[i].ReservedField7 = reserved_field7;
        message.Records[i].ReservedField8 = reserved_field8;
        std::copy(std::begin(reserved_field9),std::end(reserved_field9),std::begin(message.Records[i].ReservedField9));
        message.Records[i].IndexCloseValueIndicator = index_close_value_indicator;
        message.Records[i].ReservedField10 = reserved_field10;
    }

    return message;
}


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

//  exit(0);
}

int main(int argc, char** argv) {
  std::cout << "argc: " << argc << std::endl;
  if (argc < 4) {
    std::cerr << " Usage: " << argv[0] << "  <interface>  <mcast-ip>  <mport>" << std::endl;
    exit(0);
  }

  //signal(SIGINT, termination_handler);

  std::string interface_ = argv[1];
  std::string mcast_ip_ = argv[2];
  int mcast_port_ = atoi(argv[3]);

  HFSAT::MulticastReceiverSocket abc_(mcast_ip_, mcast_port_, interface_);
  char* u_buf = new char[2000];

  while (1) {
     Message message;
     //int read_length = abc_.ReadN(2000, reinterpret_cast<char*>(&message));
     int read_length = abc_.ReadN(2000, u_buf);
     //Message* msg_ptr=reinterpret_cast<Message*>(u_buf);
     std::cout << " Length : " << read_length << std::endl;
     
     int32_t msg_type = 0;
     memcpy((void*)&msg_type, (void*)u_buf, sizeof(int32_t));
     int32_t swapped_msg_type = swapEndian32(msg_type);

     std::cout << " Msg Type : " <<msg_type << " Swapped Msg Type: " << swapped_msg_type << std::endl;

     if((swapped_msg_type!=2011) && (swapped_msg_type!=2012))
     continue;

     message = store_data(u_buf);
     message = swapEndian(message);
     printMessage(message);    

/*
    TestOrderDatagram* test_order_dgram = reinterpret_cast<TestOrderDatagram*>(u_buf);
    HFSAT::ttime_t difftime = HFSAT::GetTimeOfDay() - test_order_dgram->time_;
    std::cout << " Received seq: " << test_order_dgram->seq_ << " after : " << difftime << " on " << interface_ << " "
              << mcast_ip_ << ":" << mcast_port_ << std::endl;

    ++no_packets_recv_;

    times_vec_.push_back(difftime);
*/

  }
}
