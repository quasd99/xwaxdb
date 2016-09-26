#ifndef CRATE_PLS_H
#define CRATE_PLS_H

#include "crate.h"

namespace xwaxdb
{

class crate_pls : public crate
{
public:
    crate_pls();   
    bool scan();
private:
};

} // namespace xwaxdb
 
#endif // CRATE_PLS_H
