/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   File.h
 * Author: fra
 *
 * Created on February 11, 2016, 3:35 PM
 */

#ifndef GATS_FILE_H
#define GATS_FILE_H

#include <stdio.h>
#include <string>
#include <vector>

namespace quasd
{
    class file {
        /* file-path */
        std::string filepath;  // /foo/bar/example.wav

    public:
        file(const std::string);
        file(const file& orig);
        virtual ~file();
        
    /* file validation */
        
        /**
         * @brief Returns 0 if file exists and is a regular file
         * Returns -1 if the file cannot be accessed
         * @return int 
         */
        int is_file ();
        /**
         * @brief Returns 0 if file exists and is a regular file
         * Returns -1 if the file cannot be accessed
         * @return int 
         */
        static int is_file (const std::string &f);
        
        /**
         * @brief uses boost::filesystem::is_regular_file
         * @param std::string f
         * @return bool
         */
        static bool is_regular_file (const std::string& f);
        
        bool is_dir ();
        static bool is_dir (const std::string &f);
        
    /* path, name, extension */
        
        /**
         * @brief Returns File-Extension with leading dot or empty string
         * 
         * @param std::string f (filepath)
         * @return std::string file-extension
         */
        std::string get_extension ();
        
        /**
         * @brief Returns File-Extension with leading dot or empty string
         * 
         * @param std::string f (filepath)
         * @return std::string file-extension
         */        
        static std::string get_extension (const std::string &f);

        /**
         * @brief Returns File-Extension with leading dot or empty string
         * 
         * @param std::string f (filepath)
         * @return std::string file-extension
         */                
        static std::string get_extension_tolower (const std::string &f);
        
        /**
         * @brief Returns the (relative) path without basename
         * @return std::string path
         */
        std::string get_path ();
        
        /**
         * @brief Returns the full path without basename
         * @param std::string f (filepath)
         * @return std::string path
         */
        static std::string get_path (const std::string &f);
        
        /**
         * @brief Returns the full path without basename if a relative path is
         * given
         * 
         * @return @return std::string path
         */
        std::string get_path_full ();
        
        /**
         * @brief Returns the full path without basename (if a relative path is
         * given)
         * 
         * @param  std::string f (filepath)
         * @return 
         */
        static std::string get_path_full (const std::string &f);
        
        /**
         * @brief Returns the filename without extension
         * 
         * @return 
         */
        std::string get_filename ();
        
        /**
         * @brief Returns the filename
         * 
         * @param  std::string f (filepath)
         * @return 
         */
        static std::string get_filename (const std::string &f);
        
        /**
         * @brief Returns the filename without extension
         * 
         * @return 
         */
        std::string get_filename_without_extension ();
        
        /**
         * @brief Returns the filename without extension
         * 
         * @param  std::string f (filepath)
         * @return 
         */
        static std::string get_filename_without_extension (const std::string &f);
        
        /**
         * @brief Returns the filename with extension
         * 
         * @return 
         */        
        std::string get_basename ();
        
        /**
         * @brief Returns the filename with extension
         * 
         * @param  std::string f (filepath)
         * @return 
         */        
        static std::string get_basename (const std::string &f);

        /**
         * @brief Returns the full filepath (if a relative path is given)
         * 
         * @return 
         */                
        std::string get_filepath_full ();
        
        /**
         * @brief Returns the full filepath (if a relative path is given)
         * 
         * @param  std::string f (filepath)
         * @return 
         */                        
        static std::string get_filepath_full (const std::string &f);
        
        /**
         * @brief Return the string passed to the Constructor
         * 
         * @return std::string
         */
        std::string get_filepath ();

    /* size */
        /**
         * @brief Filesize in bytes
         * @return unsigned int
         */    
        unsigned int get_size_bytes ();
        
        /**
         * @brief Filesize in bytes
         * @param std::string f filepath
         * @return unsigned int
         */            
        static unsigned int get_size_bytes (const std::string &f);
        
        /**
         * @brief Filesize in Kilo-Bytes
         * @return double
         */
        
        double get_size_kb ();
        /**
         * @brief Filesize in Kilo-Bytes
         * @param std::string f filepath
         * @return double
         */            
        static double get_size_kb (const std::string &f);
        
        /**
         * @brief Filesize in Mega-Bytes
         * @return double
         */                    
        double get_size_mb ();
        
        /**
         * @brief Filesize in Mega-Bytes
         * @param std::string f filepath
         * @return double
         */                    
        static double get_size_mb (const std::string &f);
        
      /* content */
        
        /**
         * @brief Get a list with the directory content
         * @param path
         * @return std::vector<std::string>
         */
        //TODO
        //static std::vector<std::string> get_dir_content(const std::string& path);
        
        /**
         * @brief Get a list with the directory content
         * @param std::string path
         * @return std::vector<std::string>
         */
        static std::vector<std::string>
          get_dir_content(const std::string& path);
        
        /**
         * @brief Get a list with the directory content filtered by file-extension
         * @param std::string path
         * @param std::string extension
         * @return std::vector<std::string>
         */
        static std::vector<std::string>
          get_dir_content(const std::string& path, const std::string& ext);
        
        /**
         * @brief Get a list with the directory content filtered by a list
         * of file-extensions
         * @param std::string path
         * @param std::vector<std::string> extensions
         * @return std::vector<std::string>
         */
        static std::vector<std::string>
          get_dir_content(const std::string& path, 
                          const std::vector<std::string>& extensions);
        
                
        /**
         * @brief Get a list with the directory content recursively
         * @param path
         * @return std::vector<std::string>
         */
        static std::vector<std::string>
          get_dir_content_recursive(const std::string& path);
        
        /**
         * @brief Get a list with the directory content recursively
         * @param path
         * @return std::vector<std::string>
         */
        static std::vector<std::string>
          get_dir_content_recursive(const std::string& path,
                                    const std::string& ext);
        
        /**
         * @brief Get a list with the directory content filtered by a list
         * of file-extensions
         * @param path
         * @return std::vector<std::string>
         */
        static std::vector<std::string>
          get_dir_content_recursive(const std::string& path, 
                          const std::vector<std::string>& extensions);

        
    private:
    /* helper */
        /**
         * @brief Remove leading "./" from filepath
         * @param std::string f (filepath)
         */
        static void remove_dotslash (std::string &f);
    };

} // namespace::gats

#endif /* GATS_FILE_H */

