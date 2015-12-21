#include "bbtk.h"

set<int> BBTK::bbtk(Mbr& wmbr, RSTree& ptree, int k)
{
    Record::topk_cnt++;
    Heap tkh;
    MinHeap heap;
    heap.push(MinHeapEntry(minscore(wmbr, ptree.root->mbr), ptree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(tkh.size() >= k && e.key > tkh.top())
            continue;
        if(e.nPtr)
        {
            if(e.nPtr->level)
            {
                NodePtrV& children = *e.nPtr->children;
                for(unsigned int ic=0; ic<children.size(); ic++)
                {
                    NodePtr cPtr = children[ic];
                    heap.push(MinHeapEntry(minscore(wmbr, cPtr->mbr), cPtr));
                }
            }
            else
            {
                EntryPtrV& entries = *e.nPtr->entries;
                for(unsigned int ie=0; ie<entries.size(); ie++)
                {
                    EntryPtr ePtr = entries[ie];
                    heap.push(MinHeapEntry(minscore(wmbr, ePtr->mbr), ePtr));
                }
            }
        }
        else
        {
            tkh.push(HeapEntry(e.key, e.ePtr->dataID));
            if(tkh.size() > k)
                tkh.pop();
        }
    }

    set<int> tk;
    while(!tkh.isEmpty())
    {
        tk.insert(tkh.topid());
        tkh.pop();
    }
    return tk;
}
