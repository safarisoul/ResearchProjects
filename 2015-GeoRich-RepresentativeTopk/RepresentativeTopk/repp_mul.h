#ifndef REPP_MUL_H
#define REPP_MUL_H

#include "repp_util.h"

class REPP_MUL
{
    public:
        REPP_MUL();

        static void repp(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight);
        static void repp_mh(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight);
    protected:
    private:
        static void multi(map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k);
        static double getscore(Mbr& pmbr, Mbr& wmbr);
};

inline double REPP_MUL::getscore(Mbr& pmbr, Mbr& wmbr)
{
    double score = 0;
    for(int d=0; d<DIM; d++)
        score += pmbr.minCoord[d] * wmbr.minCoord[d];
    return score;
}

#endif // REPP_MUL_H
