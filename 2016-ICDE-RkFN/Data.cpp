#include "Data.h"

/*******************************************************************************
 * Point
 ******************************************************************************/

Point::Point()
{
}

Point::Point(Coord c[DIM])
{
    for(size_t dim = 0; dim < DIM; dim++)
        coord[dim] = c[dim];
}

Point::Point(Coord x, Coord y)
{
    coord[0] = x;
    coord[1] = y;
}

istream& operator>>(istream& in, Point& p)
{
    for(size_t dim = 0; dim < DIM; dim++)
        in >> p.coord[dim];
    return in;
}

bool Point::operator< (const Point& p) const
{
    for(size_t dim = 0; dim < DIM; dim++)
        if(coord[dim] != p.coord[dim])
            return coord[dim] < p.coord[dim];
    return true;
}

void Point::print()
{
    cout << "<" << coord[0];
    for(size_t dim = 0; dim < DIM; dim++)
        cout << "," << coord[dim];
    cout << ">" << endl;
}

/*******************************************************************************
 * Data
 ******************************************************************************/

Point_V Data::facilities;
Point_V Data::clients;
Point_V Data::queries;

RStarTree* Data::fTree = NULL;
RStarTree* Data::cTree = NULL;
Point* Data::query = NULL;
size_t Data::indexq = 0;

Entry_V Data::fEntries;
Entry_V Data::cEntries;

Data::Data()
{
}

Data::~Data()
{
    if(fTree)
        delete fTree;
    if(cTree)
        delete cTree;
    if(query)
        delete query;
}

void Data::buildFTree()
{
    string facilityFileName = Argument::getFacilityFileName();
    ifstream facilityFileIFS(facilityFileName.c_str());
    facilities.clear();
    loadPoint(facilityFileIFS, facilities, facilityFileName);
    if(fTree)
        delete fTree;
    fTree = new RStarTree();
    buildTree(facilities, fEntries, fTree);
}

void Data::buildCTree()
{
    string clientFileName = Argument::getClientFileName();
    ifstream clientFileIFS(clientFileName.c_str());
    clients.clear();
    loadPoint(clientFileIFS, clients, clientFileName);
    if(cTree)
        delete cTree;
    cTree = new RStarTree();
    buildTree(clients, cEntries, cTree);
}

void Data::loadQueries()
{
    string queryFileName = Argument::getQueryFileName();
    ifstream queryFileIFS(queryFileName.c_str());
    queries.clear();
    loadPoint(queryFileIFS, queries, queryFileName);
    indexq = 0;
    if(queries.size() > 0)
        query = &queries.at(0);
}

size_t Data::getNumOfQueries()
{
    return queries.size();
}

void Data::next()
{
    indexq++;
    if(indexq < queries.size())
        query = &queries.at(indexq);
    else
        query = NULL;
}

void Data::buildTree(Point_V& points, Entry_V& entries, RStarTree* tree)
{
    entries.clear();
    for(size_t ip = 0; ip < points.size(); ip++)
    {
        Data_P datap = &points.at(ip);
        Mbr mbr(points.at(ip).coord);
        LeafNodeEntry entry(mbr, datap);
        entries.push_back(entry);
    }
    //cout << entries.size() << " entries created" << endl;

    for(size_t ie = 0; ie < entries.size(); ie++)
        tree->insertData(&entries.at(ie));
    //cout << tree->root->aggregate << " entries inserted" << endl;
}

void Data::loadPoint(ifstream& in, Point_V& points, string fileName)
{
    assert(in.is_open());
    while(true)
    {
        Point point;
        in >> point;
        if(in) // check that the inputs succeeded
            points.push_back(point);
        else
            break;
    }
    in.close();
    //cout << points.size() << " points from " << fileName << endl;
}
