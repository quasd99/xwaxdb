#include <glibmm-2.4/glibmm/miscutils.h>
#include <glibmm-2.4/glibmm/uriutils.h>

#include <fstream>
#include <iostream>
#include <regex>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include "gmbrctool.h"

using namespace quasd;
 
gmbrctool::gmbrctool()
{
}

void
gmbrctool::print_gmbrc_stdpath()
{
	std::cout << get_gmbrc_stdpath() << std::endl;
}

std::string
gmbrctool::get_gmbrc_stdpath()
{
	return Glib::get_home_dir() + "/.config/gmusicbrowser/gmbrc";
}

void
gmbrctool::print_gmbrc_playlists()
{
	for (auto plsname : get_playlistnames())
	{
		std::cout << plsname << std::endl;
	}
}

bool
gmbrctool::is_in_unique_trackids(const std::string& line)
{
	// extract the track-id
	std::size_t Pos = line.find_first_of('\t');
	if ( Pos!=std::string::npos )
	{   
		std::string strID = line.substr(0, Pos);
		if ( !strID.empty() )
		{
			unsigned int ID = atoi(strID.c_str());
			if ( set_unique_trackids.find(ID) != set_unique_trackids.end() )
			{
				return true;
			}            
		}
	}
	return false;
}


std::vector<std::string>
gmbrctool::get_playlistnames()
{
	std::vector<std::string> vRet;
	std::string Path = get_gmbrc_stdpath();
	std::ifstream F(Path);
	if (!F)
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":cannot open:"
			<< Path
			<< std::endl;
		return vRet;
	}
	
	
	std::string Line;
	int State = 0;
	std::regex regState2 ("SavedOn:(.*)"); // end of playlist-section
    
	while (getline(F, Line))
	{
		// check state
		if ( Line.compare("SavedLists:") == 0 )
		{
			getline(F, Line); // get the first playlist
			State = 1;
		}
		else if ( std::regex_match(Line, regState2) )
		{
			break;
		}
		
		if ( State == 1 )
		{
			vRet.push_back(parse_playlistname(Line));
		}
	}
	return vRet;
}

std::string
gmbrctool::parse_playlistname(const std::string& line)
{
	std::string Name{""};
	std::size_t Pos = line.find_first_of(':');
	if ( Pos!=std::string::npos )
	{ // extract name and remove " from title 
		Name = line.substr(2, (Pos-2));
		Name.erase(std::remove(Name.begin(), Name.end(), '"'), Name.end());
	}
	return Name;
}

bool
gmbrctool::parse_playlists()
{
	b_all_tracks = false;
	std::string Path = get_gmbrc_stdpath();
	std::ifstream F(Path);
	if (!F)
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":cannot open:"
			<< Path
			<< std::endl;
		return false;
	}
	
	int State = 0;
	std::string Line;  
	while (getline(F, Line))
	{		
		// check state
		if (Line.compare("SavedLists:") == 0)
		{
				std::cout << "Info:" << __PRETTY_FUNCTION__
									<< ":processing playlist-section"
									<< std::endl;

				getline(F, Line); // get the first playlist
				State = 1;
		}
		else if (std::regex_match (Line, regex_state2))
		{
				State = 2;
		}
		else if (Line.compare("[Songs]")  == 0)
		{
				std::cout << "Info:" << __PRETTY_FUNCTION__
									<< ":processing songs-section"
									<< std::endl;
				getline(F, Line); // skip explanation line
				getline(F, Line); // get first line
				State = 3;
		}
		else if ( Line.compare("[album]")  == 0 )
		{
				std::cout << "Info:" << __PRETTY_FUNCTION__ << ":ready." << std::endl;
				std::cout << "  " << map_track_data.size() << " tracks parsed in "
					<< map_pls_data.size() << " playlists." << std::endl;
				break;
		}

		switch (State)
		{
			case 1:
				parse_playlist_line(Line);
				break;
			case 2:
				// waiting for state 3
				break;
			case 3:
				if ( !b_all_tracks )
				{
					if ( is_in_unique_trackids(Line) )
					{
						parse_track_line(Line);
					} // else: line is not parsed
				}
				else
				{
					parse_track_line(Line);
				}
				break;
		}
	}
	F.close();
	return true;
}

bool
gmbrctool::parse_playlists(std::set<std::string> v_pls)
{
	b_all_pls = false;
	set_pls_selection = std::move(v_pls);
	return parse_playlists();
}

