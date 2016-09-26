#include "uiprogressdialog.h"

UiProgressDialog::UiProgressDialog()
{
	add_button(Gtk::Stock::CANCEL, 0);
	progressbar.set_show_text(true);
	progressbar.set_text("Scanning Files");
	
	textview.set_editable(false);
	textview.set_cursor_visible(false);
	
	scroll.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scroll.add(textview);
	
	vbox = get_content_area();
	vbox->pack_start(progressbar, Gtk::PACK_SHRINK, 0);
	vbox->pack_start(scroll, Gtk::PACK_EXPAND_WIDGET, 0);
	
	show_all();
}

void
UiProgressDialog::update_ui(double f, std::string msg)
{
	progressbar.set_fraction(f);
	
	auto Buffer = textview.get_buffer();
	auto Iter = Buffer->get_iter_at_line(0);
	Buffer->insert(Iter, msg);
}

