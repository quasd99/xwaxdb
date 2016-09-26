#include "settingsfile.h"

#include <fstream>
#include <iostream>

using namespace quasd;

settingsfile::settingsfile(const std::string& p) 
: str_path(p)
{
}

std::set<std::string>
settingsfile::get_sectionlist()
{
  std::set<std::string> sRet;
  for (const auto M : map_settings)
  {
    sRet.insert(M.first);
  }
  return sRet;
}

std::map<std::string, std::string>
settingsfile::get_section_data(const std::string& section)
{
  std::map<std::string, std::string> mRet;
  auto it_r = map_settings.equal_range(section); // get section iter
  for ( auto it = it_r.first; it != it_r.second; it++ ) // iterate section
  {
    mRet.insert(std::make_pair(it->second.first, it->second.second));
  }
  return std::move(mRet);
}


bool
settingsfile::parse() 
{   
	return parse(str_path, map_settings);
}

std::multimap<std::string, std::pair<std::string, std::string> >
settingsfile::parse(const std::string& path)
{
	std::multimap<std::string, std::pair<std::string, std::string> > mRet;
	parse(path, mRet);
	return std::move(mRet);
}

bool
settingsfile::parse(const std::string& path,
										std::multimap<std::string, std::pair<std::string, std::string> >& m)
{
  std::ifstream f_settings(path);
  if ( !f_settings )
  {
      std::cerr << "ERR:" << __PRETTY_FUNCTION__
              << ":cannot open:"
              << path
              << std::endl;
      return false;
  }

  std::string line;
  std::string currentSection = "root";  // must be used as section-name
                                         // if no sections are defined.
  while (getline(f_settings, line))
  {
		if ( line.size() < 1 )
		{
			continue; // skip empty line
		}
		else if ( line.at(0) == '#' || line.at(0) == ';' )
		{
			//TODO comment parsing
			continue; // skip comment line
		}
		else if ( line.at(0) == '[' )
		{
			currentSection = parse_section_line(line);
			continue; // new section
		}
		else
		{
			// get key,value
			auto pair = parse_key_val_line(line);
			if ( pair.first.size() > 0 ) // add only if key was found
					m.insert(std::make_pair(currentSection, pair));
		}
  }

  f_settings.close();

  return true;	
}

bool
settingsfile::write()
{
	std::ofstream F(str_path);
	if ( !F )
	{
			std::cerr << "ERR:" << __PRETTY_FUNCTION__
							<< ":cannot open/create:"
							<< str_path
							<< std::endl;
			return false;
	}

	//TODO comment handling
	std::string currentSection = "";
	for (auto M : map_settings)
	{
			if ( currentSection != M.first && currentSection != "root" )
			{
					// new section
					currentSection = M.first;
					F << '[' << currentSection << ']' << std::endl;
			}
			F << M.second.first << '=' << M.second.second << std::endl;
	}

	F.close();

	return true;	
}


bool
settingsfile::write(const std::string path,
										std::multimap<std::string, std::pair<std::string, std::string> >& map)
{
//	std::ofstream F(path);
//	if ( !F )
//	{
//			std::cerr << "ERR:" << __PRETTY_FUNCTION__
//							<< ":cannot open/create:"
//							<< path
//							<< std::endl;
//			return false;
//	}
//
//	//TODO comment handling
//	std::string currentSection = "";
//	for (auto M : map)
//	{
//			if ( currentSection != M.first && currentSection != "root" )
//			{
//					// new section
//					currentSection = M.first;
//					F << '[' << currentSection << ']' << std::endl;
//			}
//			F << M.second.first << '=' << M.second.second << std::endl;
//	}
//
//	F.close();

	return true;	
}



std::pair<std::string, std::string>
settingsfile::parse_key_val_line(const std::string& s)
{
    std::size_t pos;
    pos = s.find_first_of('=');
    if ( pos != std::string::npos )
    {
        return std::make_pair(s.substr(0,pos), s.substr(pos+1)); // key, value
    }
    
    return std::make_pair("", ""); // failed
}

std::string
settingsfile::parse_section_line(const std::string& s) 
{
    std::size_t pos;
    pos = s.find_first_of(']');
    if ( pos != std::string::npos )
    {
        return s.substr(1, (pos-1));
    }
    
    return std::string{""}; // failed
}
