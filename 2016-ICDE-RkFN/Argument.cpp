#include "Argument.h"

string Argument::dataset = "uni"; // "real" "uni" "unic"
string Argument::gen = "n";
string Argument::pick = "n";
string Argument::rstq = "n";
string Argument::rstq1 = "n";
string Argument::vdca = "n";
string Argument::vara = "n";
string Argument::tpru = "n";
string Argument::bf = "n";
string Argument::chfc = "n";
string Argument::dccr = "n";
size_t Argument::numQueries = 1000;
size_t Argument::k = 10;
size_t Argument::numFacilities = 100000;
size_t Argument::numClients = 100000;

void Argument::readArguments(int argc, char* argv[])
{
    for(int iArg = 1; iArg + 1 < argc; iArg += 2)
    {
        string argName = string(argv[iArg]);
        istringstream argstream(argv[iArg+1]);

        if(argName == "data")
            argstream >> dataset;
        else if(argName == "gen")
            argstream >> gen;
        else if(argName == "pick")
            argstream >> pick;
        else if(argName == "rstq")
            argstream >> rstq;
        else if(argName == "rstq1")
            argstream >> rstq1;
        else if(argName == "vdca")
            argstream >> vdca;
        else if(argName == "vara")
            argstream >> vara;
        else if(argName == "tpru")
            argstream >> tpru;
        else if(argName == "bf")
            argstream >> bf;
        else if(argName == "chfc")
            argstream >> chfc;
        else if(argName == "dccr")
            argstream >> dccr;
        else if(argName == "nq")
            argstream >> numQueries;
        else if(argName == "k")
            argstream >> k;
        else if(argName == "nf")
            argstream >> numFacilities;
        else if(argName == "nc")
            argstream >> numClients;
        else
            assert(false);
    }
}

void Argument::printArguments()
{
    cout << endl;
    cout << "dataset: " << dataset << endl;
    cout << "function:";
    cout << (gen == "y" ? " gen" : "");
    cout << (pick == "y" ? " pick" : "");
    cout << (rstq == "y" ? " rstq" : "");
    cout << (rstq1 == "y" ? " rstq1" : "");
    cout << (vdca == "y" ? " vdca" : "");
    cout << (vara == "y" ? " vara" : "");
    cout << (tpru == "y" ? " tpru" : "");
    cout << (bf == "y" ? " bf" : "");
    cout << (chfc == "y" ? " chfc" : "");
    cout << (dccr == "y" ? " dccr" : "");
    cout << endl;
    cout << "k: " << k << endl;
    cout << "numQueries: " << numQueries << endl;
    cout << "numFacilities: " << numFacilities << endl;
    cout << "numClients: " << numClients << endl;
}

string Argument::getFacilityFileName()
{
    stringstream sstream;
    sstream << "data/";
    if(rstq == "y")
        sstream << dataset << "-rst/";
    else if(rstq1 == "y")
        sstream << dataset << "-rst1/";
    else
        sstream << dataset << "/";
    sstream << "nf" << numFacilities;
    sstream << "_nc" << numClients;
    sstream << "_nq" << numQueries;
    sstream << "_k" << k << "_facilities.txt";
    return sstream.str();
}

string Argument::getClientFileName()
{
    stringstream sstream;
    sstream << "data/";
    if(rstq == "y")
        sstream << dataset << "-rst/";
    else if(rstq1 == "y")
        sstream << dataset << "-rst1/";
    else
        sstream << dataset << "/";
    sstream << "nf" << numFacilities;
    sstream << "_nc" << numClients;
    sstream << "_nq" << numQueries;
    sstream << "_k" << k << "_clients.txt";
    return sstream.str();
}

string Argument::getQueryFileName()
{
    stringstream sstream;
    sstream << "data/";
    if(rstq == "y")
        sstream << dataset << "-rst/";
    else if(rstq1 == "y")
        sstream << dataset << "-rst1/";
    else
        sstream << dataset << "/";
    sstream << "nf" << numFacilities;
    sstream << "_nc" << numClients;
    sstream << "_nq" << numQueries;
    sstream << "_k" << k << "_queries.txt";
    return sstream.str();
}

string Argument::getResultFileName()
{
    stringstream sstream;
    sstream << "result/";
    if(tpru == "y")
    {
        if(rstq1 == "y")
            sstream << dataset << "-tpru1/";
        else
            sstream << dataset << "-tpru/";
    }
    else if(rstq == "y")
        sstream << dataset << "-rst/";
    else if(rstq1 == "y")
        sstream << dataset << "-rst1/";
    else
        sstream << dataset << "/";
    sstream << "nf" << numFacilities;
    sstream << "_nc" << numClients;
    sstream << "_nq" << numQueries;
    sstream << "_k" << k << ".txt";
    return sstream.str();
}

size_t Argument::empty = 0;

size_t Argument::Schfc = 0;
size_t Argument::Sbbs = 0;
size_t Argument::Scr = 0;
size_t Argument::Sdccr = 0;

size_t Argument::NFchfc = 0;
size_t Argument::NFband = 0;
size_t Argument::NFskirt = 0;

long Argument::Tbf = 0;

long Argument::Tch = 0;
long Argument::Tfvc = 0;
long Argument::Trq = 0;
long Argument::Tchfc = 0;

long Argument::Tbbs = 0;
long Argument::Tskyrider = 0;
long Argument::Tcr = 0;
long Argument::Tcrr = 0;
long Argument::Tsk = 0;
long Argument::Tfv = 0;
long Argument::Tdccr = 0;

size_t Argument::FIOchfc = 0;
size_t Argument::CIOchfc = 0;
size_t Argument::FIOdccr = 0;
size_t Argument::CIOdccr = 0;

