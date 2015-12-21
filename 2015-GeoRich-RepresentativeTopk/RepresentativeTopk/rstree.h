#ifndef RSTREE_H
#define RSTREE_H

#include <vector>
#include <list>
#include <limits>
#include <cstdlib>
#include <algorithm>

#define INF_N -numeric_limits<double>::max()
#define INF_P numeric_limits<double>::max()

#define MAX_CHILD (pSize - sizeof(RSTNode)) / sizeof(RSTNode)

using namespace std;

const double EPS = 1e-9;

const int DIM = 3;
const int LEAF_LEVEL = 0;
const int PAGE_SIZE = 4096;

const double SPLIT_FACTOR = 0.4;
const double REINSERT_FACTOR = 0.3;
const int NEAR_MINIMUM_OVERLAP_FACTOR = 32;

/*******************************************************************************
 * Mbr - minimum bounding rectangle
 ******************************************************************************/

typedef struct Mbr
{
    double minCoord[DIM];
    double maxCoord[DIM];

    Mbr()
    {
        init();
    }
    Mbr(double coord[DIM])
    {
        for(int d=0; d<DIM; d++)
        {
            minCoord[d] = coord[d];
            maxCoord[d] = coord[d];
        }
    }

    void init();
    void enlarge(Mbr &addMbr);
    double getArea() const;
    double getMargin() const;
    double getCenter(int d);

    static double getEnlarge(Mbr &base, Mbr &addMbr);
    static Mbr getMbr(Mbr &mbr1, Mbr &mbr2);
    static double getOverlap(Mbr &mbr1, Mbr &mbr2);
}Mbr;

inline void Mbr::init()
{
    for(int d=0; d<DIM; d++)
    {
        minCoord[d] = INF_P;
        maxCoord[d] = INF_N;
    }
}

inline void Mbr::enlarge(Mbr &addMbr)
{
    for(int d=0; d<DIM; d++)
    {
        minCoord[d] = min(minCoord[d], addMbr.minCoord[d]);
        maxCoord[d] = max(maxCoord[d], addMbr.maxCoord[d]);
    }
}

inline double Mbr::getArea() const
{
    double area = 1;
    for(int d=0; d<DIM; d++)
        area *= maxCoord[d] - minCoord[d];
    return area;
}

inline double Mbr::getMargin() const
{
    double margin = 0;
    for(int d=0; d<DIM; d++)
        margin += maxCoord[d] - minCoord[d];
    return margin;
}

inline double Mbr::getCenter(int d)
{
    return (minCoord[d] + maxCoord[d]) * 0.5;
}

inline double Mbr::getEnlarge(Mbr &base, Mbr &addMbr)
{
    Mbr mbr = Mbr::getMbr(base, addMbr);
    return mbr.getArea() - base.getArea();
}

inline Mbr Mbr::getMbr(Mbr &mbr1, Mbr &mbr2)
{
    Mbr mbr;
    for(int d=0; d<DIM; d++)
    {
        mbr.minCoord[d] = min(mbr1.minCoord[d], mbr2.minCoord[d]);
        mbr.maxCoord[d] = max(mbr1.maxCoord[d], mbr2.maxCoord[d]);
    }
    return mbr;
}

inline double Mbr::getOverlap(Mbr &mbr1, Mbr &mbr2)
{
    double overlap = 1;
    for(int d=0; d<DIM; d++)
    {
        double maxMin = max(mbr1.minCoord[d], mbr2.minCoord[d]);
        double minMax = min(mbr1.maxCoord[d], mbr2.maxCoord[d]);
        if(maxMin>=minMax)
            return 0;
        overlap *= minMax - maxMin;
    }
    return overlap;
}

/*******************************************************************************
 * RTreeNode
 ******************************************************************************/

typedef struct LeafNodeEntry
{
    Mbr mbr;
    int dataID;
    double value;

    LeafNodeEntry(Mbr mbr, int dataID)
        : mbr(mbr), dataID(dataID), value(0)
        {
        }
}LeafNodeEntry;

typedef LeafNodeEntry* EntryPtr;
typedef vector<EntryPtr> EntryPtrV;

typedef struct RSTNode RSTNode;

typedef RSTNode* NodePtr;
typedef vector<NodePtr> NodePtrV;

struct RSTNode
{
    int level; // level of the node. all leaf nodes are at level 0.
    int aggregate;
    Mbr mbr;
    NodePtrV* children;
    EntryPtrV* entries;
    NodePtr parent;
    double value;

    bool operator < (const RSTNode& node) const;

    RSTNode(int level)
        : level(level), aggregate(0), children(NULL), entries(NULL), parent(NULL)
        {
            if(level)
                children = new NodePtrV();
            else
                entries = new EntryPtrV();
        }
    ~RSTNode()
    {
        if(children)
        {
            for(unsigned int ic=0; ic<children->size(); ic++)
                delete children->at(ic);
            delete children;
        }
        if(entries)
            delete entries;
    }

    inline int size();
    void insert(NodePtr cPtr);
    void insert(EntryPtr ePtr);
    void enlargeMbr(Mbr &addMbr);
    void increaseAggregate(int amount);
    void decreaseAggregate(int amount);
    void prepareEnlargementSort(Mbr &addMbr);
    void prepareDisSort();
    void take(RSTNode &source, int from);
    void resize(int from);
    void adjustMbr();
};

inline bool RSTNode::operator < (const RSTNode& node) const
{
    return value < node.value;
}

inline int RSTNode::size()
{
    return level ? children->size() : entries->size();
}

inline void RSTNode::insert(NodePtr cPtr)
{
    children->push_back(cPtr);
    cPtr->parent = this;
    enlargeMbr(cPtr->mbr);
    increaseAggregate(cPtr->aggregate);
}

