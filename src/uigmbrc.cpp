#include "uigmbrc.h"

#include <fstream>
#include <iostream>
#include <set>

UiGmbrc::UiGmbrc()
{
    init_gui();
}

//void
//UiGmbrc::init()
//{
//    // called by UiMainWindow to initialize
//}

void
UiGmbrc::init_gui()
{
	//lbl_test.set_text("Hello Buddy");

	// list-views
	lvt_all.set_column_title(0, "Gmbrc Playlists");
	scroll_all.add(lvt_all);
	lvt_all.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	lvt_selected.set_column_title(0, "Selected Gmbrc Playlists");
	lvt_selected.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	scroll_selected.add(lvt_selected);

	// buttons
	btn_add_all.set_label("A");
	btn_add_all.signal_clicked().connect(sigc::mem_fun(*this,
			&UiGmbrc::on_btn_add_all));
	btn_add_pls.set_label("<");
	btn_add_pls.signal_clicked().connect(sigc::mem_fun(*this,
			&UiGmbrc::on_btn_add_pls));
	btn_remove_pls.set_label(">");
	btn_remove_pls.signal_clicked().connect(sigc::mem_fun(*this,
			&UiGmbrc::on_btn_remove_pls));
	btn_remove_all.set_label("X");
	btn_remove_all.signal_clicked().connect(sigc::mem_fun(*this,
			&UiGmbrc::on_btn_remove_all));
	box_lpane_btns.set_orientation(Gtk::ORIENTATION_VERTICAL);
	box_lpane_btns.pack_start(spacer_btn_top, Gtk::PACK_EXPAND_WIDGET, 0);
	box_lpane_btns.pack_start(btn_add_all, Gtk::PACK_SHRINK, 0);
	box_lpane_btns.pack_start(btn_add_pls, Gtk::PACK_SHRINK, 0);
	box_lpane_btns.pack_start(btn_remove_pls, Gtk::PACK_SHRINK, 0);
	box_lpane_btns.pack_start(btn_remove_all, Gtk::PACK_SHRINK, 0);
	box_lpane_btns.pack_start(spacer_btn_bottom, Gtk::PACK_EXPAND_WIDGET, 0);

	// packing lpane
	box_lpane.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	box_lpane.pack_start(scroll_selected, Gtk::PACK_EXPAND_WIDGET, 0);
	box_lpane.pack_start(box_lpane_btns, Gtk::PACK_SHRINK, 0);
	box_lpane.pack_start(scroll_all, Gtk::PACK_EXPAND_WIDGET, 0);


	pane.pack1(box_lpane, TRUE, TRUE);
	//pane.pack2(scroll_all, TRUE, TRUE);

	set_orientation(Gtk::ORIENTATION_VERTICAL);
	pack_start (pane, Gtk::PACK_EXPAND_WIDGET, 0);
}

void
UiGmbrc::append_pls_to_lvt_all(const std::string& pls)
{
    guint row_number = lvt_all.append();
    lvt_all.set_text(row_number, 0, pls);
		b_sel_changed = true;
}

void
UiGmbrc::append_pls_to_lvt_selected(const std::string& pls)
{
    guint row_number = lvt_selected.append();
    lvt_selected.set_text(row_number, 0, pls);
		b_sel_changed = true;
}

std::vector<std::string>
UiGmbrc::get_pls_selected()
{
	std::vector<std::string> vRet;
	std::string strPlsTitle;
	for (const auto Row : lvt_selected.get_model()->children())
	{
		(*Row).get_value(0, strPlsTitle);
		vRet.push_back(strPlsTitle);
	}
	return std::move(vRet);
}


void
UiGmbrc::on_btn_add_all()
{
	std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;

	// copy all playlists from lvt_all to lvt_selected
	std::string strPlsTitle;
	for (const auto Row : lvt_all.get_model()->children())
	{
		(*Row).get_value(0, strPlsTitle);
		lvt_selected.append(strPlsTitle);
		set_pls_added.insert(strPlsTitle);
	}

	// clear lvt_all
	lvt_all.clear_items();

	b_sel_changed = true;
}

void
UiGmbrc::on_btn_add_pls()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
        
    std::string strPlsTitle;
    std::vector<Gtk::TreeModel::RowReference> Rows;
    auto refModel = lvt_all.get_model();
    auto refListStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(refModel);
    auto Selection = lvt_all.get_selection();
    
    for (auto Rowpath : Selection->get_selected_rows())
    {
			// get pls-title and add it to lvt_selected
			Gtk::TreeModel::iterator iter = refModel->get_iter(Rowpath);
			(*iter).get_value(0, strPlsTitle);
			lvt_selected.append(strPlsTitle);
			set_pls_added.insert(strPlsTitle);

			// save row reference
			Rows.emplace_back(Gtk::TreeModel::RowReference(refModel, Rowpath));
    }
    
    // remove the row-references from treemodel
    for (auto &Iter : Rows)
    {
			auto treeIter = refModel->get_iter(Iter.get_path());
			if ( treeIter )
			{
				refListStore->erase(treeIter);
			}   
    }
    
		b_sel_changed = true;
}

void
UiGmbrc::on_btn_remove_all()
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
    
    // copy all playlists from lvt_selected to lvr_all
    std::string strPlsTitle;
    for (const auto Row : lvt_selected.get_model()->children())
    {
        (*Row).get_value(0, strPlsTitle);
        lvt_all.append(strPlsTitle);
				set_pls_removed.insert(strPlsTitle);
    }
    
    // clear lvt_all
    lvt_selected.clear_items();
		
		b_sel_changed = true;
}

void
UiGmbrc::on_btn_remove_pls()
{
	std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;

	std::string strPlsTitle;
	std::vector<Gtk::TreeModel::RowReference> Rows;
	auto refModel = lvt_selected.get_model();
	auto refListStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(refModel);
	auto Selection = lvt_selected.get_selection();

	for (auto Rowpath : Selection->get_selected_rows())
	{
		// get pls-title and add it to lvt_selected
		Gtk::TreeModel::iterator iter = refModel->get_iter(Rowpath);
		(*iter).get_value(0, strPlsTitle);
		lvt_all.append(strPlsTitle);
		set_pls_removed.insert(strPlsTitle);

		// save row reference
		Rows.emplace_back(Gtk::TreeModel::RowReference(refModel, Rowpath));
	}

	// remove the row-references from treemodel
	for (auto &Iter : Rows)
	{
		auto treeiter = refModel->get_iter(Iter.get_path());
		if (treeiter)
		{
				refListStore->erase(treeiter);
		}   
	}

	b_sel_changed = true;
}

//void UiGmbrc::select_playlist(const std::string& pls)
//{
//	// remove pls-title from lvt_all
//	auto refModel = lvt_all.get_model();
//	auto refListStore = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(refModel);
//	std::string Val;
//
//	for (const auto it : refModel->children())
//	{
//		(*it).get_value(0, Val);
//		if ( Val == pls )
//		{
//			refListStore->erase(*it);
//			break;
//		}
//	}
//
//	// add pls-title to lvt_selected
//	append_pls_to_lvt_selected(pls);
//}
