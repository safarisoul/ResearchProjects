#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#define KM_L2DIS_RATIO 88

using namespace std;

/**********************************************************************
 * Basic data types
 **********************************************************************/

typedef unsigned int Amount_t;
typedef unsigned int Index_t;
typedef double Distance_t;
typedef long INT64;

struct Argument
{
    Argument() { };

    /// command line arguments
    static Amount_t numObjects; // ..... number of moving objects
    static Amount_t numFacilities; // .. number of facilities
    static Amount_t movePercent; // .... how many out of 100 objects are continuously move (not static)
    static Distance_t speedkm; // ...... how many km/h do objects move
    static Distance_t speed; // ........ how much do objects change location per each timestamp
    static Amount_t timeStamp; // ...... how many timestamps to perform monitoring
    static Amount_t numQueries; // ..... number of queries
    static Amount_t k; // .............. k in RkNN
    static Amount_t numUpdates; // ..... number of updates
    static string network; // .......... which road network is used
    static Index_t category; // ........ the category of facilities
    static const Index_t uniCategory; // the category means categories are not distinguished
    static string function; // ......... which function to perform
    static string version; // .......... which version of algorithm to be used

    /// system parameter
    static double EPS;
    static double INF;

    /// for experiments
    static unsigned int queryPointID; // .. for test case when numQueries = 1

    static INT64 zoneNodes;
    static INT64 zoneFullEdges;
    static INT64 zonePartialEdges;
    static INT64 touchedNodes;
    static INT64 touchedEdges;
    static Distance_t zoneLength;

    static INT64 bZoneNodes;
    static INT64 bZoneFullEdges;
    static INT64 bZonePartialEdges;
    static Distance_t bZoneLength;
    static INT64 updateCounts;

    static INT64 calculation;
    static INT64 monitor;
    static INT64 update;

    /// functions
    static void readArguments(int argc, char* argv[]);
    static void printArguments();
    inline static bool isFacility(Index_t cid) { return (category == uniCategory || category == cid) ? true : false; };

    /// for output files
    static string resultDirectory;
/*
    static string getZonePrefix();
    static string getZonePrefixUpdateBefore();
    static string getZonePrefixUpdate();
*/
    static string getObjectFileName();
    static string getQueryFileName();
    static string getUpdateFileName();

    static void outputExperiment();
};

#endif // ARGUMENT_H
