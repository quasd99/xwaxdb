#include "settings.h"
#include "libquasd/settingsfile.h"

#include <glibmm-2.4/glibmm.h>
#include <iostream>
#include <fstream>
#include <map>

using namespace quasd;
 
settings::settings() {}
settings::settings(const std::string& p, const std::string& f)
: path_application(p),
	filename_settings(f)
{ 
}

std::string
settings::get_path_userhome()
{
    return Glib::get_home_dir();
}

bool
settings::parse_file_settings()
{
	map_settings = quasd::settingsfile::parse(get_filepath_settings());
	if ( map_settings.size() > 0 )
	{
		for (const auto M : map_settings)
		{
			set_sections.insert(M.first); // save all sections to v_sections
		}
    return true;
	}
	
	return false;
}

std::string
settings::get_filepath_settings()
{
  return path_application + '/' + filename_settings;
}

std::string
settings::get_section_value(const std::string& section, const std::string& key)
{
  auto it_r = map_settings.equal_range(section);      // get section
  for ( auto it = it_r.first; it != it_r.second; it++ ) // iterate section
  {
    // it.second = std::pair<std::string, std::string>
    if ( it->second.first == key ) // find key
    {
        return it->second.second;  // return value
    }
  }

  return std::string{""}; // failed
}

std::set<std::string>
settings::get_sections()
{   
	return set_sections;
}

std::string
settings::get_value(const std::string& key)
{
  // if no sections is used in settings-file
  // internal the section named 'root' must be used
  auto it_r = map_settings.equal_range("root");       // get section
  for ( auto it = it_r.first; it != it_r.second; it++ ) // iterate section
  {
    // it.second = std::pair<std::string, std::string>
    if ( it->second.first == key ) // find key
    {
        return it->second.second;  // return value
    }
  }

  return std::string{""}; // failed    
}

std::vector<std::pair<std::string, std::string>>
settings::get_section_data(const std::string& section)
{
  std::vector<std::pair<std::string, std::string>> v_ret;
  auto it_r = map_settings.equal_range(section);       // get section
  for ( auto it = it_r.first; it != it_r.second; it++ ) // iterate section
  {
    // it.second = std::pair<std::string, std::string>
    v_ret.push_back(it->second);
  }

  return std::move(v_ret);
}
