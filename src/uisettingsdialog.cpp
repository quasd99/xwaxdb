#include "uisettingsdialog.h"

UiSettingsDialog::UiSettingsDialog(BaseObjectType* cobject,
                                   const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Dialog(cobject),
  builder(refGlade)
{
	set_title("xwaxdb settings");
	set_size_request(600,300);
	
	builder->get_widget("cbtn_gmbrc", cbtn_gmbrc);
	builder->get_widget("rbtn_std_gmbrc", rbtn_std_gmbrc);
	builder->get_widget("rbtn_custom_gmbrc", rbtn_custom_gmbrc);
	builder->get_widget("rbtn_custom_gmbrc", rbtn_custom_gmbrc);
	builder->get_widget("entry_custom_gmbrc", entry_custom_gmbrc);
	builder->get_widget("cbtn_af", cbtn_af);
	builder->get_widget("cbtn_pls", cbtn_pls);
	builder->get_widget("entry_crates_path", entry_crates_path);
	builder->get_widget("cbtn_bpm_scan", cbtn_bpm_scan);
	
	add_button(Gtk::Stock::APPLY, 1);
	add_button(Gtk::Stock::CANCEL, 0);
}

bool
UiSettingsDialog::exec()
{
	int status = run();
	switch (status)
	{
		case 0:
			// cancel
			break;
		case 1:
			// TODO: Validation
      return true;
			break;
	}
  
  return false;
}


