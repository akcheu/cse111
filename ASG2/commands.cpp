// Allen Cheung | CSE 111 | alkhcheu@ucsc.edu
// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"
#include <iomanip>

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   auto cwd = state.get_cwd();
   if (words.size() == 1) {
      cerr << words[0] << ": Not enough operands" << endl;
      return;
   } else {
      for (int i = 1; i < words.size(); i++) {
         auto cat_path = words[i];
         auto save_cwd = state.get_cwd();
         size_t found = cat_path.find("/");
         auto root = state.get_root();
         auto cwd_ent = root->get_contents()->get_dirents();
         auto cwd_itor = cwd_ent.begin();
         if (found != std::string::npos) {
            state.set_cwd(root);
            wordvec find = split(words[i], "/");
            bool found_flag {false};
            bool exit {false};
            int i = 1;
            while (exit == false) {
               if (i == find.size()) {
                  exit = true;
               }
               for (int d = 0; d < state.get_cwd()->
                    get_contents()->size(); d++) {
                  if (cwd_itor->first.compare(find[i - 1]) == 0) {
                     if (i == find.size()) {
                        if (cwd_itor->second->is_file == false) {
                           cerr << "Not a file" << endl;
                           return;
                        }
                        found_flag = true;
                        exit = true;
                        state.set_cwd(cwd_itor->second);
                        break;
                     } else {
                        state.set_cwd(cwd_itor->second);
                        cwd_itor = state.get_cwd()->
                        get_contents()->get_dirents().begin();
                        i++;
                        break;
                     }
                  }
                  cwd_itor++;
               }
            }
            if (found_flag == false) {
               cerr << "No such file or directory" << endl;
               break;
            }
            wordvec file_contents = cwd_itor->second->
            get_contents()->readfile();
            for (int i = 0; i < file_contents.size(); i++) {
               cout << file_contents[i];
            }
            cout << endl;
         } else {
            state.set_cwd(cwd);
            auto cwd_dirents = state.get_cwd()->
            get_contents()->get_dirents();
            auto cat_itor = cwd_dirents.begin();
            bool found_flag {false};
            while (cat_itor != cwd_dirents.end()) {
               if (cat_itor->first.compare(cat_path) == 0) {
                  auto check_file = cat_itor->second;
                  if (check_file->is_file == false) {
                     cerr << words[0] << ": Cannot cat a directory"
                     << endl;
                     return;
                  }
                  found_flag = true;
                  break;
               }
               cat_itor ++;
            }
            if (found_flag == false) {
               cerr << words[0] << ": No such file or directory"
               << endl;
               return;
            }
            wordvec file_contents = cat_itor->second->
            get_contents()->readfile();
            for (int i = 0; i < file_contents.size(); i++) {
               cout << file_contents[i];
            }
            cout << endl;
         }
      }
   }
   state.set_cwd(cwd);
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   if (words.size() > 2) {
      cerr << words[0] << ": Too many operands" << endl;
      return;
   }
   if (words.size() == 1) {
      auto root = state.get_root();
      state.set_cwd(root);
      state.get_cwd()->is_root = true;
      return;
   }
   auto cd_path = words[1];
   if (cd_path.compare("/") == 0) {
      auto root = state.get_root();
      state.set_cwd(root);
      state.get_cwd()->is_root = true;
      return;
   } else {
      state.get_cwd()->is_root = false;
   }
   
   size_t found = cd_path.find("/");
   if (found != std::string::npos) {
      if (found + 1 == cd_path.length()) {
         cd_path.erase(found);
      } else {
         cerr << words[0] << ": No such file or directory"
         << endl;
         return;
     }
   }
   
   auto find = state.get_cwd()->get_contents()->get_dirents();
   auto find_itor = find.begin();
   bool found_flag {false};
   while (find_itor != find.end()) {
      if (find_itor->first.compare(cd_path) == 0) {
         auto check_file = find_itor->second;
         if (check_file->is_file == true) {
            cerr << words[0] << ": Cannot cd to a file" << endl;
            return;
         }
         found_flag = true;
         break;
      }
      find_itor ++;
   }
   if (found_flag == false) {
      cerr << words[0] << ": No such file or directory" << endl;
      return;
   }
   state.set_cwd(find_itor->second);
   
   
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   for (int i = 1; i < words.size(); i++) {
      cout << words[i];
      cout << " ";
   }
   cout << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   auto curr_dir = state.get_cwd();
   if (words.size() == 1) {
      cout << state.get_cwd()->get_path() << ":" << endl;
      auto cwd_dirents = state.get_cwd()->
      get_contents()->get_dirents();
      auto dir_itor = cwd_dirents.begin();
      for (int i = 0; i < cwd_dirents.size(); i++) {
         state.set_cwd(dir_itor->second);
         cout << right << setw(6) <<
         state.get_cwd()->get_inode_nr();
         cout << "  ";
         cout << right << setw(6) <<
         state.get_cwd()->get_contents()->size();
         cout << "  " << dir_itor->first;
         if (dir_itor->second->is_file == false) {
            cout << "/";
         }
         cout << endl;
         dir_itor ++;
      }
   } else {
      for (int i = 1; i < words.size(); i++) {
         auto save_cwd = state.get_cwd();
         auto pathname = words[i];
         size_t found = pathname.find("/");
         auto root = state.get_root();
         auto cwd_ent = root->get_contents()->get_dirents();
         auto cwd_itor = cwd_ent.begin();
         if (found != std::string::npos) {
            
            state.set_cwd(root);
            wordvec find = split(words[i], "/");
            
            if (find.size() == 0) {
               state.set_cwd(state.get_root());
               cout << state.get_cwd()->get_path() << ":" << endl;
               auto itor = state.get_cwd()->get_contents()->
               get_dirents().begin();
               for (int y = 0; y < state.get_root()->
                    get_contents()->size(); y++) {
                  state.set_cwd(itor->second);
                  cout << right << setw(6) << state.get_cwd()->
                  get_inode_nr();
                  cout << "  ";
                  cout << right << setw(6) << state.get_cwd()->
                  get_contents()->size();
                  cout << "  " << itor->first;
                  if (itor->second->is_file == false) {
                     cout << "/";
                  }
                  cout << endl;
                  itor ++;
               }
               break;
            }
            // Search if first find is in root
            bool founded {false};
            auto test_itor = state.get_root()->get_contents()->
            get_dirents().begin();
            auto end_itor = state.get_root()->get_contents()->
            get_dirents().end();
            while (test_itor != end_itor) {
               if (test_itor->first.compare(find[0]) == 0) {
                  founded = true;
                  break;
               }
               test_itor ++;
            }
            if (founded == false) {
               cerr << "No such file or directory" << endl;
               return;
            }
            bool found_flag {false};
            bool exit {false};
            int i = 1;
            while (exit == false) {
               if (i == find.size()) {
                  exit = true;
               }
               for (int d = 0; d < state.get_cwd()->get_contents()->
                    size(); d++) {
                  if (cwd_itor->first.compare(find[i - 1]) == 0) {
                     if (i == find.size()) {
                        found_flag = true;
                        exit = true;
                        state.set_cwd(cwd_itor->second);
                        break;
                     } else {
                        if (cwd_itor->second->is_file == true) {
                           cerr << "No such file or directory" << endl;
                           return;
                        }
                        state.set_cwd(cwd_itor->second);
                        cwd_itor = state.get_cwd()->get_contents()->
                        get_dirents().begin();
                        i++;
                        break;
                     }
                  } else {
                     cwd_itor++;
                  }
               }
            }
            if (found_flag == false) {
               cerr << "No such file or directory" << endl;
               break;
            }
            auto cwd_entries = state.get_cwd()->get_contents()->
            get_dirents();
            cout << state.get_cwd()->get_path() << ":" << endl;
            auto dir_iterator = cwd_entries.begin();
            for (int k = 0; k < cwd_entries.size(); k++) {
               state.set_cwd(dir_iterator->second);
               cout << right << setw(6) << state.get_cwd()->
               get_inode_nr();
               cout << "  ";
               cout << right << setw(6) << state.get_cwd()->
               get_contents()->size();
               cout << "  " << dir_iterator->first;
               if (dir_iterator->second->is_file == false) {
                     cout << "/";
               }
               cout << endl;
               dir_iterator ++;
            }
            
         } else {
            auto cwd_dirents = state.get_cwd()->get_contents()->
            get_dirents();
            auto ret_itor = cwd_dirents.begin();
            bool found_fg {false};
            while (ret_itor != cwd_dirents.end()) {
               if (ret_itor->first.compare(pathname) == 0) {
                  found_fg = true;
                  break;
               }
               ret_itor ++;
            }
            if (found_fg == false) {
               cerr << words[0] << ": No such file or directory"
               << endl;
               return;
            }
            
            state.set_cwd(ret_itor->second);
            cwd_dirents = state.get_cwd()->get_contents()->
            get_dirents();
            cout << state.get_cwd()->get_path() << ":" << endl;
            auto dir_itor = cwd_dirents.begin();
            for (int j = 0; j < cwd_dirents.size(); j++) {
               state.set_cwd(dir_itor->second);
               cout << right << setw(6) << state.get_cwd()->
               get_inode_nr();
               cout << "  ";
               cout << right << setw(6) << state.get_cwd()->
               get_contents()->size();
               cout << "  " << dir_itor->first;
               if (dir_itor->second->is_file == false) {
                  cout << "/";
               }
               cout << endl;
               dir_itor ++;
            }
         }
         state.set_cwd(save_cwd);
      }
   }
   state.set_cwd(curr_dir);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   auto curr_dir = state.get_cwd();
   if (words.size() == 2 and words[1].compare("/") == 0) {
      state.set_cwd(state.get_root());
   } else {
      auto cwd_itor = state.get_cwd()->get_contents()->
      get_dirents().begin();
      for (int i = 1; i < words.size(); i++) {
         auto save_cwd = state.get_cwd();
         auto pathname = words[i];
         size_t found = pathname.find("/");
         auto root = state.get_root();
         auto cwd_ent = root->get_contents()->get_dirents();
         cwd_itor = cwd_ent.begin();
         if (found != std::string::npos) {
            
            state.set_cwd(root);
            wordvec find = split(words[i], "/");
            
            bool founded {false};
            auto test_itor = state.get_root()->get_contents()->
            get_dirents().begin();
            auto end_itor = state.get_root()->get_contents()->
            get_dirents().end();
            while (test_itor != end_itor) {
               if (test_itor->first.compare(find[0]) == 0) {
                  founded = true;
                  break;
               }
               test_itor ++;
            }
            if (founded == false) {
               cerr << "No such file or directory" << endl;
               return;
            }
            
            bool found_flag {false};
            bool exit {false};
            int i = 1;
            while (exit == false) {
               if (i == find.size()) {
                  exit = true;
               }
               for (int d = 0; d < state.get_cwd()->
                    get_contents()->size(); d++) {
                  if (cwd_itor->first.compare(find[i - 1]) == 0) {
                     if (i == find.size()) {
                        found_flag = true;
                        exit = true;
                        state.set_cwd(cwd_itor->second);
                        break;
                     } else {
                        if (cwd_itor->second->is_file == true) {
                           cerr << "No such file or directory" << endl;
                           return;
                        }
                        state.set_cwd(cwd_itor->second);
                        cwd_itor = state.get_cwd()->
                        get_contents()->get_dirents().begin();
                        i++;
                        break;
                     }
                  } else {
                     cwd_itor++;
                  }
               }
            }
            if (found_flag == false) {
               cerr << "No such file or directory" << endl;
               break;
            }
         } else {
            bool find {false};
            auto iter = state.get_root()->get_contents()->
            get_dirents().begin();
            auto end_iter = state.get_root()->get_contents()->
            get_dirents().end();
            while (iter != end_iter) {
               if (iter->first.compare(words[1]) == 0) {
                  find = true;
                  break;
               }
               iter ++;
            }
            if (find == false) {
               cerr << "No such file or directory" << endl;
               return;
            }
            cwd_itor = iter;
         }
      }
      state.set_cwd(cwd_itor->second);
   }
   
   
   bool end_flag {false};
   bool found_directory {false};
   while (end_flag == false){
      cout << state.get_cwd()->get_path() << ":" << endl;
      auto cwd_dirents = state.get_cwd()->get_contents()->
      get_dirents();
      auto dir_itor = cwd_dirents.begin();
      auto save_directory = dir_itor;
      for (int i = 0; i < cwd_dirents.size(); i++) {
         state.set_cwd(dir_itor->second);
         cout << right << setw(6) << state.get_cwd()->
         get_inode_nr();
         cout << "  ";
         cout << right << setw(6) << state.get_cwd()->
         get_contents()->size();
         cout << "  " << dir_itor->first;
         if (dir_itor->second->is_file == false) {
            cout << "/";
            if (dir_itor->first.compare(".") != 0 and dir_itor->
                first.compare("..") != 0) {
               if (found_directory == false) {
                  found_directory = true;
                  save_directory = dir_itor;
               }
            }
         }
         cout << endl;
         dir_itor ++;
      }
      
      if (found_directory == true) {
         state.set_cwd(save_directory->second);
         found_directory = false;
      } else {
         end_flag = true;
      }
   }
   state.set_cwd(curr_dir);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   if (words.size() == 1) {
      cerr << words[0] << ": Not enough operands" << endl;
      return;
   }
   auto save_cwd = state.get_cwd();
   auto pathname = words[1];
   size_t found1 = pathname.find("/");
   auto root = state.get_root();
   auto cwd_ent = root->get_contents()->get_dirents();
   auto cwd_itor = cwd_ent.begin();
   if (found1 != std::string::npos) {
      state.set_cwd(root);
      wordvec find = split(words[1], "/");
      if (find.size() == 0) {
         cerr << "File name already exists" << endl;
         return;
      }
      // Search if first find is in root
      bool founded {false};
      auto test_itor = state.get_root()->get_contents()->
      get_dirents().begin();
      auto end_itor = state.get_root()->get_contents()->
      get_dirents().end();
      while (test_itor != end_itor) {
         if (test_itor->first.compare(find[0]) == 0) {
            founded = true;
            break;
         }
         test_itor ++;
      }
      if (founded == false) {
         cout << "No such file or directory" << endl;
         return;
      }
      
      bool found_flag {false};
      bool exit {false};
      int i = 1;
      while (exit == false) {
         if (i == find.size() - 1) {
            exit = true;
         }
         while (cwd_itor != state.get_cwd()->get_contents()
                ->get_dirents().end()) {
            if (cwd_itor->first.compare(find[i - 1]) == 0) {
               if (i == find.size() - 1) {
                  found_flag = true;
                  exit = true;
                  state.set_cwd(cwd_itor->second);
                  break;
               } else {
                  if (cwd_itor->second->is_file == true) {
                     cerr << "No such file or directory" << endl;
                     return;
                  }
                  state.set_cwd(cwd_itor->second);
                  cwd_itor = state.get_cwd()->get_contents()->
                  get_dirents().begin();
                  i++;
                  break;
               }
            } else {
               cwd_itor++;
            }
         }
      }
      if (found_flag == false) {
         cerr << "No such file or directory" << endl;
         state.set_cwd(save_cwd);
         return;
      }
      wordvec file_contents;
      if (words.size() > 2) {
         int i {2};
         while (i != words.size()) {
            file_contents.push_back(words[i]);
            i++;
            if (i != words.size()) {
               file_contents.push_back(" ");
            }
         }
      }
      
      auto find_file = state.get_cwd()->get_contents()->
      get_dirents();
      auto find_itor = find_file.begin();
      while (find_itor != find_file.end()) {
         if (find_itor->first.compare(find[i]) == 0) {
            if (find_itor->second->is_file == false) {
               cerr << "Cannot overwrite directory" << endl;
            } else {
               find_itor->second->get_contents()->
               writefile(file_contents);
               state.set_cwd(save_cwd);
               return;
            }
         }
         find_itor++;
      }
      auto new_file = state.get_cwd()->get_contents()->
      mkfile(find[i]);
      new_file->get_contents()->writefile(file_contents);
      state.set_cwd(save_cwd);
      return;
   }
   auto file_name = words[1];
   bool found_flag {false};
   auto check_rep = state.get_cwd()->get_contents()->
   get_dirents();
   auto itor_check = check_rep.begin();
   auto file_ptr = itor_check->second;
   while (itor_check != check_rep.end()) {
      if (itor_check->first.compare(file_name) == 0) {
         file_ptr = itor_check->second;
         if (file_ptr->is_file == false) {
            cerr << words[0] << ": Cannot overwrite directory"
            << endl;
            return;
         }
         found_flag = true;
         break;
      }
      itor_check ++;
   }

   wordvec file_contents;
   if (words.size() > 2) {
      int i {2};
      while (i != words.size()) {
         file_contents.push_back(words[i]);
         i++;
         if (i != words.size()) {
            file_contents.push_back(" ");
         }
      }
   }

   
   if (found_flag == true) {
      file_ptr->get_contents()->writefile(file_contents);
      return;
   } else {
      auto new_file = state.get_cwd()->
      get_contents()->mkfile(file_name);
      new_file->get_contents()->writefile(file_contents);
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   if (words.size() == 1) {
      cerr << words[0] << ": Too little operands" << endl;
      return;
   }
   if (words.size() > 2) {
      cerr << words[0] << ": Too many operands" << endl;
      return;
   }
   
   auto save_cwd = state.get_cwd();
   auto pathname = words[1];
   size_t found1 = pathname.find("/");
   auto root = state.get_root();
   auto cwd_ent = root->get_contents()->get_dirents();
   auto cwd_itor = cwd_ent.begin();
   if (found1 != std::string::npos) {
      state.set_cwd(root);
      wordvec find = split(words[1], "/");
      if (find.size() == 0) {
         cerr << "File name already exists" << endl;
         return;
      }
      // Search if first find is in root
      bool founded {false};
      auto test_itor = state.get_root()->
      get_contents()->get_dirents().begin();
      auto end_itor = state.get_root()->
      get_contents()->get_dirents().end();
      while (test_itor != end_itor) {
         if (test_itor->first.compare(find[0]) == 0) {
            founded = true;
            break;
         }
         test_itor ++;
      }
      if (founded == false) {
         cerr << "No such file or directory" << endl;
         return;
      }
      
      bool found_flag {false};
      bool exit {false};
      int i = 1;
      while (exit == false) {
         if (i == find.size() - 1) {
            exit = true;
         }
         while (cwd_itor != state.get_cwd()->
                get_contents()->get_dirents().end()) {
            if (cwd_itor->first.compare(find[i - 1]) == 0) {
               if (i == find.size() - 1) {
                  found_flag = true;
                  exit = true;
                  state.set_cwd(cwd_itor->second);
                  break;
               } else {
                  if (cwd_itor->second->is_file == true) {
                     cerr << "No such file or directory"
                     << endl;
                     return;
                  }
                  state.set_cwd(cwd_itor->second);
                  cwd_itor = state.get_cwd()->
                  get_contents()->get_dirents().begin();
                  i++;
                  break;
               }
            } else {
               cwd_itor++;
            }
         }
      }
      if (found_flag == false) {
         cerr << "No such file or directory" << endl;
         state.set_cwd(save_cwd);
         return;
      }
      
      auto find_file = state.get_cwd()->get_contents()->
      get_dirents();
      auto find_itor = find_file.begin();
      while (find_itor != find_file.end()) {
         if (find_itor->first.compare(find[i]) == 0) {
            cerr << words[0] << ": Directory name already taken"
            << endl;
            return;
         }
         find_itor++;
      }
      auto curr_directory = state.get_cwd();
      string new_pathname;
      string current_pathname = state.get_cwd()->
      get_path();
      if (state.get_cwd()->is_root == false) {
         new_pathname = current_pathname + "/" + find[i];
      } else {
         new_pathname = current_pathname + find[i];
      }
      auto new_directory = state.get_cwd()->get_contents()->
      mkdir(find[i]);
      state.set_cwd(new_directory);
      state.get_cwd()->get_contents()->init_dir(curr_directory
                                                , new_directory);
      state.get_cwd()->set_path(new_pathname);
      state.set_cwd(save_cwd);
      return;
   }
      
   auto curr_dir = state.get_cwd();
   string current_path = curr_dir->get_path();
   string new_dirname = words[1];

   size_t found = new_dirname.find("/");
   if (found != std::string::npos) {
      if (found + 1 == new_dirname.length()) {
         new_dirname.erase(found);
      } else {
         cerr << words[0] << ": Invalid directory name"
         << endl;
         return;
      }
   }
   auto check_rep = state.get_cwd()->get_contents()->
   get_dirents();
   auto itor_check = check_rep.begin();
   while (itor_check != check_rep.end()) {
      if (itor_check->first.compare(new_dirname) == 0) {
         cerr << words[0] << ": Directory name already taken"
         << endl;
         return;
      }
      itor_check ++;
   }
 
   string new_path;
   if (state.get_cwd()->is_root == false) {
      new_path = current_path + "/" + new_dirname;
   } else {
      new_path = current_path + new_dirname;
   }
   
   auto new_dir = state.get_cwd()->get_contents()->
   mkdir(new_dirname);
   state.set_cwd(new_dir);
   state.get_cwd()->get_contents()->init_dir(curr_dir,
                                             new_dir);
   state.get_cwd()->set_path(new_path);
   state.set_cwd(curr_dir);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string new_prompt;
   // Create new prompt from words from cmd line
   for (size_t i = 1; i < words.size(); i++){
    new_prompt += words[i] + " ";
   }
   // Make sure there is a space after new prompt
   state.prompt_ = new_prompt;
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.get_cwd()->get_path() << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   if (words.size() == 1) {
      cerr << words[0] << ": Not enough operands" << endl;
      return;
   }
   if (words.size() > 2) {
      cerr << words[0] << ": Too many operands" << endl;
      return;
   }
   auto rm_path = words[1];
   
   size_t found = rm_path.find("/");
   if (found != std::string::npos) {
      if (found + 1 == rm_path.length()) {
         rm_path.erase(found);
      } else {
         cerr << words[0] << ": No such file or directory"
         << endl;
         return;
     }
   }
   
   auto cwd_dirents = state.get_cwd()->get_contents()->
   get_dirents();
   bool found_flag {false};
   auto itor_check = cwd_dirents.begin();
   while (itor_check != cwd_dirents.end()) {
      if (itor_check->first.compare(rm_path) == 0) {
         found_flag = true;
         break;
      }
      itor_check ++;
   }
   
   if (found_flag == false) {
      cerr << words[0] << ": No such file or directory"
      << endl;
      return;
   } else {
      if (itor_check->second->is_file == true) {
         state.get_cwd()->get_contents()->remove(rm_path);
      } else {
         if (itor_check->second->get_contents()->size() > 2) {
            cerr << words[0] <<
            ": Cannot delete non-empty directory" << endl;
         } else {
            state.get_cwd()->get_contents()->remove(rm_path);
         }
      }
   }

}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   auto curr_dir = state.get_cwd();
   if (words.size() == 1) {
      cerr << "Not enough operands" << endl;
      return;
   } else {
      auto cwd_itor = state.get_cwd()->get_contents()->
      get_dirents().begin();
      for (int i = 1; i < words.size(); i++) {
         auto save_cwd = state.get_cwd();
         auto pathname = words[i];
         size_t found = pathname.find("/");
         auto root = state.get_root();
         auto cwd_ent = root->get_contents()->get_dirents();
         cwd_itor = cwd_ent.begin();
         if (found != std::string::npos) {
            
            state.set_cwd(root);
            wordvec find = split(words[i], "/");
            
            // Search if first find is in root
            bool founded {false};
            auto test_itor = state.get_root()->get_contents()->
            get_dirents().begin();
            auto end_itor = state.get_root()->get_contents()->
            get_dirents().end();
            while (test_itor != end_itor) {
               if (test_itor->first.compare(find[0]) == 0) {
                  founded = true;
                  break;
               }
               test_itor ++;
            }
            if (founded == false) {
               cerr << "No such file or directory" << endl;
               return;
            }
            
            bool found_flag {false};
            bool exit {false};
            int i = 1;
            while (exit == false) {
               if (i == find.size()) {
                  exit = true;
               }
               for (int d = 0; d < state.get_cwd()->
                    get_contents()->size(); d++) {
                  if (cwd_itor->first.compare(find[i - 1])
                      == 0) {
                     if (i == find.size()) {
                        found_flag = true;
                        exit = true;
                        state.set_cwd(cwd_itor->second);
                        break;
                     } else {
                        if (cwd_itor->second->is_file == true) {
                           cerr << "No such file or directory"
                           << endl;
                           return;
                        }
                        state.set_cwd(cwd_itor->second);
                        cwd_itor = state.get_cwd()->
                        get_contents()->get_dirents().begin();
                        i++;
                        break;
                     }
                  } else {
                     cwd_itor++;
                  }
               }
            }
            if (found_flag == false) {
               cerr << "No such file or directory" << endl;
               break;
            }
         } else {
            bool find {false};
            auto iter = state.get_root()->get_contents()->
            get_dirents().begin();
            auto end_iter = state.get_root()->get_contents()->
            get_dirents().end();
            while (iter != end_iter) {
               if (iter->first.compare(words[1]) == 0) {
                  find = true;
                  break;
               }
               iter ++;
            }
            if (find == false) {
               cerr << "No such file or directory" << endl;
               return;
            }
            cwd_itor = iter;
         }
      }
      state.set_cwd(cwd_itor->second);
   }
   
   vector<inode_ptr> store_direct;
   bool end_flag {false};
   bool found_directory {false};
   while (end_flag == false){
      auto cwd_dirents = state.get_cwd()->get_contents()->
      get_dirents();
      auto dir_itor = cwd_dirents.begin();
      auto save_directory = dir_itor;
      for (int i = 0; i < cwd_dirents.size(); i++) {
         state.set_cwd(dir_itor->second);
         if (dir_itor->second->is_file == false) {
            if (dir_itor->first.compare(".") != 0 and dir_itor->
                first.compare("..") != 0) {
               if (found_directory == false) {
                  found_directory = true;
                  save_directory = dir_itor;
                  store_direct.push_back(save_directory->second);
               }
            }
         }
         dir_itor ++;
      }
      if (found_directory == true) {
         state.set_cwd(save_directory->second);
         found_directory = false;
      } else {
         state.set_cwd(save_directory->second);
         end_flag = true;
      }
   }
   auto deleter = state.get_cwd()->get_contents()->
   get_dirents().begin();
   auto end = state.get_cwd()->get_contents()->
   get_dirents().end();
   for (int i = 0; i < store_direct.size(); i++) {
      while (deleter != end) {
         state.get_cwd()->get_contents()->
         remove(deleter->first);
         deleter ++;
      }
      state.set_cwd(store_direct.back());
      store_direct.pop_back();
   }
   wordvec first_op = split(words[1], "/");
   state.set_cwd(curr_dir);
   state.get_cwd()->get_contents()->remove(first_op[0]);
}

