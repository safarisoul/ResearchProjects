#ifndef BBS_H
#define BBS_H

#include "RStarTreeUtil.h"

class BBS
{
    public:
        BBS() { }

        static bool circularkSkyband(RStarTree& band, RStarTree& tree, Coord max, size_t k, Point& query);

        static void falsePositive(RStarTree& band, RStarTree& tree, Coord max, size_t k);
        static void falseNegative(RStarTree& band, RStarTree& tree, Coord max, size_t k);

    protected:
    private:
        static bool skyband(RStarTree& band, MinHeap& heap, Coord corner[], size_t k, Coord dRange[DIM][2]);
        static bool skyband(RStarTree& band, MinHeap& heap, Coord corner[], size_t k);
        static size_t rangeQuery(RStarTree& tree, Coord range[][2], size_t k);
        static bool find(RStarTree& tree, Mbr& mbr);

        // inline functions
        static double minDis(Coord corner[], Mbr& mbr);
};

inline double BBS::minDis(Coord corner[], Mbr& mbr)
{
    double minDis = 0;
    for(size_t dim = 0; dim < DIM; dim++)
        if(corner[dim] == 0)
            minDis += mbr.coord[dim][0];
        else
            minDis += corner[dim] - mbr.coord[dim][1];
    return minDis;
}


#endif // BBS_H
