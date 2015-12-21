#include "record.h"

string Arg::pfile = "";
string Arg::wfile = "";
int Arg::t = 10;
int Arg::k = 10;
double Arg::dweight = 0.5;
int Arg::nfun = 100;
int Arg::nmv = 17;
string Arg::pro = "rtk";

void Arg::readArgs(int argc, char* argv[])
{
    for(int ia = 1; ia + 1 < argc; ia += 2)
    {
        string argName = string(argv[ia]);
        istringstream argstream(argv[ia+1]);

        if(argName == "pfile")
            argstream >> pfile;
        else if(argName == "wfile")
            argstream >> wfile;
        else if(argName == "t")
            argstream >> t;
        else if(argName == "k")
            argstream >> k;
        else if(argName == "dweight")
            argstream >> dweight;
        else if(argName == "nfun")
            argstream >> nfun;
        else if(argName == "nmv")
            argstream >> nmv;
        else if(argName == "pro")
            argstream >> pro;
        else
        {
            cout << "void Arg::readArgs(int argc, char* argv[]) error : " << argName << endl;
            assert(false);
        }
    }
}

void Arg::outputArgs()
{
    cout << "pfile : " << pfile << endl;
    cout << "wfile : " << wfile << endl;
    cout << "t : " << t << endl;
    cout << "k : " << k << endl;
    cout << "dweight : " << dweight << endl;
    cout << "nfun : " << nfun << endl;
    cout << "nmv : " << nmv << endl;
    cout << "pro : " << pro << endl;
}

long long Record::skyband_cnt = 0;
long long Record::rtopk_cnt = 0;
long long Record::topk_cnt = 0;
long long Record::candidate_cnt = 0;

double Record::rtopk_time = 0;
double Record::topk_time = 0;
double Record::mul_time = 0;
double Record::overhead_time = 0;
double Record::pick_time = 0;
double Record::pick_mh_time = 0;

long long Record::io_bbs_ptree_cnt = 0;
long long Record::io_bbra_wtree_cnt = 0;
long long Record::io_bbtk_wtree_cnt = 0;
long long Record::io_mul_wtree_cnt = 0;

void Record::init()
{
    skyband_cnt = 0;
    rtopk_cnt = 0;
    topk_cnt = 0;
    candidate_cnt = 0;

    rtopk_time = 0;
    topk_time = 0;
    mul_time = 0;
    overhead_time = 0;
    pick_time = 0;
    pick_mh_time = 0;

    io_bbs_ptree_cnt = 0;
    io_bbra_wtree_cnt = 0;
    io_bbtk_wtree_cnt = 0;
    io_mul_wtree_cnt = 0;
}

void Record::outputrecord()
{
    cout << endl;
    cout << "skyband size : " << skyband_cnt << endl;
    cout << "call on reverse top k : " << rtopk_cnt << endl;
    cout << "call on top k : " << topk_cnt << endl;
    cout << "candidate size for pick : " << candidate_cnt << endl;
    cout << endl;
    cout << "time on reverse top k (sec) : " << rtopk_time << endl;
    cout << "time on top k (sec) : " << topk_time << endl;
    cout << "time on multiplication (sec) : " << mul_time << endl;
    cout << "time on calculate minhash and kmv (sec) : " << overhead_time << endl;
    cout << "time on pick by set operation (sec) : " << pick_time << endl;
    cout << "time on pick by minhash and kmv (sec) : " << pick_mh_time << endl;
    cout << endl;
    cout << "bbs ptree io : " << io_bbs_ptree_cnt << endl;
    cout << "bbra wtree io : " << io_bbra_wtree_cnt << endl;
    cout << "bbtk wtree io : " << io_bbtk_wtree_cnt << endl;
    cout << "multi wtree io : " << io_mul_wtree_cnt << endl;
    cout << endl;
    cout << "total io : " << ( io_bbs_ptree_cnt + io_bbra_wtree_cnt + io_bbtk_wtree_cnt + io_mul_wtree_cnt ) << endl;
    cout << endl;
}
