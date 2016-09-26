#include "crate_pls.h"
#include "libquasd/file.h"
#include "libquasd/m3utool.h"
#include "libquasd/tagtool.h"

using namespace xwaxdb;
 
crate_pls::crate_pls()
{
}

bool
crate_pls::scan()
{
	// get playlists
	for (const auto plsPath : quasd::file::get_dir_content(ptr_xdb->get_path_crate(), ".m3u"))
	{		
		// parse playlist
		std::vector<std::string> vAf = 
			quasd::m3utool::parse_simple(plsPath, true, true);
		
		if ( vAf.size() == 0 ) continue; // skip empty directory
		
		s_crate sCrate = crate::handle_crate("pls", 
																				 quasd::file::get_basename(plsPath));
		sCrate.path = plsPath;

		int countNewInDb = handle_aflist(vAf, sCrate); // parse tags & add to db

		if ( sCrate.b_is_new )
		{
			ptr_xdb->append_crate(sCrate);
		}
		
		std::cout << "pls-crate:" << sCrate.name_orig 
			<< ":" << sCrate.v_afid.size()
			<< ", new in DB:" << countNewInDb
			<< ", new in pls:" << sCrate.v_new_afid.size()
			<< std::endl;		

	}
    
	return true;
}
