#ifndef CRATE_GMBRC_H
#define CRATE_GMBRC_H

#include "crate.h"
#include "libquasd/gmbrctool.h"

namespace xwaxdb
{
    
class crate_gmbrc : public crate
{
  const std::string str_filename_pls_selection = "gmbrc_preselected_pls.lst";

  // gmbrc-specific
  std::string str_path_gmbrc;
  quasd::gmbrctool gt;
  std::vector<std::string> v_pls_selected; // selected playlists previous session
  std::vector<std::string> v_pls_all;      // all gmbrc-playlists
  
public:
  
  crate_gmbrc();

  bool scan_selected_pls(const std::vector<std::string>& v_pls);
  bool init_gmbrc();

  /**
   * @brief Read the file containing the via gui selected gmbrc-playlists
   * from the last session and save it to v_pls_selected.
   * file = $APPDIR/gmbrc_preselected_pls.lst
   */
  void read_pls_selected();

  /**
   * @brief Set str_path_gmbrc to standard: $HOME/.config/gmusicbrowser/gmbrc
   */
  void use_gmbrc_stdpath();

  /**
   * @brief Set str_path_gmbrc to custom path
   */
  void set_gmbrc_path(const std::string& p) { str_path_gmbrc = p; };
  std::vector<std::string> get_pls_all() { return v_pls_all; };
  std::vector<std::string> get_pls_selected() { return v_pls_selected; };

};
 
} // namespace xwaxdb
 
#endif // CRATE_GMBRC_H
