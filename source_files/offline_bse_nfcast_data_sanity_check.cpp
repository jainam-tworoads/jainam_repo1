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
        price=std::stod(word);
        std::getline(ss_line,time_str,'|');
        if(time_str.length()!=26)
        std::cout << "[WARN] time_str length is not equal to 26. The length of it is : " << time_str.length() << std::endl;         
        
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
        std::string unconverted_fname = "/home/dvcinfra/jainam/bse_nfcast_unconverted_data/SENSEX30_231117.txt";
        std::string fname = get_filename(unconverted_fname);
        std::cout<<"Filename : "<<fname<<std::endl;
        std::string converted_fname = "/home/dvcinfra/jainam/bse_nfcast_converted_data/"+fname;
        std::cout<<"Converted Filename : "<<converted_fname<<std::endl;
                

        std::ifstream myfile(unconverted_fname);
        std::string line;
        int lc=0;
        if (myfile.is_open()){
                double prev_price=0;
                double min_price = 10000000000;
                double max_price = -10000000000;
                std::time_t prev_time = 0;
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
                        if(lc!=3){
                                if(exchange_time.tv_sec-prev_time >= 3)
                                std::cout<<"[WARN] More than 2 seconds data is missing. Total duration of data missing : " << exchange_time.tv_sec-prev_time << std::endl;
                                
                                if((price - prev_price)/prev_price >= 0.1)
                                std::cout<<"[WARN] Consecutive Price difference is greater than 10 percent. Actual Consecutive price difference is : " << std::fixed << std::setprecision(2) << (price - prev_price)/prev_price << std::endl; 
                               
                        }
                        prev_price = price;
                        prev_time = exchange_time.tv_sec;                       
                        if(price>max_price)
                        max_price=price;
                        if(price<min_price)
                        min_price=price; 
                }
                if((max_price - min_price)/min_price >= 0.2)
                                std::cout<<"[WARN] Max-Min Price difference is greater than 20 percent. Actual Max-Min price difference is : " << std::fixed << std::setprecision(2) << (max_price - min_price)/min_price << std::endl; 

                myfile.close();                
        }
        
        
        return 0;
}

