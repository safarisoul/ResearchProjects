#ifndef REPP_RTK_H
#define REPP_RTK_H

#include "repp_util.h"

class REPP_RTK
{
    public:
        REPP_RTK();

        static void repp(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight);
        static void repp_mh(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight);
        static void repp_mh2(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight);
    protected:
    private:
        static void rtopk(map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k);

        static void rtopk_mh(map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k);
        static int rangeQuery_mh(map<int, set<int> >& rtks, RSTree& band, Mbr& range);

        static void rtopk_mh(map<int, Heap>& kmvm, map<int, vector<int> >& mhm, map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k);
        static int rangeQuery_mh(map<int, vector<int> >& mhm, RSTree& band, Mbr& range);
};

#endif // REPP_RTK_H
