#include "uimainwindow.h"
#include "uigmbrc.h"
#include "libquasd/file.h"
#include "uiprogressdialog.h"
#include "uisettingsdialog.h"

#include <iostream>
#include <thread>

UiMainWindow::UiMainWindow()
{
	init_session();	
	init_gui();

	if ( settings.get_section_value("general",  "crate_gmbrc") == "on" )
	{
		std::cout << "call:init_gui_gmbrc()" << std::endl;
		init_gui_gmbrc();
	}

	init_gui_crates();
	
	std::cout << "###########################################" << std::endl;
	std::cout << "Total files: " << ptr_xdb->get_total_af() << std::endl;
	std::cout << "No BPM     : " << ptr_xdb->get_no_bpm_count() << std::endl;
	std::cout << "###########################################" << std::endl;
	//std::cout << "Missing    : " << ptr_xdb->get_missing_af_count() << std::endl;

	// app quit
	signal_delete_event().connect(sigc::mem_fun(*this,
			&UiMainWindow::slot_app_quit));
}

void UiMainWindow::init_gui()
{
    // window settings
    set_title("xwax db");
    set_default_size(800, 600);
    
    // toolbar
    mainbar.set_toolbar_style(Gtk::ToolbarStyle::TOOLBAR_BOTH);
    btn_settings.set_stock_id(Gtk::Stock::PREFERENCES);
    btn_settings.set_label("Settings");
    mainbar.append(btn_settings, sigc::mem_fun(*this, &UiMainWindow::on_btn_settings));
    btn_export.set_stock_id(Gtk::Stock::PROPERTIES);
    btn_export.set_label("Export");
    mainbar.append(btn_export, sigc::mem_fun(*this, &UiMainWindow::on_btn_export));
    btn_bpm_scan.set_stock_id(Gtk::Stock::EXECUTE);
    btn_bpm_scan.set_label("BPM Scan");
    mainbar.append(btn_bpm_scan, sigc::mem_fun(*this, &UiMainWindow::on_btn_bpm_scan));

		// notebook
		notebook.append_page(ui_crates, "Crates");
    if (settings.get_section_value("general",  "crate_gmbrc") == "on")
    {
			notebook.append_page(ui_gmbrc, "Gmbrc Playlists");
    }
    notebook.signal_switch_page().connect(sigc::mem_fun(*this,
              &UiMainWindow::on_notebook_switch_page));
    
    //main vertical box container
    vbox.set_orientation(Gtk::ORIENTATION_VERTICAL);
    vbox.pack_start (mainbar, Gtk::PACK_SHRINK, 0);
    vbox.pack_start(notebook,  Gtk::PACK_EXPAND_WIDGET, 0);
    
    // add & show
    add(vbox);
    show_all();
}

bool
UiMainWindow::init_session()
{
	std::cout << "Info:" << __PRETTY_FUNCTION__ << std::endl;
	
	std::string strPathApp = 
		quasd::settings::get_path_userhome() + '/' + ".config/quasd/xwaxdb";
	settings.set_path_application(strPathApp);
	settings.set_filename_settings("xwaxdb.settings");
	if ( !settings.parse_file_settings() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__ 
			<< ":cannot parse:"
			<< settings.get_filepath_settings() 
			<< std::endl;
		return false;
	}

	ptr_xdb = std::shared_ptr<xwaxdb::db_crates>(new xwaxdb::db_crates);
	ptr_xdb->set_path_application(strPathApp);	
	ptr_xdb->set_check_bpm(true);
	if ( !ptr_xdb->read_xwax_db() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__ 
			<< ":cannot read:"
			<< strPathApp << "/xwax.db"
			<< std::endl;
		return false;
	}
	ptr_xdb->read_crates_db();

	if ( settings.get_section_value("general", "crate_audiofiles") == "on"
			 || settings.get_section_value("general", "crate_pls") != "on" )
	{
		std::string pathCrate = 
			settings.get_section_value("general", "path_crate");	

		if ( pathCrate.empty() )
		{
			std::cerr << "ERR:" << __PRETTY_FUNCTION__
				<< ":path_crate not set"
				<< std::endl;
			return false;
		}
		
		ptr_xdb->set_path_crate(pathCrate);
	}
	
	if ( settings.get_section_value("general", "crate_audiofiles") == "on" )
	{
		c_af.set_xdb(ptr_xdb);
		c_af.scan();
	}	
	
	if ( settings.get_section_value("general", "crate_pls") == "on" )
	{
		c_pls.set_xdb(ptr_xdb);
		c_pls.scan();
	}	
		
	return true;
}
	


