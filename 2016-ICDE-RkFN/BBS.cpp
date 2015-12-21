#include "BBS.h"

bool BBS::circularkSkyband(RStarTree& band, RStarTree& tree, Coord max, size_t k, Point& query)
{
    Coord cornerUL[DIM] = {0, max};
    Coord cornerUR[DIM] = {max, max};
    Coord cornerLL[DIM] = {0, 0};
    Coord cornerLR[DIM] = {max, 0};

    Coord rangeUL[DIM][2] = {{0, query.coord[0] - EPS}, {query.coord[1] + EPS, max}};
    Coord rangeUR[DIM][2] = {{query.coord[0] + EPS, max}, {query.coord[1] + EPS, max}};
    Coord rangeLL[DIM][2] = {{0, query.coord[0] - EPS}, {0, query.coord[1] - EPS}};
    Coord rangeLR[DIM][2] = {{query.coord[0] + EPS, max}, {0, query.coord[1] - EPS}};

    MinHeap heapUL;
    heapUL.push(MinHeapEntry(minDis(cornerUL, tree.root->mbrn), tree.root));
    MinHeap heapUR;
    heapUR.push(MinHeapEntry(minDis(cornerUR, tree.root->mbrn), tree.root));
    MinHeap heapLL;
    heapLL.push(MinHeapEntry(minDis(cornerLL, tree.root->mbrn), tree.root));
    MinHeap heapLR;
    heapLR.push(MinHeapEntry(minDis(cornerLR, tree.root->mbrn), tree.root));

    bool dominant = true;
    if(dominant && skyband(band, heapUL, cornerUL, k, rangeUL))
        dominant = false;
    if(dominant && skyband(band, heapUR, cornerUR, k, rangeUR))
        dominant = false;
    if(dominant && skyband(band, heapLL, cornerLL, k, rangeLL))
        dominant = false;
    if(dominant && skyband(band, heapLR, cornerLR, k, rangeLR))
        dominant = false;
    if(dominant)
    {
        Argument::Sbbs++;
        return false;
    }

    skyband(band, heapUL, cornerUL, k);
    skyband(band, heapUR, cornerUR, k);
    skyband(band, heapLL, cornerLL, k);
    skyband(band, heapLR, cornerLR, k);
    return true;
}

// check if band includes any unnecessary point
void BBS::falsePositive(RStarTree& band, RStarTree& tree, Coord max, size_t k)
{
    size_t total = 0, good = 0, bad = 0;

    MinHeap heap;
    heap.push(MinHeapEntry(2, band.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(2, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    heap.push(MinHeapEntry(1, entryPtr));
                }
            }
        }
        else if(e.entryPtr)
        {
            total++;

            Mbr& mbre = e.entryPtr->mbre;

            size_t mincnt = k + 1;

            for(int mask=0; mask<(1<<DIM); mask++)
            {
                Coord corner[DIM];
                for(size_t dim = 0; dim < DIM; dim++)
                    if((mask&(1<<dim)) == 0)
                        corner[dim] = 0;
                    else
                        corner[dim] = max;

                Coord range[DIM][2];
                for(size_t dim = 0; dim < DIM; dim++)
                    if(corner[dim] == 0)
                    {
                        range[dim][0] = 0;
                        range[dim][1] = mbre.coord[dim][0];
                    }
                    else
                    {
                        range[dim][0] = mbre.coord[dim][1];
                        range[dim][1] = corner[dim];
                    }
                size_t cnt = rangeQuery(band, range, k);
                mincnt = min(mincnt, cnt);
            }

            if(mincnt > k)
                bad++;
            else
                good++;
        }
        else
            assert(false);
    }

    cout << "falsePositive: [total]" << total << " [good]" << good << " [bad]" << bad << endl;
}

