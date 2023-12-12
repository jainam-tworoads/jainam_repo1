// =====================================================================================
//
//       Filename:  read_aflash_raw_data.cpp
//
//    Description:  Reads Rawdump of alphaflash feed and displays its hex string and
//                  the parsed message
//
//        Version:  1.0
//        Created:  03/10/2014 10:38:06 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  (c) Copyright Two Roads Technological Solutions Pvt Ltd 2011
//
//        Address:  Suite No 162, Evoma, #14, Bhattarhalli,
//                  Old Madras Road, Near Garden City College,
//                  KR Puram, Bangalore 560049, India
//          Phone:  +91 80 4190 3551
//
// =====================================================================================

#include <arpa/inet.h>
#include <errno.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

#include "dvccode/Utils/bulk_file_reader.hpp"
#define N 4000000

uint64_t swapEndian64(uint64_t value){
    return ((value & 0x00000000FFFFFFFF) << 32) | ((value & 0xFFFFFFFF00000000) >> 32) |
           ((value & 0x0000FFFF0000FFFF) << 16) | ((value & 0xFFFF0000FFFF0000) >> 16) |
           ((value & 0x00FF00FF00FF00FF) << 8) | ((value & 0xFF00FF00FF00FF00) >> 8); 

}

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

#pragma pack(push, 1)
struct Record {
        long IndexCode;
        long IndexHigh;
        long IndexLow;
        long IndexOpen;
        long PrevIndexClose;
        long IndexValue;
        char IndexID[7];
        char ReservedField6;
        char ReservedField7;
        char ReservedField8;
        char ReservedField9[2];
        short IndexCloseValueIndicator;
        short ReservedField10;
};


#pragma pack(push, 1)
struct Message {
        long MessageType;  // 2011 for Index broadcast 1, 2012 for Index broadcast 2
        long ReservedField1;
        long ReservedField2;
        unsigned short ReservedField3;
        short Hour;
        short Minute;
        short Second;
        short Millisecond;
        short ReservedField4;
        short ReservedField5;
        short NumberOfRecords;
        
        //struct Record {
                //long IndexCode;
                //long IndexHigh;
                //long IndexLow;
                //long IndexOpen;
                //long PrevIndexClose;
                //long IndexValue;
                //char IndexID[7];
                //char ReservedField6;
                //char ReservedField7;
                //char ReservedField8;
                //char ReservedField9[2];
                //short IndexCloseValueIndicator;
                //char ReservedField10;
        //}

        struct Record Records[24];

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
     //int number_of_records = std::min(24,message.NumberOfRecords);
     int number_of_records = message.NumberOfRecords;
     number_of_records=std::min(24,number_of_records);    
     for (int i = 0; i < number_of_records; i++) {
         std::cout << "Record " << i + 1 << ":\n";
         std::cout << "  IndexCode: " << (message.Records[i]).IndexCode << std::endl;
         std::cout << "  IndexHigh: " << (message.Records[i]).IndexHigh << std::endl;
         std::cout << "  IndexLow: " << (message.Records[i]).IndexLow << std::endl;
         std::cout << "  IndexOpen: " << (message.Records[i]).IndexOpen << std::endl;
         std::cout << "  PrevIndexClose: " << (message.Records[i]).PrevIndexClose << std::endl;
     
         std::cout << "  IndexValue: " << message.Records[i].IndexValue << std::endl;
         
         std::cout << "  ReservedField6: " << message.Records[i].ReservedField6 << std::endl;
         std::cout << "  ReservedField7: " << message.Records[i].ReservedField7 << std::endl;
         std::cout << "  ReservedField8: " << message.Records[i].ReservedField8 << std::endl;
         
         std::cout << "  IndexCloseValueIndicator: " << (message.Records[i]).IndexCloseValueIndicator << std::endl;
         std::cout << "  ReservedField10: " << message.Records[i].ReservedField10 << std::endl;
     }
}


