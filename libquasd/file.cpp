/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   File.cpp
 * Author: fra
 * 
 * Created on February 11, 2016, 3:35 PM
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

#include "libquasd/file.h"

#include "boost/filesystem.hpp"

struct recursive_directory_range
{
    typedef boost::filesystem::recursive_directory_iterator iterator;
    recursive_directory_range(boost::filesystem::path p) : p_(p) {}

    iterator begin() { return boost::filesystem::recursive_directory_iterator(p_); }
    iterator end() { return boost::filesystem::recursive_directory_iterator(); }

    boost::filesystem::path p_;
};

using namespace quasd;

file::file(const std::string f) : filepath(f) {}
file::file(const file& orig) {}
file::~file() {}

int
file::is_file ()
{
    return is_file (filepath);
}

int
file::is_file (const std::string &f)
{
    /* C: unistd,h */
    struct stat s;
        
    if (stat (f.c_str(), &s) == -1)
    {
        std::cerr << "ERR:" << __FILE__ << ":" << __FUNCTION__
                  << ":cannot stat file"
                  << std::endl;
        return -1;
    }    
    else
    {
        /* try to open read only: fcntl.h */
        int fd = open (f.c_str(), O_RDONLY);
        if (fd == -1)
        {
            std::cerr << "ERR:" << __FILE__ << ":" << __FUNCTION__
                      << ":no permission to open read only"
                      << std::endl;
            return 1;
        }        
    }
    
    return 0;
}

bool file::is_regular_file(const std::string& f)
{
    return boost::filesystem::is_regular_file(f);
}


bool
file::is_dir ()
{
    return is_dir (filepath);
}

bool
file::is_dir(const std::string &f)
{
    struct stat s;
    if (stat (f.c_str(), &s) == -1)
        return false;
    
    if ( (s.st_mode & S_IFMT) == S_IFDIR)
        return true;
    else
        return false;
        
    /* given path is a directory */
//    switch (s.st_mode & S_IFMT)
//    {
//        case S_IFDIR:
//            std::cerr << "ERR:" << __FILE__ << ":" << __FUNCTION__
//                      << ":directory (not file)"
//                      << std::endl;
//            return 2;
//            break;
//    }
}


void
file::remove_dotslash(std::string &f)
{
    if (f.size() > 2)
    {
        if (f.at(0) == '.' && f.at(1) == '/')
            f = f.substr(2, f.length()-2); // remove leading "./"
    }
}


std::string
file::get_extension ()
{
    return get_extension(filepath);
}

std::string
file::get_extension (const std::string &f)
{   
    if(f.find_last_of(".") != std::string::npos)
    {
        return f.substr(f.find_last_of("."), f.size());
    }
    
    return std::string("");
}

std::string
file::get_extension_tolower(const std::string& f)
{
    std::string ext = file::get_extension(f);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);    
    return ext;
}


std::string
file::get_path ()
{
    return get_path(filepath);
}

std::string
file::get_path (const std::string &f)
{    
    std::string path = f;
    remove_dotslash (path);
    
    if(path.find_last_of("/\\") != std::string::npos)
        return path.substr(0, path.find_last_of("/\\"));
    else
        return "";
}

std::string
file::get_path_full ()
{
    return get_path_full (filepath);
}

std::string
file::get_path_full (const std::string &f)
{
    std::string path = get_path (f); // get relative path
    
    if (!path.empty() && path.at(0) == '/')
        return path; // already full-path
    else
    {
        std::ostringstream fullpath;
        fullpath << getcwd(NULL, 0); // current working path
        
        // if file is not in current working dir but in relative path
        if (!path.empty())
            fullpath << "/" << path;
        
        return fullpath.str();
    }
}

std::string
file::get_filename ()
{
    return get_filename_without_extension(filepath);
}

std::string
file::get_filename (const std::string &f)
{
    std::string filename_ext;
    if(f.find_last_of("/\\") != std::string::npos) 
    {
      return f.substr(f.find_last_of("/\\")+1);
    } 

    return std::string("");
}

std::string
file::get_filename_without_extension ()
{
    return get_filename_without_extension(filepath);
}

std::string
file::get_filename_without_extension (const std::string &f)
{
    std::string filename_ext;
    if(f.find_last_of("/\\") != std::string::npos) 
    {
      filename_ext = f.substr(f.find_last_of("/\\")+1);
      return filename_ext.substr(0, filename_ext.find_last_of("."));
    } 

    // no extension
    return std::string("");
}

std::string
file::get_basename ()
{
    return get_basename (filepath);
}

std::string
file::get_basename (const std::string &f)
{
    std::string path = f;
    remove_dotslash(path);
    
    return path.substr(path.find_last_of("/\\")+1);    
}

std::string
file::get_filepath ()
{
    /* return (relative) filepath (Given by Constructor) */
    return filepath;
}

