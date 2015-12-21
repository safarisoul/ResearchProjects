#include "rstree.h"

int RSTree::minChild = RSTree::maxChild * SPLIT_FACTOR;

/** Invoke Insert starting with the leaf level as a parameter, to insert a new data rectangle
  */
void RSTree::insertData(EntryPtr ePtr)
{
    /// Bits of overflowLevel indicate first time overflow of the corresponding level.
    int overflowLevel = -1; // initialize 32 bits of 1s
    overflowLevel <<= (root->level); // set the lower (root->level) bits to 0s
    insert(NULL, ePtr, LEAF_LEVEL, overflowLevel);
}

/** Invoke ChooseSubtree, with the level as a parameter, to find an appropriate node N, in which to place the new entry E.
  *
  * If N has less than M entries, accommodate E in N.
  * If N has M entries, invoke OverflowTreatment with the level of N as a parameter [for reinsertion or split].
  *
  * If OverflowTreatment was called and a plit was performed, propagate OverflowTreatment upwards if necessary.
  * If OverflowTreatment caused a split of the root, create a new root.
  *
  * Adjust all covering rectangles in the insertion path such that they are minimum bounding boxes enclosing their children rectangles.
  *
  * --------------------------------------------------
  *
  * [OverflowTreatment]
  * If the level is not the root level and this is the first call of OverflowTreatment in the given level during the insertion of one data rectangle, then invoke ReInsert.
  * Else invoke Split.
  */
void RSTree::insert(NodePtr cPtr, EntryPtr ePtr, int desiredLevel, int &overflowLevel)
{
    /// Bits of overflowLevel indicate first time overflow of the corresponding level
    Mbr &mbr = (desiredLevel ? cPtr->mbr : ePtr->mbr);
    NodePtr nPtr = chooseSubtree(mbr, desiredLevel);
    if(desiredLevel) // if desiredLevel is not leaf node level
        nPtr->insert(cPtr);
    else // if desiredLevel is leaf node level
        nPtr->insert(ePtr);

    while(nPtr) // while node is not NULL
    {
        while(nPtr->size() > RSTree::maxChild)
        {
            if(overflowLevel & (1<<nPtr->level))
                split(*nPtr);
            else
                reInsert(*nPtr, overflowLevel);
        }
        nPtr = nPtr->parent;
    }
}

/** Set N to be the root.
  *
  * If N is a leaf, return N.
  * If the childpointers in N point to leaves [determine the minimum overlap cost].
  *     Choose the entry in N whose rectangle needs least overlap enlargement to include the new data rectangle.
  *     Resolve ties by choosing the entry whose rectangle needs least area enlargement.
  *     Then the entry with the rectangle of smallest area.
  * If the childpointers in N do not point to leaves [determine the minimum area cost].
  *     Choose the entry in N whose rectangle needs least area enlargement to include the new data rectangle.
  *     Resolve ties by choosing the entry with the rectangle of smallest area.
  *
  * Set N to be the childnode pointed to by the childpointer of the chosen entry and repeat.
  *
  * --------------------------------------------------
  *
  * [determine the nearly minimum overlap cost]
  * Sort the rectangles in N in increasing order of their area enlargement needed to include the new data rectangle.
  * Let A be the group of the first p entries.
  * From the entries in A, considering all entries in N, choose the entry whose rectangle needs least overlap enlargement.
  * Resolve ties as described above.
  */
