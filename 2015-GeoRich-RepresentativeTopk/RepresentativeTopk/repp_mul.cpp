#include "repp_mul.h"

void REPP_MUL::repp(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight)
{
    map<int, set<int> > rtks;
    multi(rtks, ptree, wtree, k);

    REPP_UTIL::pick(rst, s, rtks, wtree.root->aggregate, t, dweight);
}

void REPP_MUL::repp_mh(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight)
{
    map<int, Heap> kmvm;
    map< int, vector<int> > mhm;
    map<int, set<int> > rtks;
    multi(rtks, ptree, wtree, k);

    REPP_UTIL::mhkmv(kmvm, mhm, rtks);

    REPP_UTIL::pick_mh(rst, s, kmvm, mhm, wtree.root->aggregate, t, dweight);
    REPP_UTIL::score(rst, s, rtks, wtree.root->aggregate, t, dweight);
}

void REPP_MUL::multi(map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    RSTree band;
    EntryPtrV bandv;
    BBS::skyband(band, bandv, ptree, k);

    NodePtrV queue;
    queue.push_back(wtree.root);
    while(queue.size() > 0)
    {
        Record::io_mul_wtree_cnt++;
        NodePtr nPtr = queue.back();
        queue.pop_back();
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
                Mbr& wmbr = entries[ie]->mbr;
                Heap tkh;
                for(unsigned int iv=0; iv<bandv.size(); iv++)
                {
                    double score = getscore(bandv[iv]->mbr, wmbr);
                    if(tkh.size() < k)
                        tkh.push(HeapEntry(score, bandv[iv]->dataID));
                    else if(score < tkh.top())
                    {
                        tkh.pop();
                        tkh.push(HeapEntry(score, bandv[iv]->dataID));
                    }
                }
                while(!tkh.isEmpty())
                {
                    rtks[tkh.topid()].insert(entries[ie]->dataID);
                    tkh.pop();
                }
            }
        }
    }

    gettimeofday(&end, NULL);
    Record::mul_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}
