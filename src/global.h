/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   global.h
 * Author: fra
 *
 * Created on September 13, 2016, 2:15 PM
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <unordered_map>
#include <vector>

namespace xwaxdb
{
  
/**
 * @brief
 * representation of a crate
 */
struct s_crate
{
  std::string path;
  std::string name_orig;   // = playlist-name
  std::string name_uniq; // unique name for use in xwax
  std::string type;        // audiofiles, gmbrc, pls
  bool b_is_new;
  int items = 0;
  int items_new = 0; // <- new detected by current session
  std::vector<std::unordered_map<std::string, std::string> > v_track;
  std::vector<unsigned int> v_afid; // audiofile-id
  std::vector<unsigned int> v_new_afid;
};

}

#endif /* GLOBAL_H */

