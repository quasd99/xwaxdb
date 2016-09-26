#ifndef SETTINGSFILE_H
#define SETTINGSFILE_H

#include <map>
#include <string>
#include <set>
#include <vector>

namespace quasd
{
 
class settingsfile
{
    std::string str_path;
    
    // key = section, value = pair(key, value)
    std::multimap<std::string, std::pair<std::string, std::string> > map_settings;
    
public:
    settingsfile(const std::string& p);
    
    std::set<std::string> get_sectionlist();
    std::map<std::string, std::string> get_section_data(const std::string& section);
    
    /**
     * @brief Use this method if you prefer to work with an settingsfile-instance
     * to use it's map & methods
     * and functions.
     * @return True if the file can be parsed
     */
    bool parse();
    
    /**
     * @brief Get the parsed settings-map.
     * @param const std::string& path (path to settings-file)
     * @return map key=section, pair: 1=key, 2=value
     */
    static std::multimap<std::string, std::pair<std::string, std::string> > 
      parse(const std::string& path);
    void set_map_settings(std::multimap<std::string,std::pair<std::string, std::string> >& m);
    
    bool write();
    static bool write(const std::string path,
      std::multimap<std::string,std::pair<std::string, std::string> >& m);
    
private:
    /**
     * @brief Extract and retuns the string between []
     * @param std::string
     * @return std::string result or empty string
     */
    static std::string parse_section_line(const std::string& s);
    
    /**
     * @brief Split string at first '='. Returns pair(key,value).
     * @param std::string
     * @return pair(key,value) or pair with empty strings
     */
    static std::pair<std::string, std::string> parse_key_val_line(const std::string& s);

    /**
     * @brief Used by the static & non-static method parse
     */    
    static bool parse(const std::string& path,
               std::multimap<std::string, std::pair<std::string, std::string>>& m);
};


} // namespace quasd
 
#endif // SETTINGSFILE_H