inline void RSTNode::insert(EntryPtr ePtr)
{
    entries->push_back(ePtr);
    enlargeMbr(ePtr->mbr);
    increaseAggregate(1);
}

inline void RSTNode::enlargeMbr(Mbr &addMbr)
{
    mbr.enlarge(addMbr);
    if(parent)
        parent->enlargeMbr(addMbr);
}

inline void RSTNode::increaseAggregate(int amount)
{
    aggregate += amount;
    if(parent)
        parent->increaseAggregate(amount);
}

inline void RSTNode::decreaseAggregate(int amount)
{
    aggregate -= amount;
    if(parent)
        parent->decreaseAggregate(amount);
}

inline void RSTNode::prepareEnlargementSort(Mbr &addMbr)
{
    for(unsigned int ic=0; ic<children->size();ic++)
    {
        NodePtr cPtr = children->at(ic);
        cPtr->value = Mbr::getEnlarge(cPtr->mbr, addMbr);
    }
}

inline void RSTNode::prepareDisSort()
{
    double center[DIM];
    for(int d=0; d<DIM; d++)
        center[d] = mbr.getCenter(d);
    if(level)
        for(unsigned int ic=0; ic<children->size(); ic++)
        {
            NodePtr cPtr = children->at(ic);
            cPtr->value = 0;
            for(int d=0; d<DIM; d++)
            {
                double sideLength = cPtr->mbr.getCenter(d) - center[d];
                cPtr->value += sideLength * sideLength;
            }
        }
    else
        for(unsigned int ie=0; ie<entries->size(); ie++)
        {
            EntryPtr ePtr = entries->at(ie);
            ePtr->value = 0;
            for(int d=0; d<DIM; d++)
            {
                double sideLength = ePtr->mbr.getCenter(d) - center[d];
                ePtr->value += sideLength * sideLength;
            }
        }
}

inline void RSTNode::take(RSTNode &source, int from) // copy and remove elements from source
{
    if(source.level) // If source is not a leaf node
    {
        NodePtrV &sourceV = *source.children;
        for(unsigned int ic=from; ic<sourceV.size(); ic++)
            insert(sourceV[ic]);
    }
    else // if source is a leaf node
    {
        EntryPtrV &sourceV = *source.entries;
        for(unsigned int ie=from; ie<sourceV.size(); ie++)
            insert(sourceV[ie]);
    }
    source.resize(from);
}

inline void RSTNode::resize(int from)
{
    if(level) // if this is not a leaf node
    {
        children->resize(from);
        int newAggregate = 0;
        for(unsigned int ic=0; ic<children->size(); ic++)
            newAggregate += children->at(ic)->aggregate;
        decreaseAggregate(aggregate - newAggregate);
    }
    else // if this is a leaf node
    {
        entries->resize(from);
        decreaseAggregate(aggregate - from);
    }
    adjustMbr();
}

inline void RSTNode::adjustMbr()
{
    mbr.init();
    if(level) // if this is not a leaf node
        for(unsigned int ic=0; ic<children->size(); ic++)
            mbr.enlarge(children->at(ic)->mbr);
    else // if this is a leaf node
        for(unsigned int ie=0; ie<entries->size(); ie++)
            mbr.enlarge(entries->at(ie)->mbr);
    if(parent)
        parent->adjustMbr();
}

/*******************************************************************************
 * Sorting
 ******************************************************************************/

typedef struct AxisSort
{
    int axis;

    AxisSort(int axis)
        : axis(axis)
        {

        }

    bool operator() (NodePtr child1, NodePtr child2);
    bool operator() (EntryPtr entry1, EntryPtr entry2);
}AxisSort;

inline bool AxisSort::operator() (NodePtr child1, NodePtr child2)
{
    if(child1->mbr.minCoord[axis] == child2->mbr.minCoord[axis])
        return (child1->mbr.maxCoord[axis] < child2->mbr.maxCoord[axis]);
    return child1->mbr.minCoord[axis] < child2->mbr.minCoord[axis];
}

inline bool AxisSort::operator() (EntryPtr entry1, EntryPtr entry2)
{
    if(entry1->mbr.minCoord[axis] == entry2->mbr.minCoord[axis])
        return entry1->mbr.maxCoord[axis] < entry2->mbr.maxCoord[axis];
    return entry1->mbr.minCoord[axis] < entry2->mbr.minCoord[axis];
}

/*******************************************************************************
 * R Star Tree
 ******************************************************************************/

typedef struct RSTree
{
    public:
        const static int pSize = PAGE_SIZE; // page size in bytes
        const static int maxChild = MAX_CHILD; // M -- node capacity -- maximum number of entries
        static int minChild; // m -- minimum number of entries in a node

        RSTree()
            : root(NULL)
            {
                root = new RSTNode(LEAF_LEVEL);
            }
        ~RSTree()
        {
            if(root)
                delete root;
        }

        RSTNode* root;

        void insertData(EntryPtr ePtr);
    protected:
    private:
        /// Insertion
        void insert(NodePtr cPtr, EntryPtr ePtr, int desiredLevel, int &overflowLevel); // with OverflowTreatment

        /// Insertion - ChooseSubtree
        NodePtr chooseSubtree(Mbr &mbr, int desiredLevel);

        /// Insertion - OverflowTreatment - Split
        void split(RSTNode &node);
        int chooseSplitAxis(RSTNode &node);
        double computeS(RSTNode &node);
        int chooseSplitIndex(RSTNode &node, int axis);

        /// Insertion - OverflowTreatment - ReInsert
        void reInsert(RSTNode &node, int &overflowLevel);
}RSTree;

#endif // RSTREE_H