void
gmbrctool::parse_playlist_line(const std::string& line)
{	
	
	std::size_t Pos = line.find_first_of(':');
	if ( Pos != std::string::npos )
	{   
		// extract name and remove " from title
		std::string Name = line.substr(2, (Pos-2) );
		Name.erase(std::remove(Name.begin(), Name.end(), '"'), Name.end());
		
		if ( !b_all_pls && 
			   ( set_pls_selection.find(Name) == set_pls_selection.end() ) )
		{  // add only user-selected playlists to map_pls_data
			return;
		}
			
		// extract track-IDs
		std::string trackIDs = line.substr((Pos+20));
		std::vector<std::string> vTokens;
		boost::split(vTokens, trackIDs, boost::is_any_of(" "), boost::token_compress_on);
		std::vector<unsigned int> vPlsID;
		for ( const auto strID : vTokens )
		{
			if (strID != "''") // empty playlist
			{
				unsigned int id = atoi(strID.c_str());
				if ( !b_all_tracks ) set_unique_trackids.insert(id);
				vPlsID.push_back(id); // save it
			}
		}
		map_pls_data.insert(std::make_pair(Name, std::move(vPlsID)));
	}
	else
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":error gmbrc-playlist-entry:" << line
			<< std::endl;                 
	}
}

void
gmbrctool::parse_track_line(const std::string& line)
{
// split to tokens
	std::vector<std::string> vTokens;
	boost::split(vTokens, line, boost::is_any_of("\t"), boost::token_compress_off);

	if (!vTokens.empty())
	{
		//s_gpe_trackdata t;
		std::unordered_map<std::string, std::string> M;
		unsigned int ID = std::stoul(vTokens[0], nullptr, 0);

		M["id"]       = ID;
		M["artist"]   = vTokens[4];
		M["album"]    = vTokens[2];
		M["bpm"]      = vTokens[6];
		std::string str_file = Glib::uri_unescape_string(vTokens[22]);
		std::string str_path = Glib::uri_unescape_string(vTokens[11]);
		M["filepath"] = str_file + "/" + str_path;
		M["title"]    = vTokens[33];
		M["genre"]    = vTokens[13];
		M["label"]    = vTokens[15];
		M["length"]   = vTokens[18];
		M["year"]     = vTokens[36];
		M["rating"]   = vTokens[25];
		// t.bitrate  = atoi(vTokens[5].c_str());
		// t.samplerate = atoi(vTokens[30].c_str());

		map_track_data.insert(std::make_pair(ID, M));
	}
	else
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":error reading:" << line
			<< std::endl;                 
	}	
}

std::vector<std::string>
gmbrctool::get_pls_all()
{
	std::vector<std::string> vRet;
	for (const auto M : map_pls_data)
	{
		vRet.push_back(M.first);
	}
	return std::move(vRet);
}

std::vector<std::unordered_map<std::string, std::string>>
gmbrctool::get_pls_afdata(const std::string& name)
{
	std::vector<std::unordered_map<std::string, std::string>> vRet;
	std::set<std::string> setUniqueEntries;

	auto it = map_pls_data.find(name);
	if ( it != map_pls_data.end() )
	{
		for (const auto trackID : it->second)
		{
			std::unordered_map<std::string, std::string> M = get_trackdata_by_id(trackID);
			if ( setUniqueEntries.insert(M["filepath"]).second )
			{ // only add unique playlist-entries like gmusicbrowser
				vRet.push_back(M);
			}
		}
	}
	else
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":playlist-name not found:"
			<< name
			<< std::endl;
	}
	return std::move(vRet);
}

std::vector<std::string>
gmbrctool::get_pls_af(const std::string& name)
{
	std::vector<std::string> vRet;
	std::set<std::string> setUniqueEntries;
	auto it = map_pls_data.find(name);
	if ( it != map_pls_data.end() )
	{
		for (const auto trackID : it->second)
		{
			std::unordered_map<std::string, std::string> M = get_trackdata_by_id(trackID);
			if ( setUniqueEntries.insert(M["filepath"]).second )
			{ // only add unique playlist-entries like gmusicbrowser
				vRet.push_back(M["filepath"]);
			}
		}
	}
	else
	{
		std::cerr << "ERR:" << __PRETTY_FUNCTION__
			<< ":playlist-name not found:"
			<< name
			<< std::endl;
	}
	return std::move(vRet);
}


std::unordered_map<std::string, std::string>
gmbrctool::get_trackdata_by_id(const unsigned int& id)
{
    auto IT = map_track_data.find(id);
    return IT->second;
}

void
gmbrctool::use_gmbrc_stdpath()
{
	str_gmbrc_path = get_gmbrc_stdpath();
}
