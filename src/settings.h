#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <string>
#include <set>
#include <vector>

namespace quasd
{
 
class settings
{
    std::string path_userhome;
    std::string path_application;
    std::string filename_settings;
    std::string path_tmp;
    
    // container for all settings
    // key = section, value = pair(key, value)
    std::multimap<std::string,
                  std::pair<std::string, std::string> > map_settings;
    
    // container for all sections, filled by 'parse_file_settings'
    std::set<std::string> set_sections;
    
public:
    settings();
    /**
     * 
     * @param const std::string& application path
     * @param const std::string& settings-filename
     */
    settings(const std::string& p, const std::string& f);
    
  // getter/setter
    /**
     * @brief
     * Use Glib::get_home_dir() to return user's home directory.
     * @return std::string home directory
     */
    static std::string get_path_userhome();
    std::string get_path_application() { return path_application; };
    void set_path_application(const std::string& p) { path_application = p; };
    std::string get_filename_settings() { return filename_settings; };
    void set_filename_settings(const std::string& f) { filename_settings = f; };
    
    /**
     * @brief Returns joined strings str_path_application & str_filename_settings.
     * @return std::string filepath settings
     */
    std::string get_filepath_settings();
    
    /**
     * @brief If settings-file got section, use this to get value
     * @param std::string section
     * @param std::string key
     * @return std::string value
     */
    std::string get_section_value(const std::string& section,
                                         const std::string& key);
    
    /**
     * @brief If a simple settings-file is used without sections, use this
     * to get a value.
     * @param std::string key
     * @return std::string value;
     */
    std::string get_value(const std::string& key);
    
    /**
     * @brief Returns a list with all section-names
     * @return std::vector<std::string>
     */
    std::set<std::string> get_sections();
    
     /**
      * @brief Returns a list with the key-value-pairs of specified section.
      * Use it to iterate the complete section.
      * @param std::string section
      * @return std::vector<std::pair<std::string, std::string>>
      */
    std::vector<std::pair<std::string, std::string>>
            get_section_data(const std::string& section);
        
  // user-interface  
    /**
     * @brief 
     * Parse the settings-file. Returns true if file can be opened &
     * can be parsed without error. 
     * 
     * Uses str_path_application + str_filename_settings as path.
     * 
     * @return bool success
     * 
     */
    bool parse_file_settings();
    
    
private:
    /**
     * @brief Extract and retuns the string between [....]
     * @param std::string
     * @return std::string result or empty string
     */
    //std::string parse_section_line(const std::string& s);
    
    /**
     * @brief Split string at first '='. Returns pair(key,value).
     * @param std::string
     * @return pair(key,value) or pair with empty strings
     */
    //std::pair<std::string, std::string> parse_key_val_line(const std::string& s);
};

} // namespace quasd
 
#endif // SETTINGS_H
