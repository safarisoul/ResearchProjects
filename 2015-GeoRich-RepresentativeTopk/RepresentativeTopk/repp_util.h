#ifndef REPP_UTIL_H
#define REPP_UTIL_H

#include <map>
#include <sys/time.h>

#include "bbs.h"
#include "bbra.h"
#include "bbtk.h"
#include "minhash.h"
#include "kmv.h"
#include "record.h"

typedef struct Score
{
    double cscore;
    double dscore;
    double score;

    void output();
}Score;

class REPP_UTIL
{
    public:
        REPP_UTIL();

        static void pick(set<int>& rst, Score& s, map< int, set<int> >& rtks, int wsize, int t, double dweight);
        static void pick_mh(set<int>& rst, Score& s, map<int, Heap>& kmvm, map< int, vector<int> >& mhm, int wsize, int t, double dweight);
        static void pick_mh2(set<int>& rst, Score& s, map<int, Heap>& kmvm, map< int, vector<int> >& mhm, int wsize, int t, double dweight);
        static void mhkmv(map<int, Heap>& kmvm, map<int, vector<int> >& mhm, map<int, set<int> >& rtks);
        static void score(set<int>& rst, Score& s, map< int, set<int> >& rtks, int wsize, int t, double dweight);
    protected:
    private:
        static double mindis(map< int, set<int> >& rtks, set<int>& rst, int id);
        static double jaccardsimilarity(set<int>& set1, set<int>& set2);
        static int coverage(set<int>& set1, set<int>& set2);
        static void setunion(set<int>& to, set<int>& from);

        static double mindis(map< int, vector<int> >& mhm, set<int>& rst, int id);
};

#endif // REPP_UTIL_H
