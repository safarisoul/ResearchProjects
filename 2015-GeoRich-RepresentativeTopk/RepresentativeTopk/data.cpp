#include "data.h"

PointV Data::ppoints;
EntryV Data::pentries;
RSTreePtr Data::ptree = NULL;

PointV Data::wpoints;
EntryV Data::wentries;
RSTreePtr Data::wtree = NULL;

void Data::initData(string pfile, string wfile)
{
    if(ptree)
        delete(ptree);
    ptree = new RSTree();
    loadPoint(ppoints, pfile);
    buildTree(ppoints, pentries, ptree);

    if(wtree)
        delete(wtree);
    wtree = new RSTree();
    loadPoint(wpoints, wfile);
    buildTree(wpoints, wentries, wtree);
}
