#ifndef M3UTOOL_H
#define M3UTOOL_H

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

namespace quasd
{
//    struct s_m3u_trackdata
//    {
//        std::string artist;
//        std::string title;
//        std::string path;
//        std::string path_type; // absolute, relative, url
//        int length;
//    };
 
class m3utool
{
  // key=playlist-pos, map(key,value)
  // keylist: path (path/url), artist, title, length
  std::map<unsigned int, std::unordered_map<std::string, std::string> > map_data;
  
  bool b_make_abs_path;
  bool b_ignore_url;
  bool b_is_extm3u;
  std::string path_m3u;
public:
    m3utool();
    
    /**
     * @brief Parse m3ufile with EXTINF-info. Results can be accessed via
     * get_m3u_data(). Returns true if file could be opened and contains at least
     * one entry.
     * @param const std::string& path (to m3u-file)
     * @param make_abs_path (convert relative paths to absolute paths)
     * @param ignore_url (urls are not parsed, just files)
     * @return bool success
     */
    bool parse(const std::string& path,
        bool make_abs_path = false,
        bool ignore_url = false);
    /**
     * @brief Simple static function that returns all entries without the EXTINF
     * information
     * @param const std::string& path (to m3u-file)
     * @param make_abs_path (convert relative paths to absolute paths)
     * @param ignore_url (urls are not parsed, just files)
     * @return std::vector<std::string>
     */
    static std::vector<std::string> parse_simple(const std::string& path,
        bool make_abs_path,
        bool ignore_url);
    
    bool get_is_extm3u() { return b_is_extm3u; };
    
    std::map<unsigned int, std::unordered_map<std::string, std::string> >
    get_m3u_data() { return std::move(map_data); };
private:
  /**
   * @brief Returns true if line is a url
   * @param const std::string&line
   * @return bool
   */
  static bool is_url(const std::string& line);
  
  static std::string get_abs_path(const std::string& m3upath,
     const std::string& relpath);
  
  /**
   * @brief If b_make_absolute is true and param1 is a relative path the
   * absolute path is returned.
   * If b_ignore_url is true and param1 is a url a empty string is returned
   * @param path
   * @return std::string
   */
  std::string get_path(const std::string& path);
  
  /**
   * @brief Extract info from param line and write it to param t
   * @param std::string line
   * @param struct s_m3u_trackdata
   * @return bool success
   */
  std::unordered_map<std::string, std::string>
  parse_extinf_line(const std::string& line);
};

} // namespace quasd
 
#endif // M3UTOOL_H