Message swapEndian(Message message){
        message.MessageType=swapEndian32(message.MessageType);
        std::cout << "Decoded MessageType: " << message.MessageType << std::endl;
        if(message.MessageType == 2011  || message.MessageType == 2012){
        
        message.ReservedField1 = swapEndian32(message.ReservedField1);
        message.ReservedField2 = swapEndian32(message.ReservedField2);
        message.ReservedField3 = swapEndian16(message.ReservedField3);
        message.Hour = swapEndian16(message.Hour);
        
        message.Minute = swapEndian16(message.Minute);
        
        message.Second = swapEndian16(message.Second);
        
        message.Millisecond = swapEndian16(message.Millisecond);
        
        
        message.ReservedField4 = swapEndian16(message.ReservedField4);
        message.ReservedField5 = swapEndian16(message.ReservedField5);
        
        int number_of_records = message.NumberOfRecords; 
        number_of_records=std::min(24,number_of_records);       
        for (int i = 0; i < number_of_records; i++) {
                //std::cout << "Val of i " << i << std::endl;
                std::cout<<"Unswapped Index Code"<<(message.Records[i]).IndexCode<<std::endl;
                (message.Records[i]).IndexCode = swapEndian32((message.Records[i]).IndexCode);
                (message.Records[i]).IndexHigh = swapEndian32((message.Records[i]).IndexHigh);
                (message.Records[i]).IndexLow = swapEndian32((message.Records[i]).IndexLow);
                (message.Records[i]).IndexOpen = swapEndian32((message.Records[i]).IndexOpen);
                (message.Records[i]).PrevIndexClose = swapEndian32((message.Records[i]).PrevIndexClose);
                (message.Records[i]).IndexValue = swapEndian32((message.Records[i]).IndexValue);
                (message.Records[i]).IndexCloseValueIndicator = swapEndian16((message.Records[i]).IndexCloseValueIndicator);
                (message.Records[i]).ReservedField10 = swapEndian16((message.Records[i]).ReservedField10);
                //std::cout << "Index code " << message.Records[i].IndexCode  << std::endl;
        }
        //std::cout << "Printing Spot data\n";
        //printMessage(message);
        }
        else{
                std::cout << "Ignoring message " << message.MessageType << std::endl;
        }
        return message;
}


                


