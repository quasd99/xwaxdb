#include "uicrates.h"

#include <iostream>

UiCrates::UiCrates()
{
    init();
}

void
UiCrates::init()
{
	std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
	lvt_crate.set_rules_hint(true);
	lvt_crate.set_column_title(COL_CRATE::TYPE, "Type");
	lvt_crate.set_column_title(COL_CRATE::NAME, "Name");
	lvt_crate.set_column_title(COL_CRATE::XWAX_NAME, "xwax-Name");
	lvt_crate.set_column_title(COL_CRATE::FILES, "Files");
	lvt_crate.set_column_title(COL_CRATE::NEW_FILES, "New Files");
	lvt_crate.set_column_title(COL_CRATE::PATH, "Path");
	for (int i=0; i < 6; i++)
	{
		auto Column = lvt_crate.get_column(i);
		Column->set_clickable(true);
		Column->set_resizable(true);
	}
	
	//lvt_crate.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	scroll_crate.add(lvt_crate);
	
	lvt_crate_selection = lvt_crate.get_selection();
	lvt_crate_selection->signal_changed().connect(sigc::mem_fun(*this,
                                      &UiCrates::on_lvtcr_selection_changed));
	
	lvt_af.set_rules_hint(true);
	lvt_af.set_reorderable(true);
	lvt_af.set_show_expanders(true);
	// TODO: hidden column id
	lvt_af.set_column_title(COL_AF::ARTIST, "Artist");
	lvt_af.set_column_title(COL_AF::TITLE, "Title");
	lvt_af.set_column_title(COL_AF::ALBUM, "Album");
	lvt_af.set_column_title(COL_AF::YEAR, "Year");
	lvt_af.set_column_title(COL_AF::BPM, "BPM");
	lvt_af.set_column_title(COL_AF::PATH_AF, "Path");
	//lvt_af.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	for (int i=0; i < 6; i++)
	{
		auto Column = lvt_af.get_column(i);
		Column->set_clickable(true);
		Column->set_resizable(true);
	}
	scroll_af.add(lvt_af);
	
	pane.pack1(scroll_crate, TRUE, TRUE);
	pane.pack2(scroll_af, TRUE, TRUE);

	set_orientation(Gtk::ORIENTATION_VERTICAL);
	pack_start (pane, Gtk::PACK_EXPAND_WIDGET, 0);
}

void
UiCrates::append_crate(const xwaxdb::s_crate& cr)
{
	guint rowNum = lvt_crate.append();
	lvt_crate.set_text(rowNum, COL_CRATE::TYPE, cr.type);
	lvt_crate.set_text(rowNum, COL_CRATE::NAME, cr.name_orig);
	lvt_crate.set_text(rowNum, COL_CRATE::XWAX_NAME, cr.name_uniq);
	lvt_crate.set_text(rowNum, COL_CRATE::FILES, std::to_string(cr.v_afid.size()));
	lvt_crate.set_text(rowNum, COL_CRATE::NEW_FILES, std::to_string(cr.v_new_afid.size()));
	lvt_crate.set_text(rowNum, COL_CRATE::PATH, cr.path);
}

void
UiCrates::on_lvtcr_selection_changed()
{
	if ( lvt_crate_selection->count_selected_rows() == 0 )
	{
		return;
	}
	
	auto Selection = lvt_crate.get_selection();
	auto refModel = lvt_crate.get_model();
	std::string name_orig;
	std::string type;
	for (auto Rowpath : Selection->get_selected_rows())
	{
		auto Iter = refModel->get_iter(Rowpath);
		(*Iter).get_value(COL_CRATE::TYPE, type);
		(*Iter).get_value(COL_CRATE::NAME, name_orig);
	}
	
	xwaxdb::s_crate Crate;
	if ( ptr_xdb->getw_crate(type, name_orig, Crate))
	{
		lvt_af.clear_items();
		for (const auto Afid : Crate.v_afid)
		{
			std::unordered_map<std::string, std::string> mAfData = ptr_xdb->get_af_data(Afid);
			guint rowNum = lvt_af.append();
			lvt_af.set_text(rowNum, COL_AF::ARTIST, mAfData["artist"]);
			lvt_af.set_text(rowNum, COL_AF::TITLE, mAfData["title"]);
			lvt_af.set_text(rowNum, COL_AF::ALBUM, mAfData["album"]);
			lvt_af.set_text(rowNum, COL_AF::YEAR, mAfData["year"]);
			lvt_af.set_text(rowNum, COL_AF::BPM, mAfData["bpm"]);
			lvt_af.set_text(rowNum, COL_AF::PATH_AF, mAfData["filepath"]);
		}
	}
}
