#include "src/global.h"
#include "src/crate_gmbrc.h"
#include "libquasd/gmbrcplaylistexporter.h"


#include <iostream>
#include <fstream>

using namespace xwaxdb;
 
crate_gmbrc::crate_gmbrc()
{
}

bool
crate_gmbrc::init_gmbrc()
{
	bool bSuccess = gt.parse_playlists();
	v_pls_all = gt.get_pls_all();
	return bSuccess;
}

bool
crate_gmbrc::scan_selected_pls(const std::vector<std::string>& v_pls)
{
	// scan the user-selected playlists for updates
	for (const auto plsTitle : v_pls)
	{
		// get the crate or generate new one
		s_crate sCrate = crate::handle_crate("gmbrc", plsTitle);
		sCrate.path = "gmbrc";
		
		// get af-data from quasd::gmbrctool
		auto vAfData = gt.get_pls_afdata(plsTitle);
		if ( vAfData.size() == 0 ) continue; // Playlist not found or empty
		
		// process all audiofiles in the gmbrc-pls
		int countNewInDb = 0;
		for (auto mapAfData : vAfData)
		{ 
			// get the af-id from ptr_xdb
			std::string afPath = mapAfData["filepath"];
			unsigned int Id = ptr_xdb->get_af_id(afPath);
			if ( Id == 0 )
			{ 
				Id = ptr_xdb->append_afdata(afPath, mapAfData);  // add to db, get id
				countNewInDb++;
			}
			
			// check if file is new in this crate
			if ( std::find(sCrate.v_afid.begin(), sCrate.v_afid.end(), Id) 
					 == sCrate.v_afid.end() )
			{
				sCrate.v_afid.push_back(Id);
				sCrate.v_new_afid.push_back(Id);
			}
		}
		
		if ( sCrate.b_is_new )
		{
			ptr_xdb->append_crate(sCrate);
		}
		
		std::cout << "gmbrc-crate:" << sCrate.name_orig 
			<< ":" << sCrate.v_afid.size()
			<< ", new in DB:" << countNewInDb
			<< ", new in pls:" << sCrate.v_new_afid.size()
			<< std::endl;				
	}
	
	return true;
}

void
crate_gmbrc::use_gmbrc_stdpath()
{
    gt.use_gmbrc_stdpath();
}

void
crate_gmbrc::read_pls_selected()
{
	std::string Path = ptr_xdb->get_path_application() + '/' 
		+ str_filename_pls_selection;
	std::ifstream F(Path);
	if (!F) {
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
						<< ":cannot open:"
						<< Path
						<< std::endl;
		return;
	}

	std::string Line;
	while (getline(F, Line))
	{
		if ( Line.size() > 0 )
		{
				v_pls_selected.push_back(Line);
		}
	}

	F.close();
}