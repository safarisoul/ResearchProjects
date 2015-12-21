#ifndef MINHASH_H
#define MINHASH_H

#include <iostream>
#include <vector>
#include <set>
#include <climits>
#include <cassert>
#include <algorithm>
#include <ctime>

using namespace std;

const int MAX_FUN = 1000;

class MinHash
{
    public:
        MinHash()
        {
            srand((unsigned)time(0));
        }

        static void init(int nf);
        static vector<int> getmh(set<int>& data);
        static double jaccardsimilarity(vector<int>& mh1, vector<int>& mh2);
        static double jaccardsimilarity(vector< vector<int> >& mhs);
    protected:
    private:
        static int PRIME_MAX;
        static int PRIME_MIN;
        static int nfun;
        static int hfun[MAX_FUN][3];
        static int hash(int data, int funi);

        static double getRandom(int max);
};

inline int MinHash::hash(int data, int funi)
{
    long long value = hfun[funi][0];
    value *= data;
    value += hfun[funi][1];
    value %= hfun[funi][2];
    return (int)value;
}

inline double MinHash::getRandom(int max)
{
    double num = rand();
    return num / RAND_MAX * max;
}

#endif // MINHASH_H
