#ifndef KMV_H
#define KMV_H

#include <set>
#include <cmath>
#include <iostream>
#include <assert.h>

#include "heap.h"

using namespace std;

class KMV
{
    public:
        KMV();

        inline static void setsize(int size) { size = size; };
        inline static bool isundersize(Heap& kmv) { return kmv.size() < size; };
        static Heap getkmv(set<int>& data);
        static double estimatesize(Heap& kmv);
        static double unionsize(Heap& kmv1, Heap& kmv2);
        static void combine(Heap& to, Heap& from);
    protected:
    private:
        static int size;
        static double goldenratio;
        static double hash(int id);
        static void combine(Heap& kmv, Heap& kmv1, Heap& kmv2);
};
/*
inline double KMV::estimatesize(Heap& kmv)
{
    if(kmv.size() == 0)
        return 0;
    double est = 0;
    HEntryV values = kmv.getElements();
    for(int i=1; i<values.size(); i++)
        est += i / values[i].key;
    if(values.size() > 1)
        est /= values.size() - 1;
    return est;
}
*/
inline double KMV::estimatesize(Heap& kmv)
{
    if(kmv.size() == 0)
        return 0;
    return (kmv.size() - 1) / kmv.top();
}

inline double KMV::unionsize(Heap& kmv1, Heap& kmv2)
{
    Heap kmv;
    combine(kmv, kmv1, kmv2);
    return estimatesize(kmv);
}

inline double KMV::hash(int id)
{
    double v = id * goldenratio;
    return v - (int)v;
}


#endif // KMV_H
