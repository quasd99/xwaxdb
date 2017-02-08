#include "crate_audiofiles.h"
#include "libquasd/file.h"
#include "libquasd/tagtool.h"

using namespace xwaxdb;
 
crate_audiofiles::crate_audiofiles()
{
  v_ext = {".mp3", ".flac", ".ogg", ".m4a", ".wav", ".aiff"};
}

std::vector<std::string>
crate_audiofiles::get_crate_directories(const std::string& path)
{   
	// every dir is a crate that will be scanned for audiofiles
	std::vector<std::string> vRet;
	for (const auto pEntry : quasd::file::get_dir_content(path) )
	{
		if (quasd::file::get_basename(pEntry) == "xwax_tmp" )
			continue;
		if ( quasd::file::is_dir(pEntry) )
			vRet.push_back(pEntry);
	}
	return std::move(vRet);
}

bool
crate_audiofiles::scan() 
{       
	// iterate crate-root-content
	for (const auto Entry : get_crate_directories(ptr_xdb->get_path_crate() ) )
	{
		// get all audiofiles in this directory/crate
		std::vector<std::string> vAf = 
			quasd::file::get_dir_content_recursive(Entry, v_ext);
		
		if ( vAf.size() == 0 ) continue; // skip empty dir
		
		// create crate or get crate from db
		s_crate sCrate = crate::handle_crate("audiofile", 
																				 quasd::file::get_basename(Entry));
		sCrate.path = Entry;
		
		// pass all audiofiles to db. All audiofile-ids are written to sCrate.v_afid
		int countNewInDb = handle_aflist(vAf, sCrate); 
		
		if ( sCrate.b_is_new )
		{
			ptr_xdb->append_crate(sCrate);
		}
		
		std::cout << "audiofile-crate:" << sCrate.name_orig 
			<< ":" << sCrate.v_afid.size()
			<< ", new in DB:" << countNewInDb
			<< ", new in pls:" << sCrate.v_new_afid.size()
			<< std::endl;		
	}
	return true;
}
