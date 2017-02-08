#include "db_crates.h"
#include "libquasd/file.h"

#include <fstream>
#include <iostream>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <iosfwd>
#include <sstream>

using namespace xwaxdb;

db_crates::db_crates()
{
	tag_parser.set_tagframefilter_common(v_common_tags);
}

unsigned int
db_crates::append_af(const std::string& af)
{
	if ( !map_af_id.insert(std::make_pair(af, ++id_af)).second )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":file already in db:"
			<< af
			<< std::endl;
		return 0;
	}
	
	// parse tags
	std::unordered_map<std::string, std::string> mTags;
	if ( !tag_parser.parse(af, mTags) ) // write to mTags
	{
		// failed parsing tags: check if file exist
		if ( !quasd::file::is_regular_file(af) )
		{
			std::cerr << "ERR:" << __PRETTY_FUNCTION__
				<< ":cannot access file:"
				<< af
				<< std::endl;
			
			set_af_notfound.insert(id_af);
		}
	}
		
	check_bpm(mTags["bpm"], id_af);
	
	mTags["filepath"] = af;
	map_id_afdata.insert(std::make_pair(id_af, mTags));
	
	return id_af;
}

unsigned int
db_crates::append_afdata(const std::string& af,
                         std::unordered_map<std::string, std::string>& dat)
{
	if ( !map_af_id.insert(std::make_pair(af, ++id_af)).second )
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":file already in db:"
			<< af
			<< std::endl;
		return 0;
	}
	
	check_bpm(dat["bpm"], id_af);
		
	if ( dat["filepath"].empty() )
		dat["filepath"] = af;

	map_id_afdata.insert(std::make_pair(id_af, dat));
	return id_af;
}

void
db_crates::scan_bpm()
{
	std::string strExec1 = "bpm-tag -f \"";
	std::string strExec2 = "\" 2> /tmp/bpm-tag.resp";
	
	std::vector<unsigned int> vScanned;
	int tracksTotal  = set_no_bpm.size();
	int currentTrack = 0;
	for (const auto Afid : set_no_bpm)
	{
		if ( b_abort_bpmscan ) break;
		currentTrack++;
		
		std::string afPath = get_af_data(Afid)["filepath"];
		double Fraction = double(currentTrack) / tracksTotal;
		std::stringstream ssMsg;
		ssMsg << currentTrack << '/' << tracksTotal << ": " << afPath << std::endl;
		std::string Msg = ssMsg.str();
		std::cout << Msg;
		
		signal_new_bpmscan.emit(Fraction, Msg);
		
		std::stringstream ssExec;
		ssExec << strExec1 << afPath << strExec2;
		int status = std::system(ssExec.str().c_str());
		if ( status == 0 )
		{
			std::ifstream F("/tmp/bpm-tag.resp");
			std::stringstream ssBpm;
			ssBpm << F.rdbuf();
			std::string Bpm = parse_bpmtag_resp(ssBpm.str());
			
			if ( !Bpm.empty() )
			{
				map_id_afdata[Afid]["bpm"] = Bpm; // write to db
				vScanned.push_back(Afid);
			}
		}
	}
	
	std::cout << "Info:" << __PRETTY_FUNCTION__ << ":"
		<< vScanned.size() << " files bpm-tagged."
		<< std::endl;
	for (const auto Afid : vScanned)
	{
		set_no_bpm.erase(Afid);
	}
	
	b_abort_bpmscan = false;
}

std::string
db_crates::parse_bpmtag_resp(const std::string& resp)
{
	std::string strRet{""};
	std::size_t Pos1 = resp.find_first_of(':');
	if ( Pos1 != std::string::npos )
	{
		strRet = resp.substr(Pos1+2);
		std::size_t Pos2 = strRet.find_first_of('B');
		if ( Pos2 != std::string::npos )
		{
			strRet = strRet.substr(0, Pos2-1);
		}
	}
	return strRet;
}

void
db_crates::check_bpm(std::string& bpm, const unsigned int id_af)
{
	if ( !bpm.empty() )
		std::replace( bpm.begin(), bpm.end(), ',', '.');
	
	if ( bpm.empty() || bpm == "0" )
	{
		set_no_bpm.insert( id_af );
	}	
}

