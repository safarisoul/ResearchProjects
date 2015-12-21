#ifndef BBTK_H
#define BBTK_H

#include <set>

#include "rstutil.h"
#include "record.h"
#include "heap.h"

class BBTK
{
    public:
        BBTK();

        static set<int> bbtk(Mbr& wmbr, RSTree& ptree, int k);
    protected:
    private:
        static double minscore(Mbr& wmbr, Mbr& pmbr);
};

inline double BBTK::minscore(Mbr& wmbr, Mbr& pmbr)
{
    double score = 0;
    for(int d=0; d<DIM; d++)
        score += wmbr.minCoord[d] * pmbr.minCoord[d];
    return score;
}
#endif // BBTK_H
