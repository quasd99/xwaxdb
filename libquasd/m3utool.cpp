#include "m3utool.h"
#include "file.h"

#include <fstream>
#include <glibmm-2.4/glibmm/uriutils.h>

using namespace quasd;
 
m3utool::m3utool()
{
}

//bool
//m3utool::parse_write(const std::string& path, std::vector<s_m3u_trackdata>& v_td)
//{
//    std::ifstream f_m3u;
//    f_m3u.open(path);
//    if ( !f_m3u )
//    {
//        std::cerr << "ERR:" << __PRETTY_FUNCTION__
//                << ":cannot access m3u-file:"
//                << path
//                << std::endl;
//        return false;
//    }
//        
//    std::string line;
//    int count = 0;
//    while (getline(f_m3u, line))
//    {
//        count++;
//        if ( line.empty() )
//        {
//            continue;
//        }
//        
//        s_m3u_trackdata t;
//        if ( line.substr(0,8) == "#EXTINF:" )
//        {
//            // read EXTINF
//            process_extinf_line(line, t);
//            
//            // read path
//            getline(f_m3u, line);
//            t.path = line;
//            
//            v_td.push_back(t);
//        }
//    }
//    
////    std::cout << "Info:" << __PRETTY_FUNCTION__ 
////            << "m3u-file:" << path << ": " << count << " tracks"
////            << std::endl;
//
//    return true;
//}

bool
m3utool::parse(const std::string& path, bool make_abs_path, bool ignore_url)
{	
	std::ifstream F;
	F.open(path);
	if ( !F )
	{
			std::cerr << "ERR:" << __PRETTY_FUNCTION__
							<< ":cannot access m3u-file:"
							<< path
							<< std::endl;
			return false;
	}
	
// init
	map_data.clear();
	b_make_abs_path = make_abs_path;
	b_ignore_url = ignore_url;
	path_m3u = quasd::file::get_path(path);
	std::unordered_map<std::string, std::string> mEntry;
	int Count = 0;
	b_is_extm3u = false;
	std::string Line;
	
// determine m3u-type	
	getline(F, Line); // first Line
	if ( Line == "#EXTM3U" )
	{ // Extended M3U
		b_is_extm3u = true;
	}
	else
	{ // Simple M3U
		std::string path = get_path(Line);
		if ( !path.empty() )
		{
			mEntry.insert(std::make_pair("path", path));
			map_data.insert(std::make_pair(Count, mEntry));
			Count++;			
		}
	}

// parse m3u
	while (getline(F, Line))
	{
		if ( Line.empty() ) continue;
		
		if ( b_is_extm3u )
		{ // extended M3U
			if ( Line.substr(0,8) == "#EXTINF:" )
			{
				mEntry = parse_extinf_line(Line); // parse EXTINF-line
				getline(F, Line); // get the next line = path
				if ( Line.empty() )
				{
					std::cerr << "ERR:" << __PRETTY_FUNCTION__
						<< ":#EXTM3U-Error: empty line after #EXTINF:"
						<< std::endl;
					continue;
				}
			}
			else
			{ // probably corrupt EXTM3U-file
				continue;
			}
		}
		
		std::string path = get_path(Line);
		mEntry.insert(std::make_pair("path", path));
		map_data.insert(std::make_pair(Count, std::move(mEntry)));
		Count++;
	}

	std::cout << "Info:" << __PRETTY_FUNCTION__
	  << ":" << path << ":entries:" << map_data.size() << std::endl;
	
	return true;
}

std::vector<std::string>
m3utool::parse_simple(const std::string& path, bool make_abs_path, bool ignore_url)
{
	std::vector<std::string> vRet;
	
	std::ifstream F;
	F.open(path);
	if ( !F )
	{
			std::cerr << "ERR:" << __PRETTY_FUNCTION__
							<< ":cannot access m3u-file:"
							<< path
							<< std::endl;
			return vRet;
	}
	
	std::string m3uPath = quasd::file::get_path(path);
	std::string Line;
	while (getline(F, Line))
	{
		if ( Line.size() < 1 )
		{
			continue;
		}
		else if ( Line.at(0) == '#' )
		{
			continue;
		}

		if ( ( !make_abs_path && !ignore_url) || Line.at(0) == '/' )
		{
			vRet.push_back(Line);
			continue;
		}
		else if ( !is_url(Line) )
		{ // relative path
			if ( !make_abs_path )
			{
				vRet.push_back(Line);
				continue;				
			}
			else
			{
				vRet.push_back(get_abs_path(m3uPath, Line)); // absolute path
				continue;
			}
		}
		else
		{ // url
			if ( !ignore_url )
			{
				vRet.push_back(Line);
				continue;				
			}	// else ignore url
		}
	}
	return vRet;
}

bool
m3utool::is_url(const std::string& line)
{
	std::string Scheme = Glib::uri_parse_scheme(line);
	if ( !Scheme.empty() )
	{
		return true;
	}
	return false;
}

std::unordered_map<std::string, std::string>
m3utool::parse_extinf_line(const std::string& line)
{
	std::unordered_map<std::string, std::string> mRet;
	// length
	std::size_t Pos1 = line.find_first_of(',');
	if ( Pos1 != std::string::npos )
	{
		mRet["length"] = line.substr(8, (Pos1 - 8));
	}

	// artist + title
	std::size_t Pos2 = line.find(" - ", Pos1);
	if ( Pos2 != std::string::npos )
	{
		mRet["artist"] = line.substr((Pos1 + 1), (Pos2 - Pos1 -1));
		mRet["title"]  = line.substr((Pos2 + 3));
	}
	
	return std::move(mRet);
}

std::string
m3utool::get_abs_path(const std::string& m3upath, const std::string& relpath)
{ //TODO: MS
	return m3upath + '/' + relpath;
}

std::string
m3utool::get_path(const std::string& path)
{
	if ( path.at(0) != '/' )
	{ // absolute path, nothing to do
		return path;
	}
	
	if ( b_make_abs_path && !is_url(path) )
	{ // convert relative to absolute path
		return path_m3u + '/' + path;
	}
	
	if ( b_ignore_url && is_url(path) )
	{ // ignore url
		return std::string{""};
	}
	
	return path; // it's a url
}

