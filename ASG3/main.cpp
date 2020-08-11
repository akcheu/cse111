// Allen Cheung | CSE111 | alkhcheu@ucsc.edu
// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;
typedef listmap<string, string, xless<string>>::iterator test_itor;

str_str_map test;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void remove_spaces(string &keyvalue) {
   int i {0};
   while (keyvalue[i] == ' ') {
      keyvalue.erase(i, 1);
   }
   int j = keyvalue.length() - 1;
   while (keyvalue[j] == ' ') {
      keyvalue.erase(j, 1);
      j--;
   }
}

void process (const string &file_name, istream &file) {  
   int line_num {1};
   while (true) {
      string input, key, value;
      bool key_all_space {true};
      bool val_all_space {true};
      getline(file, input);
      if (file.eof()) break;
      cout << file_name << ": " << line_num << ": " << input << endl;
      line_num++;
      if (input[0] == '#') {
         continue;
      }
      size_t found = input.find("=");
      if (found != std::string::npos) {
         if (found != 0) {
            key = input.substr(0, found);
            if (key.length() == 1) {
               if (key[0] != ' ') {
                  key_all_space = false;
               }
            } else {
               for (int i = 0; i < static_cast<int>(key.length())
                    - 1; i++) {
                  if (key[i] != ' ') {
                     key_all_space = false;
                     break;
                  }
               }
            }
         }
         if (found != input.length() - 1) {
            value = input.substr(found + 1, input.length() - 1);
            if (value.length() == 1) {
               if (value[0] != ' ') {
                  val_all_space = false;
               }
            } else {
               for (int i = 0; i < static_cast<int>(value.length())
                    - 1; i++) {
                  if (value[i] != ' ') {
                     val_all_space = false;
                     break;
                  }
               }
            }
         }
         // case: =
         if (key_all_space == true && val_all_space == true) {
            test_itor printer = test.end();
            --printer;
            while (printer != test.begin()) {
               cout << printer->first << " = " << printer->second
               << endl;
               --printer;
            }
            cout << printer->first << " = " << printer->second
            << endl;
         }
         // case: key =
         else if (val_all_space == true) {
            remove_spaces(key);
            test_itor remove_key = test.find(key);
            if (remove_key != test.end()) {
               test.erase(remove_key);
            }
         }
         // case: key = value
         else if (key_all_space == false && val_all_space == false) {
            remove_spaces(value);
            remove_spaces(key);
            str_str_pair insert_pair(key, value);
            test.insert(insert_pair);
            cout << key << " = " << value << endl;
         }
         // case: = value
         else if (key_all_space == true) {
            remove_spaces(value);
            test_itor print_value = test.end();
            --print_value;
            while (print_value != test.begin()) {
               if (print_value->second == value) {
                  cout << print_value->first << " = "
                  << print_value->second << endl;
               }
               --print_value;
            }
            if (print_value->second == value) {
               cout << print_value->first << " = "
               << print_value->second << endl;
            }
         }         
      } else {
         // case: key
         key = input.substr(0, found);
         if (key.length() == 1) {
            if (key[0] != ' ') {
               key_all_space = false;
            }
         } else if (key.length() == 0) {
            key_all_space = true;
         } else {
            for (int i = 0; i < static_cast<int>(key.length())
                 - 1; i++) {
               if (key[i] != ' ') {
                  key_all_space = false;
                  break;
               }
            }
         }
         if (key_all_space == false) {
            remove_spaces(key);
            test_itor found_key = test.find(key);
            if (found_key == test.end()) {
               cout << key <<": key not found" << endl;
            } else {
               cout << (*found_key).first << " = "
               << (*found_key).second << endl;
            }
         }
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   bool files_flag {false};
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {

      files_flag = true;
      string file = *argp;
      ifstream in(file);
      if (in.fail()) {
         syscall_error(file);
         continue;
      }
      process(file, in);
      in.close();
      if (test.empty() == false) {
         test_itor clear = test.begin();
         while (clear != test.end()) {
            clear = test.erase(clear);
         }
      }
   }
   if (files_flag == false) {
      process ("-", cin);
      if (test.empty() == false) {
         test_itor clear1 = test.begin();
         while (clear1 != test.end()) {
            clear1 = test.erase(clear1);
         }
      }
   }  
   return EXIT_SUCCESS;
}

