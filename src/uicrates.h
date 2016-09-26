#ifndef UICRATES_H
#define UICRATES_H

#include <gtkmm-3.0/gtkmm.h>
#include "src/global.h"
#include "src/db_crates.h"

enum COL_CRATE {TYPE, NAME, XWAX_NAME, FILES, NEW_FILES, PATH};
enum COL_AF {ARTIST, TITLE, ALBUM, YEAR, BPM, PATH_AF};
 
class UiCrates : public Gtk::Box
{
  std::shared_ptr<xwaxdb::db_crates> ptr_xdb;
  
  Gtk::HPaned pane;
  Gtk::Box box_lpane;
  Gtk::ListViewText lvt_crate {6};
  Glib::RefPtr<Gtk::TreeSelection> lvt_crate_selection;
  Gtk::ListViewText lvt_af {6};
  Gtk::ScrolledWindow scroll_crate;
  Gtk::ScrolledWindow scroll_af;
    
public:
  UiCrates();

  void init();
  void append_crate(const xwaxdb::s_crate& cr);
  void set_xdb(std::shared_ptr<xwaxdb::db_crates> db) { ptr_xdb = db; };

protected:
  void on_lvtcr_selection_changed();
};

#endif // UICRATES_H
