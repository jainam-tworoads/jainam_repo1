// =====================================================================================
// 
//       Filename:  filesystem_exp.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  11/30/2023 01:18:31 PM
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
#include<bits/stdc++.h>
#include <cstdlib>
#include <iostream>
#include<dirent.h>

int main(){
        std::string directory_path = "/home/dvcinfra/jainam/bse_nfcast_unconverted_data";
        

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
                        std::cout << unconverted_file << '\n';

                        std::string unconverted_fname = directory_path + "/" + unconverted_file;

                        std::cout << "Unconverted File abs path : " << unconverted_fname << std::endl;

                }
        } else {
                std::cerr << "Error opening directory." << std::endl;
                return EXIT_FAILURE;
        }
        return 0;
}
