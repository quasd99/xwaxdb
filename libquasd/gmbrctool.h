#ifndef GMBRCTOOL_H
#define GMBRCTOOL_H

#include <map>
#include <regex>
#include <string>
#include <set>
#include <vector>
#include <unordered_map>

namespace quasd
{
 
class gmbrctool
{
  std::string str_gmbrc_path;
  
  bool b_all_pls = true;    // parse all playlists?
  bool b_all_tracks = true; // parse all tracks?
  
  std::set<std::string> set_pls_selection;  // user-selected pls to export
  
  // key = playlistname, values: playlist-id's
  std::map<std::string, std::vector<unsigned int> > map_pls_data;
  // all trackid's found using a user-filter
  std::set<unsigned int> set_unique_trackids;
  // key: track-id, val: map (key: tag-frame, val: value)
  std::unordered_map<unsigned int, 
    std::unordered_map<std::string, std::string> > map_track_data;
  // key: track-id, track-path
  std::unordered_map<unsigned int, std::string > map_track;
public:
  gmbrctool();
  
  /**
   * @brief Parse all gmbrc-playlists:
   * Write results to map_pls_data and map_track_data
   */
  bool parse_playlists();
  
  
  /**
   * @brief Parse the gmbrc-playlists that are in set param1:
   * Write results to map_pls_data and map_track_data
   * @param set_pls (playlist-names to be parsed)
   */
  bool parse_playlists(std::set<std::string> set_pls);
    
  /**
   * @brief After parse_playlists() or parse_playlists(std::set<std::string> set_pls)
   * is used, this method returns the names of all parsed playlists.
   * @return std::set<std::string>
   */
  std::vector<std::string> get_pls_all();

/**
 * @brief Get a list with a map of the playlist's audiofile-data
 * @param const std::string& name (playlist-name) 
 * @return std::vector<std::unordered_map<std::string, std::string>>
 */  
  std::vector<std::unordered_map<std::string, std::string>>
    get_pls_afdata(const std::string& name);
  
/**
 * @brief Get a list with a map of the playlist's audiofile-paths
 * @param const std::string& name (playlist-name) 
 * @return std::vector<std::string> (audiofile-paths)
 */  
  std::vector<std::string>
    get_pls_af(const std::string& name);
    
  /**
   * @brief static method for cli-interface, prints gmbrc-path of user
   */
  static void print_gmbrc_stdpath();
  /**
   * @brief static method for cli-interface, returns gmbrc-path of user
   */
  static std::string get_gmbrc_stdpath();
  /**
   * @brief static method for cli-interface, print all gmbrc playlist to cout
   */
  static void print_gmbrc_playlists();
  
  void use_gmbrc_stdpath();
private:
  const std::regex regex_state2{"SavedOn:(.*)"}; // end of playlist-section
  
  /**
   * @brief static method for cli-interface, parse gmbrc-file and returns a
   * vector with all gmbrc-playlistnames
   */  
  static std::vector<std::string> get_playlistnames();
  /**
   * @brief static method for cli-interface. Used by get_playlistnames to
   * parse the playlistname from the given line of the gmbrc-file
   */
  static std::string parse_playlistname(const std::string& line);

  /**
   * @brief Extract the playlist-title, create a vector with track-id's. Save it to map_pls_data
   */
  void parse_playlist_line(const std::string& line);

  /**
   * @brief Read a track-line from gmbrc
   * @param std::string l
   */
  void parse_track_line(const std::string& line);

  /**
   * @brief Returns true if the track-id is in set_unique_trackids;
   * @param unsigend int id
   * @return bool
   */
  bool is_in_unique_trackids(const std::string& line);
  
  std::unordered_map<std::string, std::string>
    get_trackdata_by_id(const unsigned int& id);
};

} // namespace quasd
 
#endif // GMBRCTOOL_H
