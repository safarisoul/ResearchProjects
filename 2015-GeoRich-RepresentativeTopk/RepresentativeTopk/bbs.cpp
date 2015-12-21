#include "bbs.h"

void BBS::skyband(RSTree& band, EntryPtrV& bandv, RSTree& tree, int k)
{
    MinHeap heap;
    heap.push(MinHeapEntry(mindis(tree.root->mbr), tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nPtr)
        {
            Record::io_bbs_ptree_cnt++;
            Mbr& mbr = e.nPtr->mbr;
            Mbr range = getrange(mbr);
            int cnt = rangeQuery(band, range, k);
            if(cnt < k)
            {
                if(e.nPtr->level)
                {
                    NodePtrV& children = *e.nPtr->children;
                    for(unsigned int ic=0; ic<children.size(); ic++)
                    {
                        NodePtr cPtr = children[ic];
                        heap.push(MinHeapEntry(mindis(cPtr->mbr), cPtr));
                    }
                }
                else
                {
                    EntryPtrV& entries = *e.nPtr->entries;
                    for(unsigned int ie=0; ie<entries.size(); ie++)
                    {
                        EntryPtr ePtr = entries[ie];
                        heap.push(MinHeapEntry(mindis(ePtr->mbr), ePtr));
                    }
                }
            }
        }
        else
        {
            Mbr& mbr = e.ePtr->mbr;
            Mbr range = getrange(mbr);
            int cnt = rangeQuery(band, range, k);
            if(cnt < k)
            {
                band.insertData(e.ePtr);
                bandv.push_back(e.ePtr);
            }
        }
    }

    Record::skyband_cnt = band.root->aggregate;
}

int BBS::rangeQuery(RSTree& band, Mbr& range, int k)
{
    int cnt = 0;
    NodePtrV queue;
    queue.push_back(band.root);
    while(queue.size() > 0)
    {
        NodePtr nPtr = queue.back();
        queue.pop_back();
        Mbr& mbr = nPtr->mbr;
        // continue if no overlap
        for(int d=0; d<DIM; d++)
            if(mbr.minCoord[d] > range.maxCoord[d])
                continue;
        // check if mbr is completely in range
        bool complete = true;
        for(int d=0; d<DIM; d++)
            if(mbr.maxCoord[d] >= range.maxCoord[d])
            {
                complete = false;
                break;
            }
        if(complete)
        {
            cnt += nPtr->aggregate;
            if(cnt >= k)
                return cnt;
        }
        else
        {
            if(nPtr->level)
            {
                NodePtrV& children = *nPtr->children;
                for(unsigned int ic=0; ic<children.size(); ic++)
                    queue.push_back(children[ic]);
            }
            else
            {
                EntryPtrV& entries = *nPtr->entries;
                for(unsigned int ie=0; ie<entries.size(); ie++)
                {
                    Mbr& m = entries[ie]->mbr;
                    bool in = true;
                    for(int d=0; d<DIM; d++)
                        if(m.maxCoord[d] >= range.maxCoord[d])
                        {
                            in = false;
                            break;
                        }
                    if(in)
                    {
                        cnt++;
                        if(cnt >= k)
                            return cnt;
                    }
                }
            }
        }
    }
    return cnt;
}
