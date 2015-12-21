#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <cassert>
#include <sstream>
#include <iostream>
#include <cstdlib>

using namespace std;

struct Arg
{
    static string pfile;
    static string wfile;
    static int t;
    static int k;
    static double dweight;
    static int nfun;
    static int nmv;
    static string pro;

    static void readArgs(int argc, char* argv[]);
    static void outputArgs();
};

struct Record
{
    Record();

    static long long skyband_cnt;
    static long long rtopk_cnt;
    static long long topk_cnt;
    static long long candidate_cnt;

    static double rtopk_time;
    static double topk_time;
    static double mul_time;
    static double overhead_time;
    static double pick_time;
    static double pick_mh_time;

    static long long io_bbs_ptree_cnt;
    static long long io_bbra_wtree_cnt;
    static long long io_bbtk_wtree_cnt;
    static long long io_mul_wtree_cnt;

    static void init();
    static void outputrecord();
};

#endif // RECORD_H
