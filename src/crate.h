#ifndef CRATE_H
#define CRATE_H

#include "db_crates.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "libquasd/tagtool.h"
#include "src/global.h"

namespace xwaxdb
{
    
class crate_audiofiles;
class crate_gmbrc;
class crate_pls;

class crate
{
  std::shared_ptr<db_crates> ptr_xdb;

  quasd::tagtool tag_parser;
  std::vector<std::string> v_common_tags{"artist", "album", "title", "year",
                                         "rating", "publisher", "bpm"};
public:
  crate();

  friend class crate_audiofiles;
  friend class crate_gmbrc;
  friend class crate_pls;

  /**
   * @brief Set the crateDB xdb
   * @param db
   */
  void set_xdb(std::shared_ptr<db_crates> db) { ptr_xdb = db; };
        
private:
  /**
   * @brief Get s_crate from db or a new generated crate
   * @param type
   * @param name_orig
   * @return 
   */
  xwaxdb::s_crate handle_crate(const std::string& type, const std::string& name_orig);
  
  /**
   * @brief Handle Audiofile List
   *    
   * Iterates v_af:
   * If the file is in db, the af-id is written crate.v_afid.
   * If the file is not new, the audiofile tags are parsed and added to the db
   * and then new received af-id is written to crate.v_afid & crate.v_new_afid
   * 
   * Returns the number of files that are new in xwax.db
   * 
   * @param v_af
   * @param crate
   * @return new files added to db
   */
  int handle_aflist(const std::vector<std::string>& v_af, xwaxdb::s_crate& crate);
};

} // namespace xwaxdb
 
#endif // CRATE_H
