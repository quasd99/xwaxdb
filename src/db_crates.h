#ifndef DB_CRATES_H
#define DB_CRATES_H

#include "libquasd/settingsfile.h"
#include "libquasd/tagtool.h"
#include "src/global.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <gtkmm-3.0/gtkmm.h>

namespace xwaxdb
{
 
class db_crates
{
  std::string str_path_application;
  std::string str_path_crate; // directory to scan for audiofiles & playlists
  const std::string str_dbfile = "xwax.db";

  std::unordered_set<std::string> set_unique_filepaths;
  std::vector<std::unordered_map<std::string, std::string>> v_tracks;
  //std::vector<std::unordered_map<std::string, std::string>> v_new_tracks;
  
  std::vector<xwaxdb::s_crate> v_crates; // all crates previous session
  //std::vector<xwaxdb::s_crate> v_crates_new; // crates added during a session
  
  std::unordered_map<std::string, unsigned int> map_af_id; // af-path, id
  std::unordered_map<unsigned int,
    std::unordered_map<std::string, std::string> >map_id_afdata; // id, af-data
  
  std::unordered_set<unsigned int> set_no_bpm;
  std::unordered_set<unsigned int> set_af_notfound;
  
  quasd::tagtool tag_parser;
  std::vector<std::string> v_common_tags{"artist", "album", "title", "year",
                                         "rating", "publisher", "bpm"};

  unsigned int id_af = 0; // actual, highest audiofile-id
  bool b_append_cratetype = false; // add cratetype to name_uniq
  bool b_check_bpm = false;        // scan bpm during crate-scan
  bool b_abort_bpmscan = false;
public:
  db_crates();
  
  
  void abort_bpmscan() { b_abort_bpmscan = true; };
  /**
   * @brief Add an audiofile-path to the db. The tags of the audiofile will be
   * parsed. Returns the new id for the audiofile.
   * 
   * To avoid error check first if af is already in db with:
   * get_af_id(af) -> returns 0 if af is not in db
   * 
   * @param af audiofile path
   * @return id of the audiofile in the xwax.db or 0 (error = af already in db
   * or af does not exist)
   */
  unsigned int append_af(const std::string& af);
  
  /**
   * @brief Add an audiofile and it's tagmap to db.
   * 
   * To avoid error check first if af is already in db with:
   * get_af_id(af) -> returns 0 if af is not in db
   * 
   * @param af audiofile path
   * @param dat represenation of the audiofile's tags
   * @return id of the audiofile in the xwax.db or 0 (error = af already in db
   * or af does not exist)
   */  
  unsigned int append_afdata(const std::string& af, 
                             std::unordered_map<std::string, std::string>& dat);
  
  void append_crate(xwaxdb::s_crate& cr) { v_crates.push_back(cr); };
  /**
   * @brief Removes an audiofile + data from db. Returns the id the file had.
   * @param const std::string& af
   * @return unsigned int id
   */
  unsigned int remove_af(const std::string& af);
  /**
   * @brief Removes an audiofile + data from db. Returns the id the af got.
   * @param id
   */
  void remove_af(unsigned int id);
  
  unsigned int get_af_id(const std::string& af);
  
  //std::unordered_map<std::string, std::string> parse_af_tags();
  std::unordered_map<std::string, std::string> get_af_data(unsigned int);
  /**
   * @brief If the crate is found by param 1 & 2 write result to param 3 and
   * return true. If not just return false
   * @param type
   * @param name_orig
   * @param crate
   * @return bool
   */
  bool getw_crate(const std::string& type,
                  const std::string& name_orig,
                  xwaxdb::s_crate& cr); // <- write
  
  std::vector<xwaxdb::s_crate> get_crates();
  
  /**
   * @brief Get a unique cratename by name & cratetype. Add the crate
   * to v_crates_now. All whitespaces will be replaced with underscores.
   * Is the name already used, the crate-type will be added to name:
   * e.g.
   * 1) add Techno Selection via gmbrc  -> Techno_Selection
   * 2) Techno Selection via audiofiles -> Techno_Selection[audiofiles]
   * 
   * The unique cratename is used as -l parameter for xwax and is also
   * the filename for the .xwaxdb-files: Techno_Selection(audiofiles).xwaxdb
   * 
   * See also: write_unique_cratenames(), read_unique_cratenames()
   * 
   * @param name
   * @param type
   * @return unique cratename
   */
  std::string get_unique_cratename(const std::string& name, const std::string& type);
  
  int get_no_bpm_count() { return set_no_bpm.size(); };
  int get_missing_af_count() { return set_af_notfound.size(); };
  
  bool is_new_crate(const std::string& name, const std::string& type);
    
  void set_path_application(const std::string& p) { str_path_application = p; };
  std::string get_path_application() { return str_path_application; };
  
  void set_path_crate(const std::string& p) { str_path_crate = p; };
  std::string get_path_crate() { return str_path_crate; };
  
  int get_total_af();
  bool is_in_db(const std::string& af);
  
  /**
   * @brief Check if bpm-tag is properly set. If not add af_id to set_no_bpm
   * @param bpm
   */
  void check_bpm(std::string& bpm, const unsigned int id_af);
  
  /**
   * @brief Read the xwax.db-file, write the audiofile-information to v_tracks 
   * and save all audiofilepaths in unordered_set set_unique_filepaths.
   * Returns true if file could be parsed.
   * @return bool
   */
  bool read_xwax_db();

  /**
   * @brief At the beginnig of a session parse the content of
   * file: %HOME%/.config/quasd/xwaxdb/unique_cratenames.db
   * and write it to umap_cratenames
   */
  void read_crates_db();
  
  /**
   * @brief Sytem-Execute `bpm-tag -f %FILE%` for all files in set_no_bpm
   */
  void scan_bpm();

  /**
   * @brief Use true to append the crate-type to the xwax-cratename
   * @param b
   */
  void set_append_cratetype(bool b) { b_append_cratetype = b; };
  
  void set_check_bpm(bool b) { b_check_bpm = b; };
  
  sigc::signal<void, double, std::string> signal_new_bpmscan;
  //sigc::signal<void, double> signal_new_bpmscan;
  
  /**
   * @brief Write all audiofile-informations in v_tracks to xwax.db-file.
   * Returns true if data could be written.
   * @return bool
   */
  bool write_xwax_db();

  /**
   * @brief At the end of a session write the content of v_crates to file
   * crates.db
   */
  void write_crates_db();
  
  /**
   * @brief Export all crates (v_crates) to .xwaxdb-files
   */
  void write_dbfiles();
  
  /**
   * @brief write the xwax_lparams.lst-file to use with the xwax-launch-script
   */
  void write_lparams_file();
  
private:
  void parse_xwaxdb_line(const std::string& line);
  void parse_cratedb_line(const std::string& line);
  std::vector<unsigned int> parse_cratedb_afid(const std::string& tok);
  std::string parse_bpmtag_resp(const std::string& resp);
  
  /**
   * @brief Returns true if name is not already used (umap_cratename)
   * @param name
   * @return bool
   */
  bool is_unique_cratename(const std::string& name);
  
};

} // namespace xwaxdb
 
#endif // DB_CRATES_H
