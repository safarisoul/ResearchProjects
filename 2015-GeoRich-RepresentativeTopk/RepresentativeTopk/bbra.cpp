#include "bbra.h"

double BBRA::w = 1/DIM;

set<int> BBRA::bbra(Mbr& pmbr, RSTree& ptree, RSTree& wtree, int k)
{
    Record::rtopk_cnt++;
    set<int> rtopk;
    MinHeap heap;
    heap.push(MinHeapEntry(distance(wtree.root->mbr), wtree.root));
    while(!heap.isEmpty())
    {
        Record::io_bbra_wtree_cnt++;
        MinHeapEntry e = heap.pop();
        int judge;
        if(e.ePtr)
        {
            judge = intopka(pmbr, e.ePtr->mbr, true, ptree, k);
            assert(judge != 0);
            if(judge == 1)
                rtopk.insert(e.ePtr->dataID);
        }
        else
        {
            judge = intopka(pmbr, e.nPtr->mbr, false, ptree, k);
            if(judge == 1)
                add(rtopk, e.nPtr);
            else if(judge == 0)
            {
                if(e.nPtr->level)
                {
                    NodePtrV& children = *e.nPtr->children;
                    for(unsigned int ic=0; ic<children.size(); ic++)
                    {
                        NodePtr cPtr = children[ic];
                        heap.push(MinHeapEntry(distance(cPtr->mbr), cPtr));
                    }
                }
                else
                {
                    EntryPtrV& entries = *e.nPtr->entries;
                    for(unsigned int ie=0; ie<entries.size(); ie++)
                    {
                        EntryPtr ePtr = entries[ie];
                        heap.push(MinHeapEntry(distance(ePtr->mbr), ePtr));
                    }
                }
            }
        }
    }
    return rtopk;
}

int BBRA::intopka(Mbr& pmbr, Mbr& wmbr, bool single, RSTree& ptree, int k)
{
    double lvp = lv(pmbr, wmbr), uvp = uv(pmbr, wmbr);
    int precincPoints = 0, precEntries = 0;
    MinHeap heap;
    RSTNode& node = *ptree.root;
    double lvr = lv(node.mbr, wmbr);
    if(uvp > lvr)
    {
        heap.push(MinHeapEntry(lvr, ptree.root));
        if(uv(node.mbr, wmbr) < lvp)
            precEntries += node.aggregate;
    }
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Mbr& embr = e.nPtr->mbr; // only nodes will be push into heap
        if(precincPoints >= k && lv(embr, wmbr) >= lvp)
        {
            if(single)
                return -1;
            else
                return 0;
        }
        if(uv(embr, wmbr) < lvp)
            precEntries -= e.nPtr->aggregate;
        if(e.nPtr->level)
        {
            NodePtrV& children = *e.nPtr->children;
            for(unsigned int ic=0; ic<children.size(); ic++)
            {
                NodePtr cPtr = children[ic];
                double lvc = lv(cPtr->mbr, wmbr);
                if(uvp > lvc)
                {
                    if(uv(cPtr->mbr, wmbr) < lvp)
                    {
                        precEntries += cPtr->aggregate;
                        if(precEntries >= k)
                            return -1;
                    }
                    heap.push(MinHeapEntry(lvc, cPtr));
                }
            }
        }
        else
        {
            EntryPtrV& entries = *e.nPtr->entries;
            for(unsigned int ie=0; ie<entries.size(); ie++)
            {
                EntryPtr ePtr = entries[ie];
                if(uvp > lv(ePtr->mbr, wmbr))
                {
                    if(uv(ePtr->mbr, wmbr) < lvp)
                    {
                        precEntries++;
                        if(precEntries >= k)
                            return -1;
                    }
                    precincPoints++;
                }
            }
        }
    }
    if(precincPoints >= k)
        return 0;
    else
        return 1;
}
