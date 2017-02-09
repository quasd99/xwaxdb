#include "uimainwindow.h"
#include "uigmbrc.h"
#include "libquasd/file.h"
#include "uiprogressdialog.h"
#include "uisettingsdialog.h"

#include <fstream>
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
	
	path_application = 
		quasd::settings::get_path_userhome() + '/' + ".config/quasd/xwaxdb";
	settings.set_path_application(path_application);
	settings.set_filename_settings("xwaxdb.settings");
	if ( !settings.parse_file_settings() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__ 
			<< ":cannot parse:"
			<< settings.get_filepath_settings() 
			<< std::endl;
		return false;
	}
  else
  {
    std::cout << "settings:" << std::endl;
    for ( const auto Key : settings.get_section_data("general") )
    {
      std::cout << Key.first << "=" << Key.second << std::endl;
    }
  }
  
	ptr_xdb = std::shared_ptr<xwaxdb::db_crates>(new xwaxdb::db_crates);
	ptr_xdb->set_path_application(path_application);	
	ptr_xdb->set_check_bpm(true);
	if ( !ptr_xdb->read_xwax_db() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__ 
			<< ":cannot read:"
			<< path_application << "/xwax.db"
			<< std::endl;
		return false;
	}
	ptr_xdb->read_crates_db();

  std::string pathCrate = settings.get_section_value("general", "path_crate");			
  ptr_xdb->set_path_crate(pathCrate);
	
	
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
  std::string gmbrc_path = settings.get_section_value("general", "gmbrc_path");
  if ( gmbrc_path == "%std%")
  {
    c_gmbrc.use_gmbrc_stdpath();
  }
  else
  {
    c_gmbrc.set_gmbrc_path(gmbrc_path);
  }
	
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
    init_settings_ui(dlg);
		if ( dlg->exec() )
    {
      process_settings_ui(dlg);
      write_settings_file();
    }
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
UiMainWindow::init_settings_ui(UiSettingsDialog *dlg)
{
  if ( settings.get_section_value("general", "crate_gmbrc") == "on" )
  {
    dlg->cbtn_gmbrc->set_active(true);
  }
  else
  {
    dlg->cbtn_gmbrc->set_active(false);
  }
  
  std::string gmbrc_path = settings.get_section_value("general", "gmbrc_path");
  if ( gmbrc_path == "%std%" )
  {
    dlg->rbtn_std_gmbrc->set_active(true);
    dlg->entry_custom_gmbrc->delete_text(0, -1);
  }
  else
  {
    dlg->rbtn_std_gmbrc->set_active(false);
    dlg->entry_custom_gmbrc->set_text(gmbrc_path);
  }
  
  if ( settings.get_section_value("general", "crate_audiofiles") == "on" )
  {
    dlg->cbtn_af->set_active(true);
  }
  else
  {
    dlg->cbtn_af->set_active(false);
  }

  if ( settings.get_section_value("general", "crate_pls") == "on" )
  {
    dlg->cbtn_pls->set_active(true);
  }
  else
  {
    dlg->cbtn_pls->set_active(false);
  }
  
  std::string path_crate = settings.get_section_value("general", "path_crate");
  dlg->entry_crates_path->set_text(path_crate);
  
  if ( settings.get_section_value("general", "scan_bpm") == "on" )
  {
    dlg->cbtn_bpm_scan->set_active(true);
  }
  else
  {
    dlg->cbtn_bpm_scan->set_active(false);
  }
}

void
UiMainWindow::process_settings_ui(UiSettingsDialog* dlg)
{
  if ( dlg->cbtn_gmbrc->get_active() )
  {
    settings.set_section_value("general", "crate_gmbrc", "on");
  }
  else
  {
    settings.set_section_value("general", "crate_gmbrc", "off");
  }
  
  if ( dlg->rbtn_std_gmbrc->get_active() )
  {
    settings.set_section_value("general", "gmbrc_path", "%std%");
  }
  else
  {
    std::string gmbrc_path = dlg->entry_custom_gmbrc->get_text();
    settings.set_section_value("general", "gmbrc_path", gmbrc_path);
  }
  
  if ( dlg->cbtn_af->get_active() )
  {
    settings.set_section_value("general", "crate_audiofiles", "on");
  }
  else
  {
    settings.set_section_value("general", "crate_audiofiles", "off");
  }
  
  if ( dlg->cbtn_pls->get_active() )
  {
    settings.set_section_value("general", "crate_pls", "on");
  }
  else
  {
    settings.set_section_value("general", "crate_pls", "off");
  }
  
  std::string path_crate = dlg->entry_crates_path->get_text();
  settings.set_section_value("general", "path_crate", path_crate);
  
  if ( dlg->cbtn_bpm_scan->get_active() )
  {
    settings.set_section_value("general", "scan_bpm", "on");
  }
  else
  {
    settings.set_section_value("general", "scan_bpm", "off");
  }  
}

void
UiMainWindow::write_settings_file()
{
	std::string Path = path_application + "/xwaxdb.settings";

  std::ofstream F(Path);
  if ( !F )
  {
    std::cerr << "ERR:" << __PRETTY_FUNCTION__
      << ":cannot create:"
      << Path
      << std::endl;
    return;
  }
  
  F << "######################" << std::endl
    << "# xwaxdb config file #" << std::endl
    << "######################" << std::endl
    << std::endl
    << "[general]"
    << std::endl;
  
  for ( const auto Key : settings.get_section_data("general") )
  {
    F << Key.first << "=" << Key.second << std::endl;
  }
  
  F.close();
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



