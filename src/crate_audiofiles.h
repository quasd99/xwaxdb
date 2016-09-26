#ifndef CRATE_AUDIOFILES_H
#define CRATE_AUDIOFILES_H

#include "crate.h"
#include "libquasd/tagtool.h"

#include <vector>

namespace xwaxdb
{

class crate_audiofiles : public crate
{
  std::vector<std::string> v_ext; // supported audiofile-suffixes
public:
  crate_audiofiles();    
  bool scan();
private:
  /**
   * @brief Returns all directories in path
   * @param path
   * @return std::vector<std::string> directory full path
   */
  std::vector<std::string> get_crate_directories(const std::string& path);
};

} // namespace xwaxdb
 
#endif // CRATE_AUDIOFILES_H