bool
db_crates::is_in_db(const std::string& af)
{
	auto Iter = map_af_id.find(af);
	if ( Iter != map_af_id.end() )
		return true;
	else
		return false;
}

std::string
db_crates::get_unique_cratename(const std::string& name,
                                const std::string& type) 
{
  std::string retN{name};
  std::replace( retN.begin(), retN.end(), ' ', '_');
  if ( !is_unique_cratename(retN) || b_append_cratetype )
  {
    retN = retN + '(' + type + ')';
  }
  
  return std::move(retN);
}

bool
db_crates::is_unique_cratename(const std::string& name)
{
	for (const auto Crate : v_crates)
	{
		if ( Crate.name_uniq == name )
			return false;
	}
	return true;
}

bool
db_crates::read_xwax_db()
{
  std::string Path = str_path_application + '/' + str_dbfile;
  std::ifstream F(Path);
  if (!F) 
	{
      std::cerr << "ERR:" << __PRETTY_FUNCTION__
        << ":cannot open:"
        << Path
        << std::endl;
      return false;
  }

  std::string Line;
  while (getline(F, Line))
  {
    parse_xwaxdb_line(Line);
  }

  F.close();
  return true;
}

void
db_crates::read_crates_db()
{
	std::string Path = str_path_application + "/crates.db";
  std::ifstream F(Path);
  if ( !F )
	{
      std::cerr << "ERR:" << __PRETTY_FUNCTION__
        << ":cannot open:"
        << Path
        << std::endl;
      return;
  }
	
  std::string Line;
  while (getline(F, Line))
	{
		parse_cratedb_line(Line); // write data to v_crates
	}
	
}

void
db_crates::parse_cratedb_line(const std::string& line)
{
  // Format crates.db: tab-seperated strings in order
  // type,name_orig,name_uniq,path,af-ids(list: whitespace-seperated)

  std::vector<std::string> vToken;
  boost::split(vToken, line, boost::is_any_of("\t"), boost::token_compress_off);
	
	if ( vToken.size() >= 5 )
	{
		xwaxdb::s_crate S;
		S.type      = vToken[0];
		S.name_orig = vToken[1];
		S.name_uniq = vToken[2];
		S.path      = vToken[3];
		S.v_afid = parse_cratedb_afid(vToken[4]);
		
		v_crates.push_back(std::move(S));
	}
	else
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":error parsing line:"
			<< line
			<< std::endl;
	}
}

unsigned int
db_crates::get_af_id(const std::string& af)
{
	auto Iter = map_af_id.find(af);
	if ( Iter != map_af_id.end() )
		return Iter->second;
	else
		return 0;
}


std::vector<unsigned int>
db_crates::parse_cratedb_afid(const std::string& tok)
{
	// parse the whitespace-seperated audiofile-id's string
  std::vector<std::string> vToken;
  boost::split(vToken, tok, boost::is_any_of(" "), boost::token_compress_off);
	
	std::vector<unsigned int> vRet;
	for (auto Id : vToken)
	{
		if ( !Id.empty() )
			vRet.push_back(atoi(Id.c_str()));
	}
	return std::move(vRet);
}



bool
db_crates::write_xwax_db() 
{
  std::string Path = str_path_application + '/' + str_dbfile;
  std::ofstream F(Path);
  if ( !F )
  {
      std::cerr << "ERR:" << __PRETTY_FUNCTION__
        << ":cannot open:"
        << Path
        << std::endl;
      return false;        
  }

  for (auto Entry : map_id_afdata)
  {
    F << Entry.first << '\t' 
			<< Entry.second["filepath"] << '\t'
			<< Entry.second["artist"]  << '\t'  
			<< Entry.second["title"] << '\t'
			<< Entry.second["album"] << '\t'
			<< Entry.second["year"] << '\t'
			<< Entry.second["bpm"] << '\t'
			<< Entry.second["rating"] << '\t'
			<< Entry.second["publisher"] << '\t'
			<< std::endl;
  }

  F.close();

  return true;
}

