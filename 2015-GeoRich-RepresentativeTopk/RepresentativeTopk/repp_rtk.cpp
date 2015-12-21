#include "repp_rtk.h"

void REPP_RTK::repp(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight)
{
    map< int, set<int> > rtks;
    rtopk(rtks, ptree, wtree, k);

    REPP_UTIL::pick(rst, s, rtks, wtree.root->aggregate, t, dweight);
}

void REPP_RTK::repp_mh(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight)
{
    map<int, Heap> kmvm;
    map< int, vector<int> > mhm;
    map< int, set<int> > rtks;
    rtopk_mh(rtks, ptree, wtree, k);

    REPP_UTIL::mhkmv(kmvm, mhm, rtks);

    REPP_UTIL::pick_mh(rst, s, kmvm, mhm, wtree.root->aggregate, t, dweight);
    REPP_UTIL::score(rst, s, rtks, wtree.root->aggregate, t, dweight);
}

void REPP_RTK::repp_mh2(set<int> rst, Score& s, RSTree& ptree, RSTree& wtree, int k, int t, double dweight)
{
    map<int, Heap> kmvm;
    map< int, vector<int> > mhm;
    map< int, set<int> > rtks;
    rtopk_mh(kmvm, mhm, rtks, ptree, wtree, k);

    REPP_UTIL::pick_mh2(rst, s, kmvm, mhm, wtree.root->aggregate, t, dweight);
    REPP_UTIL::score(rst, s, rtks, wtree.root->aggregate, t, dweight);
}

void REPP_RTK::rtopk(map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    RSTree band;
    EntryPtrV bandv;
    BBS::skyband(band, bandv, ptree, k);

    for(unsigned int ib=0; ib<bandv.size(); ib++)
    {
        Mbr& pmbr = bandv[ib]->mbr;
        set<int> rstbbr = BBRA::bbra(pmbr, band, wtree, k);
        if(rstbbr.size() > 0)
            rtks[bandv[ib]->dataID] = rstbbr;
    }

    gettimeofday(&end, NULL);
    Record::rtopk_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}

void REPP_RTK::rtopk_mh(map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    RSTree band;
    EntryPtrV bandv;
    BBS::skyband(band, bandv, ptree, k);

    for(unsigned int ib=0; ib<bandv.size(); ib++)
    {
        Mbr& pmbr = bandv[ib]->mbr;
        Mbr range = BBS::getrange(pmbr);

        int cnt = rangeQuery_mh(rtks, band, range);
        if(cnt < 0)
            continue;

        set<int> rstbbr = BBRA::bbra(pmbr, band, wtree, k);
        if(rstbbr.size() > 0)
            rtks[bandv[ib]->dataID] = rstbbr;
    }

    gettimeofday(&end, NULL);
    Record::rtopk_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}

int REPP_RTK::rangeQuery_mh(map<int, set<int> >& rtks, RSTree& band, Mbr& range)
{
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
        // open up
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
                    if(rtks.find(entries[ie]->dataID) == rtks.end())
                        return -1;
                }
            }
        }
    }
    return 0;
}

void REPP_RTK::rtopk_mh(map<int, Heap>& kmvm, map<int, vector<int> >& mhm, map<int, set<int> >& rtks, RSTree& ptree, RSTree& wtree, int k)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    RSTree band;
    EntryPtrV bandv;
    BBS::skyband(band, bandv, ptree, k);

    for(unsigned int ib=0; ib<bandv.size(); ib++)
    {
        Mbr& pmbr = bandv[ib]->mbr;
        Mbr range = BBS::getrange(pmbr);

        int cnt = rangeQuery_mh(mhm, band, range);
        if(cnt < 0)
            continue;

        set<int> rstbbr = BBRA::bbra(pmbr, band, wtree, k);
        if(rstbbr.size() > 0)
        {
            rtks[bandv[ib]->dataID] = rstbbr;
            kmvm[bandv[ib]->dataID] = KMV::getkmv(rstbbr);
            mhm[bandv[ib]->dataID] = MinHash::getmh(rstbbr);
        }
    }

    gettimeofday(&end, NULL);
    Record::rtopk_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
}

int REPP_RTK::rangeQuery_mh(map<int, vector<int> >& mhm, RSTree& band, Mbr& range)
{
    vector<int> dom;

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
        // open up
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
                    if(mhm.find(entries[ie]->dataID) == mhm.end())
                        return -1;
                    dom.push_back(entries[ie]->dataID);
                }
            }
        }
    }
    if(dom.size() == 0)
        return 0;
    vector< vector<int> > dommhs;
    for(unsigned int id=0; id<dom.size(); id++)
        dommhs.push_back(mhm[dom[id]]);
    double est = MinHash::jaccardsimilarity(dommhs);
    if(est < 1)
        return -2;
    return 0;
}