void printHexString(const char *c, int len) {
  int i;
  unsigned char buff[17];
  unsigned char *pc = (unsigned char *)c;

  if (len == 0) {
    printf("  ZERO LENGTH\n");
    return;
  }
  if (len < 0) {
    printf("  NEGATIVE LENGTH: %i\n", len);
    return;
  }

  for (i = 0; i < len; i++) {

    if ((i % 16) == 0) {
      if (i != 0) printf("  %s\n", buff);

      printf("  %04x ", i);
    }

    printf(" %02x", pc[i]);

    if ((pc[i] < 0x20) || (pc[i] > 0x7e))
      buff[i % 16] = '.';
    else
      buff[i % 16] = pc[i];
    buff[(i % 16) + 1] = '\0';

  }
  while ((i % 16) != 0) {
    printf("   ");
    i++;
  }

  printf("  %s\n", buff);

  printf("\n");
  fflush(stdout);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("USAGE: %s <raw_data_filename> \nSize of TimeVal: %ld\t Int: %ld\n", argv[0], sizeof(timeval),
           sizeof(uint32_t));
    exit(0);
  }

  std::string fname(argv[1]);
  std::cerr << "File name: " << fname << std::endl;
  HFSAT::BulkFileReader bfr(N);
  bfr.open(fname);
  if (!bfr.is_open()) {
    std::cerr << "Cannot open file: " << argv[4] << std::endl;
  }

  char* msg_buf = new char[N];
  int msg_len = 1;
  timeval time_;
  char tmbuf[64];
  char data_buffer[8192];

  while (msg_len > 0) {

    memset((void*)data_buffer,0,sizeof(data_buffer)); 	  
    int lensize = bfr.read(&msg_len, sizeof(int));
    if (lensize < (int)sizeof(int)) break;

    bfr.read(&time_, sizeof(time_));
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", localtime(&time_.tv_sec));

    std::cout << " Length : " << msg_len << " " << time_.tv_sec << "." << time_.tv_usec << std::endl;

    int lensize2 = bfr.read(data_buffer, msg_len);
    std::cout << "Read :"  << lensize2 << std::endl;

    std::cout << " ================================= DATA ======================= " << std::endl;
    //printHexString(data_buffer,lensize2);

    int32_t msg_type = 0;
    memcpy((void*)&msg_type, (void*)&data_buffer, sizeof(int32_t));
    int32_t swapped_msg_type = swapEndian32(msg_type);

    std::cout << " Msg Type : " <<msg_type << " " << swapped_msg_type << std::endl;

    if((swapped_msg_type!=2011) && (swapped_msg_type!=2012))
    continue;

    //Message message;
    //memcpy((void*)&message, (void*)&data_buffer, sizeof(Message));
    //Message* message = reinterpret_cast<Message*>(data_buffer);
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

    memcpy((void*)&reserved_field1, (void*)&data_buffer+4, sizeof(int32_t));
    memcpy((void*)&reserved_field2, (void*)&data_buffer+8, sizeof(int32_t));
    memcpy((void*)&reserved_field3, (void*)&data_buffer+12, sizeof(int16_t));
    memcpy((void*)&hour, (void*)&data_buffer+14, sizeof(int16_t));
    memcpy((void*)&minute, (void*)&data_buffer+16, sizeof(int16_t));
    memcpy((void*)&second, (void*)&data_buffer+18, sizeof(int16_t));
    memcpy((void*)&msec, (void*)&data_buffer+20, sizeof(int16_t));
    memcpy((void*)&reserved_field4, (void*)&data_buffer+22, sizeof(int16_t));
    memcpy((void*)&reserved_field5, (void*)&data_buffer+24, sizeof(int16_t));
    memcpy((void*)&total_records, (void*)&data_buffer+26, sizeof(int16_t));

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
        memcpy((void*)&index_code, (void*)&data_buffer+28+40*i, sizeof(int32_t));
        memcpy((void*)&index_high, (void*)&data_buffer+32+40*i, sizeof(int32_t));
        memcpy((void*)&index_low, (void*)&data_buffer+36+40*i, sizeof(int32_t));
        memcpy((void*)&index_open, (void*)&data_buffer+40+40*i, sizeof(int32_t));
        memcpy((void*)&prev_index_close, (void*)&data_buffer+44+40*i, sizeof(int32_t));
        memcpy((void*)&index_val, (void*)&data_buffer+48+40*i, sizeof(int32_t));
        memcpy((void*)&index_id, (void*)&data_buffer+52+40*i, sizeof(index_id));
        memcpy((void*)&reserved_field6, (void*)&data_buffer+59+40*i, sizeof(char));
        memcpy((void*)&reserved_field7, (void*)&data_buffer+60+40*i, sizeof(char));
        memcpy((void*)&reserved_field8, (void*)&data_buffer+61+40*i, sizeof(char));
        memcpy((void*)&reserved_field9, (void*)&data_buffer+62+40*i, sizeof(reserved_field9));
        memcpy((void*)&index_close_value_indicator, (void*)&data_buffer+64+40*i, sizeof(int16_t));
        memcpy((void*)&reserved_field10, (void*)&data_buffer+66+40*i, sizeof(int16_t));
        
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
    
    //std::cout << "Printing Spot data Big Endian: \n";
    //printMessage(message);
    
    message = swapEndian(message);
    
    std::cout << "Printing Spot data Little Endian: \n";
    printMessage(message);
              
  }
  bfr.close();
  delete msg_buf;
  return 0;
}
