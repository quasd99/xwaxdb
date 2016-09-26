#ifndef UIGMBRC_H
#define UIGMBRC_H

#include <gtkmm-3.0/gtkmm.h>
#include <set>
#include <string>

class UiGmbrc : public Gtk::Box
{
  // ui
   //Gtk::Label lbl_test;

   Gtk::HPaned pane;

   Gtk::Box box_lpane;
   Gtk::ListViewText lvt_selected {1};
   Gtk::ScrolledWindow scroll_selected;

   Gtk::ScrolledWindow scroll_all;
   Gtk::ListViewText lvt_all {1};

   Gtk::Box box_lpane_btns;
   Gtk::Button btn_add_all;
   Gtk::Button btn_add_pls;
   Gtk::Button btn_remove_all;
   Gtk::Button btn_remove_pls;
   Gtk::Alignment spacer_btn_top;
   Gtk::Alignment spacer_btn_bottom;

  // data
   bool b_sel_changed = false;
   std::set<std::string> set_pls_removed;
   std::set<std::string> set_pls_added;
    
public:
  UiGmbrc();

  void init_gui();

  void append_pls_to_lvt_all(const std::string& pls);
  void append_pls_to_lvt_selected(const std::string& pls);

  std::vector<std::string> get_pls_selected();
  bool get_sel_changed() { return b_sel_changed; };
  std::set<std::string> get_pls_removed() { return set_pls_removed; };
  std::set<std::string> get_pls_added() { return set_pls_added; };
    
private:
    
protected:
  /* slots */
  void on_btn_add_all();
  void on_btn_add_pls();
  void on_btn_remove_all();
  void on_btn_remove_pls();
};

#endif // UIGMBRC_H
