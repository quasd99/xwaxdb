#ifndef UIPROGRESSDIALOG_H
#define UIPROGRESSDIALOG_H

#include <gtkmm-3.0/gtkmm.h>

class UiProgressDialog : public Gtk::Dialog
{
  Gtk::Box *vbox;
  Gtk::ProgressBar progressbar;
  Gtk::ScrolledWindow scroll;
  Gtk::TextView textview;
  
public:
  UiProgressDialog();

  void update_ui(double f, std::string msg);
  //void update_ui(double f);
protected:
  
private:
};

#endif // UIPROGRESSDIALOG_H
