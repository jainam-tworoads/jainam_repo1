// =====================================================================================
// 
//       Filename:  read_bse_nfcast_struct_data.cpp
// 
//    Description:  i
// 
//        Version:  1.0
//        Created:  12/04/2023 06:49:53 AM
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

#include <vector>
#include <iostream>
#include <signal.h>
#include <math.h>
#include "dvccode/CDef/ttime.hpp"
#include "dvccode/CDef/eobi_mds_defines.hpp"
#include<bits/stdc++.h>
#include<chrono>
#include<iomanip>
#include "dvccode/Utils/bulk_file_reader.hpp"
#include <ctime>
#include <sstream>
#define N 4000000000

int main(int argc, char* argv[]){
        std::string fname(argv[1]);
        std::cerr << "File name: " << fname << std::endl;
        HFSAT::BulkFileReader bfr(N);
        bfr.open(fname);
        if (!bfr.is_open()) {
                std::cerr << "Cannot open file: " << argv[1] << std::endl;
        }

        char* data_buffer = new char[8192];
        int tc=0;
        std::ofstream fout;
        fout.open(argv[2]);
        fout << "stg_create_date|index_name|idxvalue|time_stamp" <<std::endl;
        while(true){
                
                memset((void*)data_buffer,0,sizeof(data_buffer));
                size_t available_len_=bfr.read(data_buffer, sizeof(EOBI_MDS::EOBICommonStruct));
                bool is_sensex=true; 
                if(available_len_>=sizeof(EOBI_MDS::EOBICommonStruct)){
                        EOBI_MDS::EOBICommonStruct* next_event_ = reinterpret_cast<EOBI_MDS::EOBICommonStruct*>(data_buffer);
                        if(next_event_->token_!=1)
                        {
                                is_sensex=false;
                        }
                        else{
                                tc++;
                                std::cout<<"Printing Token number : "<<next_event_->token_<<std::endl;
                                std::cout<<"Printing Source Time secs : "<<next_event_->source_time.tv_sec<<std::endl;
                                std::cout<<"Printing Source Time microsecs : "<<next_event_->source_time.tv_usec<<std::endl;
                                std::ostringstream oss;
                                time_t rawTime = next_event_->source_time.tv_sec;
                                rawTime+=5 * 3600 + 30 * 60;
                                struct tm *timeInfo = localtime(&rawTime);                                
                                oss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
                                oss << ".";
                                oss << next_event_->source_time.tv_usec; 

                                std::string formattedTime = oss.str();
                                std::cout << "Formatted Time" << formattedTime << std::endl;
                                std::string line=formattedTime.substr(0,10);
                                line.append("|");
                                line.append("SENSEX");
                                line.append("|");
                                line.append(std::to_string(next_event_->order_.price));
                                line.append("|");
                                line.append(formattedTime);
                                fout << line << std::endl;
                        }                
                }
                else{
                        break;
                }
                       
        }
        fout.close();
        std::cout<<"total sensex entries"<<tc<<std::endl;        

        bfr.close();
        delete data_buffer;
        return 0;
}
