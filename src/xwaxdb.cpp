#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>

#include "crate.h"
#include "crate_audiofiles.h"
#include "crate_gmbrc.h"
#include "crate_pls.h"
#include "db_crates.h"

#include "src/uimainwindow.h"

#include "settings.h"
#include "libquasd/file.h"

#include <glibmm-2.4/glibmm/ustring.h>


void
print_sep()
{
    std::cout << std::string(80, '#') << std::endl;
}

/**
 * @brief Get the app-dir in user home dir
 * @return std;;string path
 */
std::string get_path_app()
{
	return quasd::settings::get_path_userhome() + '/' + ".config/quasd/xwaxdb";
}


/**
 * @brief Get the preselected gmbrc-playlists
 * @return std::vector<std::string> gmbrc-playlist-names
 */
std::vector<std::string> get_gmbrc_preselected_pls()
{
	std::vector<std::string> vRet;
	std::string plsPath = get_path_app() + "/gmbrc_preselected_pls.lst";
	std::ifstream F(plsPath);
	if (!F)
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__ << ":cannot open:" << plsPath
			<< std::endl;
		return vRet;
	}

	std::string Line;
	while (getline(F, Line))
	{
		vRet.push_back(Line);
	}
	F.close();
	
	return std::move(vRet);
}

int run_cli_mode(bool b_bpmtag = false)
{
// application settings
	std::string str_path_app = get_path_app();
	std::string str_filename_settings = "xwaxdb.settings";

	quasd::settings S{str_path_app, str_filename_settings};
	if ( !S.parse_file_settings() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__ << ":cannot parse:"
			<< S.get_filepath_settings() << std::endl;
		//TODO use standard-values/write or copy standard-settings-file
		return 1;
	}

	std::string pathCrate = S.get_section_value("general", "path_crate");
	if ( pathCrate.empty() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":path_crate not set:" << std::endl
			<< "  Please open the settings-file (xwaxdb -s) and specify the path\n"
			<< "  that will be scanned for audiofiles & m3u-playlists."
			<< std::endl;
		return 1;
	}

	// xdb
	std::shared_ptr<xwaxdb::db_crates> ptrXdb(new xwaxdb::db_crates);
	ptrXdb->set_path_application(str_path_app);
	ptrXdb->set_path_crate(pathCrate);
	ptrXdb->set_check_bpm(true);
	if ( !ptrXdb->read_xwax_db() )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__ << ":cannot read:"
			<< ptrXdb->get_path_application() << "/xwax.db"
			<< std::endl;
	}
	ptrXdb->read_crates_db();

	print_sep();

 // gmbrc
	if ( S.get_section_value("general", "crate_gmbrc") == "on")
	{
		xwaxdb::crate_gmbrc c_gmbrc;
		c_gmbrc.set_xdb(ptrXdb);
		c_gmbrc.use_gmbrc_stdpath();
		if ( !c_gmbrc.init_gmbrc() )
		{
			std::cerr << "ERR:" << __PRETTY_FUNCTION__
				<< ":cannot parse gmbrc:"
				<< std::endl;
		}
		c_gmbrc.read_pls_selected();
		c_gmbrc.scan_selected_pls(c_gmbrc.get_pls_selected());
		print_sep();
	}
	
 // audiofiles
	if ( S.get_section_value("general", "crate_audiofiles") == "on")
	{
	 // audiofiles
		xwaxdb::crate_audiofiles c_af;
		c_af.set_xdb(ptrXdb);
		c_af.scan();
		print_sep();
	}
	
	// pls	
	if ( S.get_section_value("general", "crate_pls") == "on")
	{
		xwaxdb::crate_pls c_pls;
		c_pls.set_xdb(ptrXdb);
		c_pls.scan();
		print_sep();
	}

	ptrXdb->write_xwax_db();
	ptrXdb->write_crates_db();
	ptrXdb->write_dbfiles();
	ptrXdb->write_lparams_file();
	
	
	// summary

//	if ( !b_bpmtag)
//	{
//		std::cout << "new files with no bpm:" << ptrXdb->get_no_bpm().size()<< std::endl;
//	}
	
	std::cout << "ready. launch xwax." << std::endl;
	
	return 0;
}

int
main (int argc, char *argv[])
{		
	if(argc<=1)
	{
		std::cout << "No arguments: using gui" << std::endl;
    Glib::RefPtr<Gtk::Application> app;
    app = Gtk::Application::create (argc, argv, "org.quasd.xwax_db");
    UiMainWindow window;
    return app->run(window);
	}
	else
	{
		//	int index;
		int C;
		int bFlag = 0;
		int nFlag = 0;
		opterr = 0;
		
		std::string pathCrate;
		
		while ((C = getopt (argc, argv, "bclhnps")) != -1)
		{
			switch(C)
			{				
				case 'b':
					bFlag = 1;
					break;
				case 'h':
					std::cout << "xwaxdb cli interface:" << std::endl
						<< "  -b\tauto bpm-tagging during scan (using bpm-tag). Only in combination with -n." << std::endl
						<< "  -l\tprint gmbrc-playlists and exit" << std::endl
						<< "  -n\tno gui (complete scan using xwaxdb.settings-file)" << std::endl
						<< "  -p\tprint preselected gmbrc-playlists filepath and exit" << std::endl
						<< "  -s\tprint settings-file path and exit" << std::endl;
					return 0;
					break;
				case 'l':
					quasd::gmbrctool::print_gmbrc_stdpath();
					quasd::gmbrctool::print_gmbrc_playlists();
					return 0;
					break;
				case 'n':
					nFlag = 1;
					break;
				case 'p':
					std::cout << get_path_app() << "/gmbrc_preselected_pls.lst" << std::endl;
					return 0;
					break;
				case 's':
					std::cout << get_path_app() << "/xwaxdb.settings" << std::endl;
					return 0;
					break;
				case '?':
					std::cerr << char(optopt) << " is not a valid option" << std::endl;
					return 1;
					break;
				default:
					std::cerr << "error, use -h for help" << std::endl;
					return 1;
					break;
			 }
		}

		if ( nFlag == 1 )
		{
			if ( bFlag == 1 )
			{
				return run_cli_mode(true);
			}
			else
			{
				return run_cli_mode();
			}
		}
//		if ( optind == argc	)
//		{
//			std::cerr << "no path to audio-file specified" << std::endl;
//			return 1;
//		}
//
//		for (int i = optind; i < argc; i++)
//		{
//
//		}
	}
	
}