NodePtr RSTree::chooseSubtree(Mbr &mbr, int desiredLevel)
{
    NodePtr nPtr = root;
    while(nPtr->level > desiredLevel)
    {
        /// since (nPtr->level > desiredLevel), hence nPtr->level > 0, nPtr is not a leaf node
        NodePtrV &children = *nPtr->children;
        nPtr->prepareEnlargementSort(mbr);
        sort(children.begin(), children.end());

        int selectedIndex = 0;
        if(nPtr->level == 1) // if the child pointers point to leaf nodes
        {
            int p = min(nPtr->size(), NEAR_MINIMUM_OVERLAP_FACTOR);
            double minOverlapEnlarge = INF_P;
            for(int ic=0; ic<p; ic++)
            {
                Mbr base = children[ic]->mbr;
                Mbr newMbr = Mbr::getMbr(base, mbr);
                double overlapBase = 0, overlap = 0;
                for(int ico=0; ico<nPtr->size(); ico++)
                    if(ico!=ic)
                    {
                        overlapBase += Mbr::getOverlap(base, children[ico]->mbr);
                        overlap += Mbr::getOverlap(newMbr, children[ico]->mbr);
                    }
                double overlapEnlarge = overlap - overlapBase;
                if(overlapEnlarge < minOverlapEnlarge)
                {
                    minOverlapEnlarge = overlapEnlarge;
                    selectedIndex = ic;
                }
            }
        }

        nPtr = children[selectedIndex];
    }
    return nPtr;
}

/** Invoke ChooseSplitAxis to determine the axis, perpendicular to which the split is performed.
  *
  * Invoke ChooseSplitIndex to determine the best distribution into two groups along that axis.
  *
  * Distribute the entries into two groups.
  */
void RSTree::split(RSTNode &node)
{
    int axis = chooseSplitAxis(node);
    int splitIndex = chooseSplitIndex(node, axis);

    NodePtr newNPtr = new RSTNode(node.level);
    newNPtr->take(node, splitIndex);
    if(!node.parent)
    {
        root = new RSTNode(node.level + 1);
        root->insert(&node);
    }
    node.parent->insert(newNPtr);
}

/** For each axis:
  * Sort the entries by the lower then by the upper value of their rectangles;
  * And determine all distributions ( [m, M-m] elements in each partition );
  * Compute S, the sum of all margin-values of the different distributions.
  *
  * Choose the axis with the minimum S as split axis.
  */
int RSTree::chooseSplitAxis(RSTNode &node)
{
    double minS = INF_P;
    int axis = 0;
    for(int d=0; d<DIM; d++)
    {
        if(node.level)
            sort(node.children->begin(), node.children->end(), AxisSort(d));
        else
            sort(node.entries->begin(), node.entries->end(), AxisSort(d));
        double S = computeS(node);
        if(S < minS)
        {
            minS = S;
            axis = d;
        }
    }
    return axis;
}

double RSTree::computeS(RSTNode &node)
{
    double S = 0;
    int size = node.size(), last = size - 1;

    Mbr part1[size], part2[size];
    if(node.level)
    {
        NodePtrV &children = *node.children;
        part1[0] = Mbr::getMbr(children[0]->mbr, children[0]->mbr);
        for(int ic=1; ic<size - minChild; ic++)
            part1[ic] = Mbr::getMbr(part1[ic-1], children[ic]->mbr);
        part2[last] = Mbr::getMbr(children[last]->mbr, children[last]->mbr);
        for(int ic=last - 1; ic>=minChild; ic--)
            part2[ic] = Mbr::getMbr(part2[ic+1], children[ic]->mbr);
    }
    else
    {
        EntryPtrV &entries = *node.entries;
        part1[0] = Mbr::getMbr(entries[0]->mbr, entries[0]->mbr);
        for(int ie=1; ie<size - minChild; ie++)
            part1[ie] = Mbr::getMbr(part1[ie-1], entries[ie]->mbr);
        part2[last] = Mbr::getMbr(entries[last]->mbr, entries[last]->mbr);
        for(int ie=last-1; ie>=minChild; ie--)
            part2[ie] = Mbr::getMbr(part2[ie+1], entries[ie]->mbr);
    }

    // is : first element of a valid second partition
    for(int is=minChild; is<=size - minChild; is++)
        S += part1[is-1].getMargin() + part2[is].getMargin();
    return S;
}

