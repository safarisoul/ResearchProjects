#ifndef REPP_TK_H
#define REPP_TK_H

#include "repp_util.h"

class REPP_TK
{
    public:
        REPP_TK();

        static void repp(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight);
        static void repp_mh(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight);
    protected:
    private:
        static void topk(map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k);
};

#endif // REPP_TK_H
