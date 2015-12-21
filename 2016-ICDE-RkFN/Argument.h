#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

struct Argument
{
    // argument
    static string dataset;
    static string gen;
    static string pick;
    static string rstq;
    static string rstq1;
    static string vdca;
    static string vara;
    static string tpru;
    static string bf;
    static string chfc;
    static string dccr;
    static size_t k;
    static size_t numQueries;
    static size_t numFacilities;
    static size_t numClients;

    static void readArguments(int argc, char* argv[]);
    static void printArguments();

    // file name
    static string getFacilityFileName();
    static string getClientFileName();
    static string getQueryFileName();

    static string getResultFileName();

    // statistic
    static size_t empty;

    static size_t Schfc;
    static size_t Sbbs;
    static size_t Scr;
    static size_t Sdccr;

    static size_t NFchfc;
    static size_t NFband;
    static size_t NFskirt;

    static long Tbf;

    static long Tch;
    static long Tfvc;
    static long Trq;
    static long Tchfc;

    static long Tbbs;
    static long Tskyrider;
    static long Tcr;
    static long Tcrr;
    static long Tsk;
    static long Tfv;
    static long Tdccr;

    static size_t FIOchfc;
    static size_t CIOchfc;
    static size_t FIOdccr;
    static size_t CIOdccr;

    static size_t cntRp;
    static size_t cntDC;
    static double expDCperR;
    static double expDCperR2;

    static double cumuAR;
    static size_t cntCM;
    static double expARperR;

    static size_t cntPCR;
    static size_t cntPCRR;
    static size_t cntPB;
    static size_t cntP;
    static size_t cntIS;
    static size_t cntRST;

    // experiment
    static void outputExperiment();
    static void clear();
};

#endif // ARGUMENT_H