size_t Argument::cntRp = 0;
size_t Argument::cntDC = 0;
double Argument::expDCperR = 0;
double Argument::expDCperR2 = 0;

double Argument::cumuAR = 0;
size_t Argument::cntCM = 0;
double Argument::expARperR = 0;

size_t Argument::cntPCR = 0;
size_t Argument::cntPCRR = 0;
size_t Argument::cntPB = 0;
size_t Argument::cntP = 0;
size_t Argument::cntIS = 0;
size_t Argument::cntRST = 0;

void Argument::outputExperiment()
{
    string experimentFileName = getResultFileName();
    ofstream experimentFile;
    experimentFile.open(experimentFileName.c_str());
    if (experimentFile.is_open())
    {
        experimentFile << "cases with empty result : " << empty << endl;

        experimentFile << endl;

        experimentFile << "cases chfc stops early : " << Schfc << endl;
        experimentFile << "cases bbs stops early : " << Sbbs << endl;
        experimentFile << "cases cr stops early : " << Scr << endl;
        experimentFile << "cases dccr stops early : " << Sdccr << endl;

        experimentFile << endl;

        experimentFile << "average nf on/out of k-th convex hull : " << ((double)NFchfc / numQueries) << endl;
        if(numQueries - Sdccr == 0)
            experimentFile << "average nf in k sky band : " << ((double)NFband) << endl;
        else
            experimentFile << "average nf in k sky band : " << ((double)NFband / (numQueries - Sdccr)) << endl;
        if(numQueries - Sdccr == 0)
            experimentFile << "average nf on/out of k-th depth contour : " << ((double)NFskirt) << endl;
        else
            experimentFile << "average nf on/out of k-th depth contour : " << ((double)NFskirt / (numQueries - Sdccr)) << endl;

        experimentFile << endl;

        experimentFile << "average brute force time : " << (Tbf / numQueries) * 1e-6 << endl;

        experimentFile << endl;

        experimentFile << "average convex hull time : " << (Tch / numQueries) * 1e-6 << endl;
        experimentFile << "average fvc time : " << (Tfvc / numQueries) * 1e-6 << endl;
        experimentFile << "average range query time : " << (Trq / numQueries) * 1e-6 << endl;
        experimentFile << "average chfc time : " << (Tchfc / numQueries) * 1e-6 << endl;

        experimentFile << endl;

        experimentFile << "average bbs time : " << (Tbbs / numQueries) * 1e-6 << endl;
        experimentFile << "average skyrider time : " << (Tskyrider / numQueries) * 1e-6 << endl;
        experimentFile << "average candidate region time : " << (Tcr / numQueries) * 1e-6 << endl;
        experimentFile << "average candidate range time : " << (Tcrr / numQueries) * 1e-6 << endl;
        experimentFile << "average skirt time : " << (Tsk / numQueries) * 1e-6 << endl;
        experimentFile << "average filtering and verification time : " << (Tfv / numQueries) * 1e-6 << endl;
        experimentFile << "average dccr time : " << (Tdccr / numQueries) * 1e-6 << endl;

        experimentFile << endl;

        experimentFile << "average chfc facility io : " << ((double)FIOchfc / numQueries) << endl;
        experimentFile << "average chfc client io : " << ((double)CIOchfc / numQueries) << endl;

        experimentFile << endl;

        experimentFile << "average dccr facility io : " << ((double)FIOdccr / numQueries) << endl;
        experimentFile << "average dccr client io : " << ((double)CIOdccr / numQueries) << endl;

        experimentFile << endl;

        experimentFile << "percentage of area outside dck : " << ((double)cntDC / cntRp) << endl;
        experimentFile << "percentage of area expected outside dck : " << expDCperR << endl;
        experimentFile << "percentage of area expected2 outside dck : " << expDCperR2 << endl;

        experimentFile << "percentage of area inside ar : " << (cumuAR / cntCM) << endl;
        experimentFile << "percentage of area expected inside ar : " << (expARperR / cntCM) << endl;

        experimentFile << endl;

        experimentFile << "average points pruned by candidate region : " << ((double)cntPCR / cntP) << endl;
        experimentFile << "average points pruned by candidate range : " << ((double)cntPCRR / cntP) << endl;
        experimentFile << "average points pruned by both : " << ((double)cntPB / cntP) << endl;
        experimentFile << "average points verified by sectors : " << ((double)cntIS / cntP) << endl;
        experimentFile << "average points in results : " << ((double)cntRST / cntP) << endl;
        experimentFile << "average points not in results : " << (numClients - (double)cntRST / cntP) << endl;
        experimentFile << "average points either pruned or in results : " << ((double)(cntPB + cntRST) / cntP) << endl;

        experimentFile.close();
    }
    else cout << "Unable to open file : " << experimentFileName << endl;
}

void Argument::clear()
{
    empty = 0;

    Schfc = 0;
    Sbbs = 0;
    Scr = 0;
    Sdccr = 0;

    NFchfc = 0;
    NFband = 0;
    NFskirt = 0;

    Tbf = 0;

    Tch = 0;
    Tfvc = 0;
    Trq = 0;
    Tchfc = 0;

    Tbbs = 0;
    Tskyrider = 0;
    Tcr = 0;
    Tcrr = 0;
    Tsk = 0;
    Tfv = 0;
    Tdccr = 0;

    FIOchfc = 0;
    CIOchfc = 0;
    FIOdccr = 0;
    CIOdccr = 0;

    cntRp = 0;
    cntDC = 0;
    expDCperR = 0;
    expDCperR2 = 0;

    cumuAR = 0;
    cntCM = 0;
    expARperR = 0;

    cntPCR = 0;
    cntPCRR = 0;
    cntPB = 0;
    cntP = 0;
    cntIS = 0;
    cntRST = 0;
}
