#ifndef UISETTINGSDIALOG_H
#define UISETTINGSDIALOG_H

#include <gtkmm-3.0/gtkmm.h>

class UiSettingsDialog : public Gtk::Dialog
{
  
public:
  UiSettingsDialog(BaseObjectType* cobject, 
                   const Glib::RefPtr<Gtk::Builder>& refGlade);

  /**
   * @brief Init the dialog with current settings and exec run
   */
  bool exec();
  
  // glade ui reference-pointer
  Gtk::CheckButton* cbtn_gmbrc;
  Gtk::RadioButton* rbtn_std_gmbrc;
  Gtk::RadioButton* rbtn_custom_gmbrc;
  Gtk::Entry*       entry_custom_gmbrc;
  
  Gtk::CheckButton* cbtn_af;
  Gtk::CheckButton* cbtn_pls;
  Gtk::Entry*       entry_crates_path;
  
  Gtk::CheckButton* cbtn_bpm_scan;

protected:
  Glib::RefPtr<Gtk::Builder> builder;
  
private:
};

#endif // UISETTINGSDIALOG_H