// check if any point not included in band
void BBS::falseNegative(RStarTree& band, RStarTree& tree, Coord max, size_t k)
{
    size_t total = 0, good = 0, bad = 0;

    MinHeap heap;
    heap.push(MinHeapEntry(2, tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(2, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    heap.push(MinHeapEntry(1, entryPtr));
                }
            }
        }
        else if(e.entryPtr)
        {
            total++;

            Mbr& mbre = e.entryPtr->mbre;

            size_t mincnt = k + 1;

            for(int mask=0; mask<(1<<DIM); mask++)
            {
                Coord corner[DIM];
                for(size_t dim = 0; dim < DIM; dim++)
                    if((mask&(1<<dim)) == 0)
                        corner[dim] = 0;
                    else
                        corner[dim] = max;

                Coord range[DIM][2];
                for(size_t dim = 0; dim < DIM; dim++)
                    if(corner[dim] == 0)
                    {
                        range[dim][0] = 0;
                        range[dim][1] = mbre.coord[dim][0];
                    }
                    else
                    {
                        range[dim][0] = mbre.coord[dim][1];
                        range[dim][1] = corner[dim];
                    }
                size_t cnt = rangeQuery(tree, range, k);
                mincnt = min(mincnt, cnt);
            }

            if(mincnt > k)
                bad++;
            else
                good++;
        }
        else
            assert(false);
    }

    cout << "falseNegative: [total]" << total << " [good]" << good << " [bad]" << bad << endl;
}

bool BBS::skyband(RStarTree& band, MinHeap& heap, Coord corner[], size_t k, Coord dRange[DIM][2])
{
    size_t cntDominant = 0;
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOdccr++;
            Mbr& mbrn = e.nodePtr->mbrn;

            Coord range[DIM][2];
            for(size_t dim = 0; dim < DIM; dim++)
                if(corner[dim] == 0)
                {
                    range[dim][0] = 0;
                    range[dim][1] = mbrn.coord[dim][0];
                }
                else
                {
                    range[dim][0] = mbrn.coord[dim][1];
                    range[dim][1] = corner[dim];
                }

            size_t cnt = rangeQuery(band, range, k);

            if(cnt < k)
            {
                if(e.nodePtr->level)
                {
                    Node_P_V& children = *e.nodePtr->children;
                    for(size_t ic = 0; ic < children.size(); ic++)
                    {
                        Node_P childPtr = children.at(ic);
                        heap.push(MinHeapEntry(minDis(corner, childPtr->mbrn), childPtr));
                    }
                }
                else
                {
                    Entry_P_V& entries = *e.nodePtr->entries;
                    for(size_t ie = 0; ie < entries.size(); ie++)
                    {
                        Entry_P entryPtr = entries.at(ie);
                        heap.push(MinHeapEntry(minDis(corner, entryPtr->mbre), entryPtr));
                    }
                }
            }
        }
        else
        {
            Mbr& mbre = e.entryPtr->mbre;

            bool dominant = true;
            for(size_t dim = 0; dim < DIM; dim++)
                if(dRange[dim][0] <= mbre.coord[dim][0] && dRange[dim][1] >= mbre.coord[dim][1])
                    ;
                else
                {
                    dominant = false;
                    break;
                }

            if(!find(band, mbre))
            {
                Coord range[DIM][2];
                for(size_t dim = 0; dim < DIM; dim++)
                    if(corner[dim] == 0)
                    {
                        range[dim][0] = 0;
                        range[dim][1] = mbre.coord[dim][0];
                    }
                    else
                    {
                        range[dim][0] = mbre.coord[dim][1];
                        range[dim][1] = corner[dim];
                    }

                size_t cnt = rangeQuery(band, range, k);

                if(cnt < k)
                    band.insertData(e.entryPtr);
            }

            if(dominant)
            {
                cntDominant++;
                if(cntDominant >= k)
                    return false;
            }
        }
    }
    return true;
}

