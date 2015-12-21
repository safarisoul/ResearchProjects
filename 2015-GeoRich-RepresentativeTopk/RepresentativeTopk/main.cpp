#include <sys/time.h>

#include "data.h"
#include "record.h"
#include "repp_rtk.h"
#include "repp_tk.h"
#include "repp_mul.h"

using namespace std;

int main(int argc, char* argv[])
{
    Arg::readArgs(argc, argv);
    Data::initData(Arg::pfile, Arg::wfile);
    MinHash::init(Arg::nfun);
    KMV::setsize(Arg::nmv);

    set<int> rst;
    Score s;
    if(Arg::pro == "rand")
        ;
    else if(Arg::pro == "rtk")
        REPP_RTK::repp(rst, s, *Data::ptree, *Data::wtree, Arg::k, Arg::t, Arg::dweight);
    else if(Arg::pro == "rtk_mh")
        REPP_RTK::repp_mh(rst, s, *Data::ptree, *Data::wtree, Arg::k, Arg::t, Arg::dweight);
    else if(Arg::pro == "rtk_mh2")
        REPP_RTK::repp_mh2(rst, s, *Data::ptree, *Data::wtree, Arg::k, Arg::t, Arg::dweight);
    else if(Arg::pro == "tk")
        REPP_TK::repp(rst, s, *Data::ptree, *Data::wtree, Arg::k, Arg::t, Arg::dweight);
    else if(Arg::pro == "tk_mh")
        REPP_TK::repp_mh(rst, s, *Data::ptree, *Data::wtree, Arg::k, Arg::t, Arg::dweight);
    else if(Arg::pro == "mul")
        REPP_MUL::repp(rst, s, *Data::ptree, *Data::wtree, Arg::k, Arg::t, Arg::dweight);
    else if(Arg::pro == "mul_mh")
        REPP_MUL::repp_mh(rst, s, *Data::ptree, *Data::wtree, Arg::k, Arg::t, Arg::dweight);

    Arg::outputArgs();
    if(Arg::pro != "rand")
        s.output();
    Record::outputrecord();

    return 0;
}
