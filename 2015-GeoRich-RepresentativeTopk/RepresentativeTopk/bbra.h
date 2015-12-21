#ifndef BBRA_H
#define BBRA_H

#include <set>
#include <assert.h>

#include "rstutil.h"
#include "record.h"

class BBRA
{
    public:
        BBRA();

        static set<int> bbra(Mbr& pmbr, RSTree& ptree, RSTree& wtree, int k);
    protected:
    private:
        static double w;

        static double lv(Mbr& pmbr, Mbr& wmbr);
        static double uv(Mbr& pmbr, Mbr& wmbr);

        static int intopka(Mbr& pmbr, Mbr& wmbr, bool single, RSTree& ptree, int k);

        static double distance(Mbr& mbr);
        static void add(set<int>& rtopk, NodePtr nPtr);
};

inline double BBRA::lv(Mbr& pmbr, Mbr& wmbr)
{
    double score = 0;
    for(int d=0; d<DIM; d++)
        score += pmbr.minCoord[d] * wmbr.minCoord[d];
    return score;
}

inline double BBRA::uv(Mbr& pmbr, Mbr& wmbr)
{
    double score = 0;
    for(int d=0; d<DIM; d++)
        score += pmbr.maxCoord[d] * wmbr.maxCoord[d];
    return score;
}

inline double BBRA::distance(Mbr& mbr)
{
    double dis = 0;
    for(int d=0; d<DIM; d++)
        if(w < mbr.minCoord[d])
        {
            double diff = mbr.minCoord[d] - w;
            dis += diff * diff;
        }
        else if(w > mbr.maxCoord[d])
        {
            double diff = w - mbr.maxCoord[d];
            dis += diff * diff;
        }
    return dis;
}

inline void BBRA::add(set<int>& rtopk, NodePtr nPtr)
{
    MinHeap heap;
    heap.push(MinHeapEntry(1, nPtr));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nPtr->level)
        {
            NodePtrV& children = *e.nPtr->children;
            for(unsigned int ic=0; ic<children.size(); ic++)
            {
                NodePtr cPtr = children[ic];
                heap.push(MinHeapEntry(1, cPtr));
            }
        }
        else
        {
            EntryPtrV& entries = *e.nPtr->entries;
            for(unsigned int ie=0; ie<entries.size(); ie++)
            {
                EntryPtr ePtr = entries[ie];
                rtopk.insert(ePtr->dataID);
            }
        }
    }
}

#endif // BBRA_H
