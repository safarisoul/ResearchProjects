#ifndef DATA_H
#define DATA_H

#include<sstream>
#include<fstream>
#include<assert.h>
#include<iostream>

#include "rstree.h"
#include "rstutil.h"

using namespace std;

typedef struct Point
{
    double coord[DIM];

    friend istream& operator >> (istream& in, Point& p)
    {
        for(int d=0; d<DIM; d++)
            in >> p.coord[d];
        return in;
    }
}Point;

typedef vector<Point> PointV;

class Data
{
    public:
        Data() {};
        ~Data()
        {
            if(ptree)
                delete ptree;
            if(wtree)
                delete wtree;
        }

        static PointV ppoints;
        static EntryV pentries;
        static RSTreePtr ptree;

        static PointV wpoints;
        static EntryV wentries;
        static RSTreePtr wtree;

        static void initData(string pfile, string wfile);
    protected:
    private:
        static void loadPoint(PointV& points, string file);
        static void buildTree(PointV& points, EntryV& entries, RSTreePtr tree);
};

inline void Data::loadPoint(PointV& points, string file)
{
    ifstream in(file.c_str());
    points.clear();
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
}

inline void Data::buildTree(PointV& points, EntryV& entries, RSTreePtr tree)
{
    entries.clear();
    for(unsigned int ip=0; ip<points.size(); ip++)
    {
        Mbr mbr(points[ip].coord);
        LeafNodeEntry entry(mbr, ip);
        entries.push_back(entry);
    }

    for(unsigned int ie=0; ie<entries.size(); ie++)
        tree->insertData(&entries[ie]);
}


#endif // DATA_H