void
db_crates::parse_xwaxdb_line(const std::string& line)
{
  // Format xwax.db: tab-seperated strings in order
  // id,filepath,artist,title,album,year,bpm,rating,publisher 

  std::vector<std::string> vToken;
  boost::split(vToken, line, boost::is_any_of("\t"), boost::token_compress_off);

  if ( vToken.size() >= 9 )
  {
		unsigned int Id = atoi(vToken[0].c_str());
		map_af_id.insert(std::make_pair(vToken[1], Id));
		
    std::unordered_map<std::string, std::string> M;
    M["filepath"]  = vToken[1];
    M["artist"]    = vToken[2];
    M["title"]     = vToken[3];
    M["album"]     = vToken[4];
    M["year"]      = vToken[5];
    M["bpm"]       = vToken[6];
    M["rating"]    = vToken[7];
    M["publisher"] = vToken[8];
		
		check_bpm(M["bpm"], Id);
    
		map_id_afdata.insert(std::make_pair(Id, M));
		
		if ( Id > id_af ) id_af = Id;
  }
  else
  {
    std::cerr << "ERR:" << __PRETTY_FUNCTION__
      << ":cannot parse:"
      << line
      << std::endl;
  }
}

int
db_crates::get_total_af()
{
  return map_af_id.size();
}

void
db_crates::write_crates_db()
{
  std::string Path = str_path_application + "/crates.db";
  std::ofstream F(Path);
  if (!F) 
  {
    std::cerr << "ERR:" << __PRETTY_FUNCTION__
      << ":cannot open:"
      << Path
      << std::endl;
    return;
  }

	for (auto Crate : v_crates)
	{
		F << Crate.type << '\t'
			<< Crate.name_orig  << '\t'
			<< Crate.name_uniq << '\t'
			<< Crate.path << '\t';
		
		for (auto Id : Crate.v_afid)
		{
			F << Id << ' ';
		}
		
		F << std::endl;
	}
 
  F.close();
}

bool
db_crates::is_new_crate(const std::string& type, const std::string& name_orig)
{
	for (auto Crate : v_crates)
	{
		if ( Crate.type == type && Crate.name_orig == name_orig )
			return true;
	}
	return false;
}

std::vector<xwaxdb::s_crate>
db_crates::get_crates()
{
	return v_crates;
}


bool
db_crates::getw_crate(const std::string& type,
											const std::string& name_orig,
											xwaxdb::s_crate& cr)
{
	for (auto Crate : v_crates)
	{
		if ( Crate.type == type && Crate.name_orig == name_orig )
		{
			cr = Crate;
			return true;
		}
	}
	return false;
}


std::unordered_map<std::string, std::string>
db_crates::get_af_data(unsigned int id)
{
	std::unordered_map<std::string, std::string> mRet;
	auto Iter = map_id_afdata.find(id);
	if ( Iter != map_id_afdata.end() )
	{
		mRet = Iter->second;
	}
	return mRet;
}

void
db_crates::write_dbfiles()
{
	std::string PathDbFiles = str_path_application + "/db/";
	for (const auto Crate : v_crates)
	{
		std::string Path = PathDbFiles + Crate.name_uniq + ".xwaxdb";
		std::ofstream F(Path);
		if (!F)
		{
			std::cerr << "ERR:" << __PRETTY_FUNCTION__
				<< ":cannot create:"
				<< Path
				<< std::endl;
			return;
		}

		for (const auto afId : Crate.v_afid)
		{
			std::unordered_map<std::string, std::string> mDat = get_af_data(afId);
			F << mDat["filepath"] << '\t'
				<< mDat["artist"] << '\t'
				<< mDat["title"] << '|'
				<< mDat["album"] << '|'
				<< mDat["year"] << "|R"
				<< mDat["rating"] << '\t'
				<< mDat["bpm"]
				<< std::endl;
		}

		F.close();
	}
}

void
db_crates::write_lparams_file()
{
  std::string Path = str_path_application + "/xwax_lparam.lst";
  std::ofstream F(Path);
	
  if (!F) 
  {
    std::cerr << "ERR:" << __PRETTY_FUNCTION__
      << ":cannot open:"
      << Path
      << std::endl;
    return;
  }

	for (const auto Crate : v_crates)
	{
		F << Crate.name_uniq << "\n";
	}
	
	F.close();
}