/** Along the chosen axis, choose the distribution with the minimum overlap-value.
  *
  * Resolve ties by choosing the distribution with minimum area-value.
  *
  * Return:
  * Index of the first element in the second partition
  */
int RSTree::chooseSplitIndex(RSTNode &node, int axis)
{
    int size = node.size(), last = size - 1;

    Mbr part1[size], part2[size];
    if(node.level)
    {
        sort(node.children->begin(), node.children->end(), AxisSort(axis));
        NodePtrV &children = *node.children;
        part1[0] = Mbr::getMbr(children[0]->mbr, children[0]->mbr);
        for(int ic=1; ic<size - minChild; ic++)
            part1[ic] = Mbr::getMbr(part1[ic-1], children[ic]->mbr);
        part2[last] = Mbr::getMbr(children[last]->mbr, children[last]->mbr);
        for(int ic=last-1; ic>=minChild; ic--)
            part2[ic] = Mbr::getMbr(part2[ic+1], children[ic]->mbr);
    }
    else
    {
        sort(node.entries->begin(), node.entries->end(), AxisSort(axis));
        EntryPtrV &entries = *node.entries;
        part1[0] = Mbr::getMbr(entries[0]->mbr, entries[0]->mbr);
        for(int ie=1; ie<size-minChild; ie++)
            part1[ie] = Mbr::getMbr(part1[ie-1], entries[ie]->mbr);
        part2[last] = Mbr::getMbr(entries[last]->mbr, entries[last]->mbr);
        for(int ie=last-1; ie>=minChild; ie--)
            part2[ie] = Mbr::getMbr(part2[ie+1], entries[ie]->mbr);
    }

    double minOverlap = INF_P, minArea = INF_P;
    int splitIndex = minChild;
    // is : first element of a valid second partition
    for(int is=minChild; is<=size-minChild; is++)
    {
        double overlap = Mbr::getOverlap(part1[is-1], part2[is]);
        if(overlap - EPS < minOverlap) // smaller or roughly equal overlap
        {
            double area = part1[is-1].getArea() + part2[is].getArea();
            if(overlap + EPS > minOverlap) // roughly equal overlap
            {
                if(area < minArea)
                {
                    minArea = area;
                    splitIndex = is;
                }
            }
            else // smaller overlap
            {
                minOverlap = overlap;
                minArea = area;
                splitIndex = is;
            }
        }
    }

    return splitIndex;
}

/** For all M+1 entries of a node N, compute the distance between the centers of their rectangles and the center of the bounding rectangle of N.
  * Sort the entries in decreasing order of their distances computed.
  * Remove the first p entries from N and adjust the bounding rectangle of N.
  * In the sort, starting with the maximum distance (= far reinsert) or minimum distance (= close reinsert), invoke Insert to reinsert the entries.
  */
void RSTree::reInsert(RSTNode &node, int &overflowLevel)
{
    overflowLevel |= (1<<node.level);

    node.prepareDisSort();
    if(node.level)
        sort(node.children->begin(), node.children->end());
    else
        sort(node.entries->begin(), node.entries->end());

    int p = RSTree::maxChild * REINSERT_FACTOR;
    int reinsertFromIndex = node.size() - p;

    if(node.level) // If source is not a leaf node
    {
        NodePtrV children;
        for(unsigned int ic=reinsertFromIndex; ic<node.children->size(); ic++)
            children.push_back(node.children->at(ic));
        node.resize(reinsertFromIndex);
        for(unsigned int ic=0; ic<children.size(); ic++)
            insert(children[ic], NULL, node.level, overflowLevel);
    }
    else // if source is a leaf node
    {
        EntryPtrV entries;
        for(unsigned int ie=reinsertFromIndex; ie<node.entries->size(); ie++)
            entries.push_back(node.entries->at(ie));
        node.resize(reinsertFromIndex);
        for(unsigned int ie=0; ie<entries.size(); ie++)
            insert(NULL, entries[ie], node.level, overflowLevel);
    }
}
