#ifndef UIMAINWINDOW_H
#define UIMAINWINDOW_H

#include <gtkmm-3.0/gtkmm.h>

#include "src/db_crates.h"
#include "src/crate_audiofiles.h"
#include "src/crate_gmbrc.h"
#include "src/crate_pls.h"

#include "src/uigmbrc.h"
#include "src/uicrates.h"
#include "src/settings.h"
#include "src/uisettingsdialog.h"

class UiMainWindow : public Gtk::Window
{
   // data, settings, etc
    quasd::settings settings;
    std::shared_ptr<xwaxdb::db_crates> ptr_xdb;
    xwaxdb::crate_audiofiles c_af;
    xwaxdb::crate_gmbrc c_gmbrc;
    xwaxdb::crate_pls c_pls;
    
   // ui 
    
    // main-container
    Gtk::Box vbox; // contains mainbar & hbox
    Gtk::Box hbox; // contains notebook & statusline
    Gtk::Toolbar mainbar;
    Gtk::Notebook notebook;

    // mainbar-items
    Gtk::ToolButton btn_settings;
    Gtk::ToolButton btn_export;
    Gtk::ToolButton btn_bpm_scan;
    
    // notebook-items
    UiGmbrc  ui_gmbrc;
    UiCrates ui_crates;
    
public:
    UiMainWindow();
    
    bool init_session();
    void init_gui();
    void init_gui_crates();
    void init_gui_gmbrc();
    //void init_gui_playlists();
    
private:
  std::string path_application;
protected:
    bool slot_app_quit(GdkEventAny* event);
    
    // ui-btn-slots
    void on_btn_bpm_scan();
    void on_btn_settings();
    void on_btn_export();
    
    void init_settings_ui( UiSettingsDialog *dlg );
    void process_settings_ui( UiSettingsDialog *dlg );
    void write_settings_file();
    
    // notebook
    void on_notebook_switch_page(Gtk::Widget* page, guint page_num);
};

#endif // UIMAINWINDOW_H
