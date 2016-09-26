#include "crate.h"
#include "libquasd/file.h"
#include "libquasd/m3utool.h"
#include "libquasd/tagtool.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <fstream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace xwaxdb;
 
crate::crate()
{
	tag_parser.set_tagframefilter_common(v_common_tags);
}

xwaxdb::s_crate
crate::handle_crate(const std::string& type, const std::string& name_orig)
{
	xwaxdb::s_crate cRet;
	if ( !ptr_xdb->getw_crate(type, name_orig, cRet)) // <- write to cRet
	{ // not found in db, generate new one
		cRet.name_orig = name_orig;
		cRet.type = type;
		cRet.name_uniq = ptr_xdb->get_unique_cratename(cRet.name_orig, cRet.type);
		cRet.b_is_new = true; // ? TODO new crate	
		std::cout << "new gmbrc crate:" << cRet.name_orig << std::endl;
	}
	
	return cRet;
}

int
crate::handle_aflist(const std::vector<std::string>& v_af, xwaxdb::s_crate& crate)
{
	int countNewInDb = 0;
	for (auto Af : v_af)
	{ 
		unsigned int Id = ptr_xdb->get_af_id(Af);
		if ( Id == 0 )
		{ // add to db
			Id = ptr_xdb->append_af(Af); // parse tags, add to db, get id
			countNewInDb++;
		}

		// check if file is new in this crate
		if ( std::find(crate.v_afid.begin(), crate.v_afid.end(), Id) 
				 == crate.v_afid.end() )
		{
			crate.v_afid.push_back(Id);
			crate.v_new_afid.push_back(Id);
		}
	}
	
	return countNewInDb;
}