std::string
file::get_filepath_full ()
{    
    return get_filepath_full(filepath);
}

std::string
file::get_filepath_full (const std::string &f)
{
    std::string path = get_path_full (f);
    std::string basename = get_basename (f);
    std::ostringstream ret;
    ret << path << "/" << basename;
    return ret.str();
}

/* size */
unsigned int
file::get_size_bytes ()
{
   return get_size_bytes (filepath);
}

unsigned int
file::get_size_bytes (const std::string &f)
{
    struct stat sb;
    if (stat(f.c_str(), &sb) == 0)
        return sb.st_size;
    else
        return 0;
}

double
file::get_size_kb ()
{
   return get_size_kb (filepath);
}

double
file::get_size_kb (const std::string &f)
{
    struct stat sb;
    if (stat(f.c_str(), &sb) == 0)
        return sb.st_size/(double)1000;
    else
        return 0;
}

double
file::get_size_mb ()
{
    return get_size_mb (filepath);
}

double
file::get_size_mb (const std::string &f)
{
    struct stat sb;
    
    if (stat(f.c_str(), &sb) == 0)
        return sb.st_size/1000/(double)1000;
    else
        return 0;
}

std::vector<std::string>
file::get_dir_content(const std::string& path)
{
    std::vector<boost::filesystem::path> v_content;
    std::vector<std::string> v_ret;
    try
    {
        if ( boost::filesystem::is_directory(path) )
        {        
            std::copy(boost::filesystem::directory_iterator(path), 
                    boost::filesystem::directory_iterator(), 
                    std::back_inserter(v_content));
        }
        
        for (const auto entry : v_content)
        {
            v_ret.push_back(entry.string());
        }
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ex.what()
                << std::endl;
    }
    
    return std::move(v_ret);
}

std::vector<std::string>
file::get_dir_content(const std::string& path, const std::string& ext)
{
    std::vector<std::string> v_ret;
    try
    {
        boost::filesystem::directory_iterator it(path);
        boost::filesystem::directory_iterator it_end;

        while ( it != it_end )
        {
            if ( boost::filesystem::is_regular(*it) && 
                 file::get_extension_tolower(it->path().string()) == ext )
            {
                v_ret.push_back(it->path().string());
            }
            it++;
        }
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ex.what()
                << std::endl;
    }

    return std::move(v_ret);

}

std::vector<std::string>
file::get_dir_content(const std::string& path, 
                      const std::vector<std::string>& extensions)
{    
    std::vector<std::string> v_ret;
    try
    {
        boost::filesystem::directory_iterator it(path);
        boost::filesystem::directory_iterator it_end;

        while ( it != it_end )
        {
            if ( boost::filesystem::is_regular(*it) )
            {
                for (const auto ext : extensions)
                {
                    if ( file::get_extension_tolower(it->path().string()) == ext )
                    {
                        v_ret.push_back(it->path().string());
                    }                
                }                
            }
            it++;
        }
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ex.what()
                << std::endl;
    }

    return std::move(v_ret);
}



std::vector<std::string>
file::get_dir_content_recursive(const std::string& path)
{
    std::vector<std::string> v_ret;
    try
    {
        for (auto it : recursive_directory_range(path))
        {
            v_ret.push_back(it.path().string());
        }
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ex.what()
                << std::endl;
    }
    
    return std::move(v_ret);
}

std::vector<std::string>
file::get_dir_content_recursive(const std::string& path, const std::string& ext)
{
    std::vector<std::string> v_ret;
    try
    {
        boost::filesystem::recursive_directory_iterator it(path);
        boost::filesystem::recursive_directory_iterator it_end;

        while ( it != it_end )
        {
            if ( boost::filesystem::is_regular_file(*it) &&
                  file::get_extension_tolower(it->path().string()) == ext
               )
            {
                v_ret.push_back(it->path().string());
            }
            it++;
        }
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ex.what()
                << std::endl;
    }

    return std::move(v_ret);
}

std::vector<std::string>
file::get_dir_content_recursive(const std::string& path,
                                const std::vector<std::string>& extensions)
{
    std::vector<std::string> v_ret;
    try
    {
        boost::filesystem::recursive_directory_iterator it(path);
        boost::filesystem::recursive_directory_iterator it_end;

        while ( it != it_end )
        {
            if ( boost::filesystem::is_regular_file(*it) )
            {
                for (const auto ext : extensions)
                {
                    if ( boost::filesystem::is_regular_file(*it) &&
                         file::get_extension_tolower(it->path().string()) == ext 
                       )
                    {
                        v_ret.push_back(it->path().string());
                    }                
                }                
            }
            it++;
        }
    }
    catch (const boost::filesystem::filesystem_error& ex)
    {
        std::cerr << "ERR:" << __PRETTY_FUNCTION__
                << ex.what()
                << std::endl;
    }

    return std::move(v_ret);
}