bool BBS::skyband(RStarTree& band, MinHeap& heap, Coord corner[], size_t k)
{
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOdccr++;
            Mbr& mbrn = e.nodePtr->mbrn;

            Coord range[DIM][2];
            for(size_t dim = 0; dim < DIM; dim++)
                if(corner[dim] == 0)
                {
                    range[dim][0] = 0;
                    range[dim][1] = mbrn.coord[dim][0];
                }
                else
                {
                    range[dim][0] = mbrn.coord[dim][1];
                    range[dim][1] = corner[dim];
                }

            size_t cnt = rangeQuery(band, range, k);

            if(cnt < k)
            {
                if(e.nodePtr->level)
                {
                    Node_P_V& children = *e.nodePtr->children;
                    for(size_t ic = 0; ic < children.size(); ic++)
                    {
                        Node_P childPtr = children.at(ic);
                        heap.push(MinHeapEntry(minDis(corner, childPtr->mbrn), childPtr));
                    }
                }
                else
                {
                    Entry_P_V& entries = *e.nodePtr->entries;
                    for(size_t ie = 0; ie < entries.size(); ie++)
                    {
                        Entry_P entryPtr = entries.at(ie);
                        heap.push(MinHeapEntry(minDis(corner, entryPtr->mbre), entryPtr));
                    }
                }
            }
        }
        else
        {
            Mbr& mbre = e.entryPtr->mbre;

            if(!find(band, mbre))
            {
                Coord range[DIM][2];
                for(size_t dim = 0; dim < DIM; dim++)
                    if(corner[dim] == 0)
                    {
                        range[dim][0] = 0;
                        range[dim][1] = mbre.coord[dim][0];
                    }
                    else
                    {
                        range[dim][0] = mbre.coord[dim][1];
                        range[dim][1] = corner[dim];
                    }

                size_t cnt = rangeQuery(band, range, k);

                if(cnt < k)
                    band.insertData(e.entryPtr);
            }
        }
    }
    return true;
}

size_t BBS::rangeQuery(RStarTree& tree, Coord range[][2], size_t k)
{
    size_t cnt = 0;
    Node_P_V queue;
    queue.push_back(tree.root);
    while(queue.size() > 0)
    {
        Node_P nodePtr = queue.back();
        queue.pop_back();
        Mbr& mbrn = nodePtr->mbrn;
        // continue if no overlap
        if(mbrn.coord[0][0] > range[0][1] || mbrn.coord[0][1] < range[0][0] || mbrn.coord[1][0] > range[1][1] || mbrn.coord[1][1] < range[1][0])
            continue;
        // take as a whole if completely contained
        if(mbrn.coord[0][0] >= range[0][0] && mbrn.coord[0][1] <= range[0][1] && mbrn.coord[1][0] >= range[1][0] && mbrn.coord[1][1] <= range[1][1])
        {
            cnt += nodePtr->aggregate;
            if(cnt > k)
                return cnt;
        }
        // else add children for further exploration
        else
        {
            if(nodePtr->level)
            {
                Node_P_V& children = *nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                    queue.push_back(children.at(ic));
            }
            else
            {
                Entry_P_V& entries = *nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Mbr& mbre = entries.at(ie)->mbre;
                    if(mbre.coord[0][0] >= range[0][0] && mbre.coord[0][1] <= range[0][1] && mbre.coord[1][0] >= range[1][0] && mbre.coord[1][1] <= range[1][1])
                    {
                        cnt++;
                        if(cnt > k)
                            return cnt;
                    }
                }
            }
        }
    }
    return cnt;
}

bool BBS::find(RStarTree& tree, Mbr& mbr)
{
    Node_P_V queue;
    queue.push_back(tree.root);
    while(queue.size() > 0)
    {
        Node_P nodePtr = queue.back();
        queue.pop_back();
        Mbr& mbrn = nodePtr->mbrn;

        bool contain = true;
        for(size_t dim = 0; dim < DIM; dim++)
            if(mbrn.coord[dim][0] > mbr.coord[dim][0] || mbrn.coord[dim][1] < mbr.coord[dim][1])
            {
                contain = false;
                break;
            }

        if(!contain)
            continue;

        if(nodePtr->level)
        {
            Node_P_V& children = *nodePtr->children;
            for(size_t ic = 0; ic < children.size(); ic++)
                queue.push_back(children.at(ic));
        }
        else
        {
            Entry_P_V& entries = *nodePtr->entries;
            for(size_t ie = 0; ie < entries.size(); ie++)
            {
                Mbr& mbre = entries.at(ie)->mbre;

                bool identical = true;
                for(size_t dim = 0; dim < DIM; dim++)
                    if(mbre.coord[dim][0] != mbr.coord[dim][0])
                    {
                        identical = false;
                        break;
                    }

                if(identical)
                    return true;
            }
        }
    }
    return false;
}


