// =====================================================================================
// 
//       Filename:  offline_bse_nfcast_data_converter.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  11/30/2023 08:46:18 AM
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

#include <iostream>
#include <fstream>
#include <string>
#include<bits/stdc++.h>
#include "dvccode/CDef/ttime.hpp"
#include "dvccode/CDef/eobi_mds_defines.hpp"
#include "dvccode/Utils/bulk_file_writer.hpp"
#include <dirent.h>


std::string get_filename(std::string unconverted_fname){
        std::stringstream ss_fname(unconverted_fname);
        std::string word;
        std::string output="";
        while(!ss_fname.eof()){
                std::getline(ss_fname,word,'/');
                output=word;
        }
        std::string fname="";
        std::stringstream ss_fname_dot(output);
        std::getline(ss_fname_dot,fname,'.');
        fname="BSE_"+fname.substr(0,6)+"_20"+fname.substr(7,6);
        return fname;
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

std::pair<double,struct timeval> get_struct_values(std::string line){
        std::stringstream ss_line(line);
        double price = 0;
        std::string time_str = "";

        std::string word;
        std::getline(ss_line,word,'|');
        std::getline(ss_line,word,'|');
        std::getline(ss_line,word,'|');
        std::cout << " Price word : "<< word << std::endl;
        price=std::stod(word);
        std::getline(ss_line,time_str,'|');
        
        //std::stringstream ss_time(time_str);
        //ss_time >> word;
        //std::string date = word;
        //ss_time >> word;
        //std::string ht_str = word;
        uint16_t year = std::stoi(time_str.substr(0,4));
        uint16_t month = std::stoi(time_str.substr(5,2));
        uint16_t day = std::stoi(time_str.substr(8,2));
        uint16_t hour = std::stoi(time_str.substr(11,2));
        uint16_t minute = std::stoi(time_str.substr(14,2));
        uint16_t second = std::stoi(time_str.substr(17,2));
        uint32_t microsec = std::stoi(time_str.substr(20,6));
        
        std::time_t unixTime_ist = get_unix_time(year,month,day,hour,minute,second);
        std::time_t unixTime_gmt = unixTime_ist - (5*3600) - (30*60);
        
        struct timeval exchange_time;
        exchange_time.tv_sec = unixTime_gmt;
        exchange_time.tv_usec = microsec;

        return {price,exchange_time};                     
}

int main(){
        
        std::string directory_path = "/home/dvcinfra/jainam/bse_nfcast_unconverted_data_bankex";
        std::vector<std::string> file_names;
                
        DIR* dir;
        struct dirent* entry;

        if ((dir = opendir(directory_path.c_str())) != nullptr) {
                while ((entry = readdir(dir)) != nullptr) {
                        file_names.push_back(entry->d_name);
                }
                closedir(dir);

                // Print the list of file names
                std::cout << "Files in directory:\n";
                for (std::string unconverted_file : file_names) {
                        if((unconverted_file == ".") || (unconverted_file == ".."))
                        continue;
                        std::cout << "Unconverted File : " << unconverted_file << '\n';
                
                        std::string unconverted_fname = directory_path + "/" + unconverted_file;

                        std::cout << "Unconverted File abs path : " << unconverted_fname << std::endl;

                        //std::string unconverted_fname = "/home/dvcinfra/jainam/bse_nfcast_unconverted_data/SENSEX30_231117.txt";
                        std::string fname = get_filename(unconverted_fname);
                        std::cout<<"Filename : "<<fname<<std::endl;
                        std::string converted_fname = "/home/dvcinfra/jainam/bse_nfcast_converted_data/bse_BANKEX_data/"+fname;
                        std::cout<<"Converted Filename : "<<converted_fname<<std::endl;
                        HFSAT::BulkFileWriter bulk_file_writer;
                        bulk_file_writer.Open(converted_fname);        

                        std::ifstream myfile(unconverted_fname);
                        std::string line;
                        int lc=0;
                        if (myfile.is_open()){
                                uint32_t msgval=0;
                                while(std::getline(myfile,line)){
                                        lc+=1;
                                        if(lc<=2)
                                        continue;
                                        std::cout<<"Printing line : "<<line<<std::endl;
                                        double price =0;
                                        struct timeval exchange_time;
                                        std::pair<double,struct timeval> struct_values_pair = get_struct_values(line);
                                        price = struct_values_pair.first;
                                        exchange_time = struct_values_pair.second;
                                        std::cout << "Price : " << std::fixed << std::setprecision(2) << price << " Exchange Time(seconds) : " << exchange_time.tv_sec << " Exchange Time(microseconds) : " << exchange_time.tv_usec <<std::endl;
                        
                                        EOBI_MDS::EOBICommonStruct* next_event_ = new EOBI_MDS::EOBICommonStruct();
                                        next_event_->source_time = exchange_time;
                                        next_event_->segment_type='E';
                                        next_event_->order_.msg_seq_num_=msgval++;
                                        next_event_->token_=12;
                                        next_event_->order_.action_='8';
                                        next_event_->order_.size=0;
                                        next_event_->order_.side=0;
                                        next_event_->order_.price=price;

                                        bulk_file_writer.Write(next_event_, sizeof(EOBI_MDS::EOBICommonStruct));
                                        bulk_file_writer.CheckToFlushBuffer(); 
                                }
                                myfile.close();                
                        }
        
                        bulk_file_writer.Close();
                }               
        } else {
                std::cerr << "Error opening directory." << std::endl;
                return EXIT_FAILURE;
        }

        return 0;
}

