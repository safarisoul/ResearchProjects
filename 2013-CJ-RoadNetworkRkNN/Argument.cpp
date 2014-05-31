#include "Argument.h"

Amount_t Argument::numObjects = 100000;
Amount_t Argument::numFacilities = 8000;
Amount_t Argument::movePercent = 100;
Distance_t Argument::speedkm = 80;
Distance_t Argument::speed = 80.0 / (3600.0 * KM_L2DIS_RATIO);
Amount_t Argument::timeStamp = 300;
Amount_t Argument::numQueries = 100;
Amount_t Argument::k = 16;
Amount_t Argument::numUpdates = 1000;
string Argument::network = "california";
Index_t Argument::category = 100;
const Index_t Argument::uniCategory = 100;
string Argument::function = "m";
string Argument::version = "0";

double Argument::EPS = 1e-10;
double Argument::INF = 1.7e300;

unsigned int Argument::queryPointID = 0;

INT64 Argument::zoneNodes = 0;
INT64 Argument::zoneFullEdges = 0;
INT64 Argument::zonePartialEdges = 0;
INT64 Argument::touchedNodes = 0;
INT64 Argument::touchedEdges = 0;
Distance_t Argument::zoneLength = 0;

INT64 Argument::bZoneNodes = 0;
INT64 Argument::bZoneFullEdges = 0;
INT64 Argument::bZonePartialEdges = 0;
Distance_t Argument::bZoneLength = 0;
INT64 Argument::updateCounts = 0;

INT64 Argument::calculation = 0;
INT64 Argument::monitor = 0;
INT64 Argument::update = 0;

void
Argument::readArguments(int argc, char* argv[])
{
    for(int iArg=1; iArg+1<argc; iArg+=2)
    {
        string argName = string(argv[iArg]);
        istringstream argstream(argv[iArg+1]);
        if(argName == "nobj")
            argstream >> numObjects;
        else if(argName == "nfac")
            argstream >> numFacilities;
        else if(argName == "movp")
            argstream >> movePercent;
        else if(argName == "sped")
        {
            argstream >> speedkm;
            speed = speedkm / (3600 * 88.0);
        }
        else if(argName == "tims")
            argstream >> timeStamp;
        else if(argName == "nque")
            argstream >> numQueries;
        else if(argName == "k")
            argstream >> k;
        else if(argName == "netw")
            argstream >> network;
        else if(argName == "cate")
            argstream >> category;
        else if(argName == "func")
            argstream >> function;
        else if(argName == "vers")
            argstream >> version;
        else if(argName == "nupd")
            argstream >> numUpdates;
        else
        {
            cout << "unrecognized argument : " << argName << endl;
            exit(EXIT_SUCCESS);
        }
    }
}

void
Argument::printArguments()
{
    cout << endl;
    cout << "numObjects : " << numObjects << endl;
    cout << "numFacilities : " << numFacilities << endl;
    cout << "movePercent : " << movePercent << endl;
    cout << "speedkm : " << speedkm << endl;
    cout << "speed : " << speed << endl;
    cout << "timeStamp : " << timeStamp << endl;
    cout << "numQueries : " << numQueries << endl;
    cout << "k : " << k << endl;
    cout << "numUpdates : " << numUpdates << endl;
    cout << "network : " << network << endl;
    cout << "category : " << category << endl;
    cout << "function : " << function << endl;
    cout << "version : " << version << endl;
}

string Argument::resultDirectory = "result/";
/*
string
Argument::getZonePrefix()
{
    stringstream sstream;
    sstream << resultDirectory << network << "/zoneNetwork/";
    sstream << network << "_";
    sstream << k << ".txt";
    return sstream.str();
}

string
Argument::getZonePrefixUpdateBefore()
{
    stringstream sstream;
    sstream << resultDirectory << network << "/zoneNetwork/";
    sstream << network << "_";
    sstream << k << "_updateBefore.txt";
    return sstream.str();
}

string
Argument::getZonePrefixUpdate()
{
    stringstream sstream;
    sstream << resultDirectory << network << "/zoneNetwork/";
    sstream << network << "_";
    sstream << k << "_update.txt";
    return sstream.str();
}
*/
string
Argument::getObjectFileName()
{
    stringstream sstream;
    sstream << resultDirectory << network << "/object/";
    sstream << network << "_";
    sstream << "no" << numObjects;
    sstream << "_mp" << movePercent;
    sstream << "_s" << speedkm;
    sstream << "_t" << timeStamp << ".txt";
    return sstream.str();
}

string
Argument::getQueryFileName()
{
    stringstream sstream;
    sstream << resultDirectory << network << "/query/";
    sstream << network << "_";
    sstream << category;
    sstream << "_nq" << numQueries;
    sstream << "_nf" << numFacilities;
    sstream << "_k" << k << ".txt";
    return sstream.str();
}

string
Argument::getUpdateFileName()
{
    stringstream sstream;
    sstream << resultDirectory << network << "/update/";
    sstream << network << "_";
    sstream << category;
    sstream << "_nq" << numQueries;
    sstream << "_nf" << numFacilities;
    sstream << "_k" << k << ".txt";
    return sstream.str();
}

void
Argument::outputExperiment()
{
    stringstream sstream;
    sstream << resultDirectory << network << "/experiment/";
    sstream << network << "_";
    sstream << "nq" << numQueries << "_nf" << numFacilities << "_k" << k << "_" << category << "_";
    sstream << "no" << numObjects << "_mp" << movePercent << "_s" << speedkm << "_t" << timeStamp << "_";
    sstream << function << "_" << version << ".txt";
    string experimentFileName = sstream.str();
    ofstream experimentFile;
    experimentFile.open(experimentFileName.c_str());
    if (experimentFile.is_open())
    {
        experimentFile << "total zone nodes : " << zoneNodes << endl;
        experimentFile << "total zone full edges : " << zoneFullEdges << endl;
        experimentFile << "total zone partial edges : " << zonePartialEdges << endl;
        experimentFile << "total touched nodes : " << touchedNodes << endl;
        experimentFile << "total touched edges : " << touchedEdges << endl;
        experimentFile << "average zone nodes : " << (zoneNodes / numQueries) << endl;
        experimentFile << "average zone length (l2dis) : " << (zoneLength / numQueries) << endl;
        experimentFile << "average zone length (km) : " << (zoneLength / numQueries * KM_L2DIS_RATIO) << endl;

        experimentFile << "total boundary zone nodes : " << bZoneNodes << endl;
        experimentFile << "total boundary zone full edges : " << bZoneFullEdges << endl;
        experimentFile << "total boundary zone partial edges : " << bZonePartialEdges << endl;
        experimentFile << "average boundary zone nodes : " << (bZoneNodes / numQueries) << endl;
        experimentFile << "average boundary zone length (l2dis) : " << (bZoneLength / numQueries) << endl;
        experimentFile << "average boundary zone length (km) : " << (bZoneLength / numQueries * KM_L2DIS_RATIO) << endl;
        experimentFile << "update counts : " << updateCounts << endl;

        experimentFile << "calculation : " << ((double)calculation / CLOCKS_PER_SEC) << endl;
        experimentFile << "monitor : " << ((double)monitor / CLOCKS_PER_SEC) << endl;
        experimentFile << "update : " << ((double)update / CLOCKS_PER_SEC) << endl;
        experimentFile.close();
    }
    else cout << "Unable to open file : " << experimentFileName << endl;
}