void
UiMainWindow::init_gui_gmbrc()
{
	c_gmbrc.set_xdb(ptr_xdb);
	c_gmbrc.use_gmbrc_stdpath();
	if ( !c_gmbrc.init_gmbrc() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":cannot parse gmbrc:"
			<< std::endl;
	}
	c_gmbrc.read_pls_selected();
	c_gmbrc.scan_selected_pls(c_gmbrc.get_pls_selected());

	// add the playlist-titles to gui-listviews
	std::vector<std::string> vSelected = c_gmbrc.get_pls_selected();
	std::cout << "c_gmbrc.get_pls_all().size():" << c_gmbrc.get_pls_all().size() << std::endl;
	for (const auto Pls : c_gmbrc.get_pls_all())
	{
		if ( std::find(vSelected.begin(), vSelected.end(), Pls) != vSelected.end() )
		{
			ui_gmbrc.append_pls_to_lvt_selected(Pls);
		}
		else
		{
			ui_gmbrc.append_pls_to_lvt_all(Pls);
		}
	}
}

void
UiMainWindow::init_gui_crates()
{   
	ui_crates.set_xdb(ptr_xdb);
	for (const auto Crate : ptr_xdb->get_crates())
	{
		ui_crates.append_crate(Crate);
	}
}

bool
UiMainWindow::slot_app_quit(GdkEventAny* event)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__
            << ":msg:quit"
            << std::endl;
        
//    session.write_gmbrc_preselected_pls(ui_gmbrc.get_pls_selected());
//    
//    if ( session.get_value_by_key("crate_gmbrc") == "on" )
//    {
//        // export all selected gmbrc-playlists
//        session.export_gmbrc_dbfiles(ui_gmbrc.get_pls_selected());
//    }
//    
//    if ( session.get_value_by_key("crate_pls") == "on" )
//    {
//        // export all playlist-files
//        session.export_pls_dbfiles();
//    }
//    
//    session.save_session_data();
    
    return false;
}

void
UiMainWindow::on_btn_settings()
{
	auto builder = Gtk::Builder::create();
	try
	{
			builder->add_from_file("src/uisettingsdialog.ui");
	}
	catch(const Glib::FileError& ex)
	{
			std::cerr << "  FileError: " << ex.what() << std::endl;
			return;
	}
	catch(const Glib::MarkupError& ex)
	{
			std::cerr << "  MarkupError: " << ex.what() << std::endl;
			return;
	}
	catch(const Gtk::BuilderError& ex)
	{
			std::cerr << "  BuilderError: " << ex.what() << std::endl;
			return;
	}

	UiSettingsDialog *dlg = nullptr;
	builder->get_widget_derived("dlg_settings", dlg);
	if ( dlg )
	{
		dlg->exec();
	}
	else
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":cannot exec settings dialog"
			<< std::endl;
	}
    
	delete dlg;
}

void
UiMainWindow::on_btn_export()
{
	ptr_xdb->write_xwax_db();
	ptr_xdb->write_crates_db();
	ptr_xdb->write_dbfiles();
	ptr_xdb->write_lparams_file();
	
	std::cout << "Info:" << __PRETTY_FUNCTION__ 
		<< "export done."
		<< std::endl;
}

void
UiMainWindow::on_btn_bpm_scan()
{
	btn_bpm_scan.set_sensitive(false);
	auto ScanThead = new std::thread(
		[this]
		{
			ptr_xdb->scan_bpm();
		}
	);
	
	UiProgressDialog dlg;
	dlg.set_title("BPM Scan");
	dlg.set_size_request(800,600);
	ptr_xdb->signal_new_bpmscan.connect(sigc::mem_fun(&dlg, &UiProgressDialog::update_ui));
	ScanThead->detach();
	dlg.run();
	ptr_xdb->abort_bpmscan();	
	btn_bpm_scan.set_sensitive(true);
}


void
UiMainWindow::on_notebook_switch_page(Gtk::Widget* page, guint page_num)
{
    std::cout << "Info:" << __PRETTY_FUNCTION__ 
              << "page:" << page_num
              << std::endl;
}



