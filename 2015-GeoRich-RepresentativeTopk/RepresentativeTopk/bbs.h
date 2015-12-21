#ifndef BBS_H
#define BBS_H

#include <iostream>

#include "rstutil.h"
#include "record.h"

using namespace std;

class BBS
{
    public:
        BBS();

        static void skyband(RSTree& band, EntryPtrV& bandv, RSTree& tree, int k);

        static Mbr getrange(Mbr& mbr);
    protected:
    private:
        static int rangeQuery(RSTree& band, Mbr& range, int k);

        static double mindis(Mbr& mbr);
};

inline Mbr BBS::getrange(Mbr& mbr)
{
    Mbr range;
    for(int d=0; d<DIM; d++)
    {
        range.minCoord[d] = 0;
        range.maxCoord[d] = mbr.minCoord[d];
    }
    return range;
}

inline double BBS::mindis(Mbr& mbr)
{
    double dis = 0;
    for(int d=0; d<DIM; d++)
        dis += mbr.minCoord[d] * mbr.minCoord[d];
    return dis;
}

#endif // BBS_H
