#ifndef CHFC_H
#define CHFC_H

#include "ConHull.h"

typedef ConvexPolygon FVC1;

typedef struct FVC
{
    FVC()
        : phi(false)
        { }

    HalfSpace_V hSpaces;
    bool phi;

    void prune(HalfSpace& halfSpace);
}FVC;

inline void FVC::prune(HalfSpace& halfSpace)
{
    hSpaces.push_back(halfSpace);
}

class CHFC
{
    public:
        CHFC() { }

        static void chfc(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult);
    protected:
    private:
        // FVC1
        static void furVorCell(RStarTree& fTree, Point& query, FVC1& fvc1);
        static void rangeQuery(RStarTree& cTree, FVC1& fvc1, Result& result, Result& notResult);
        static bool contain(FVC1& fvc1, Vertex& data);
        static void addResult(Result& result, Node_P nodePtr);

        // FVC
        static void furVorCell(RStarTree& fTree, Point& query, size_t k, FVC& fvc);
        static bool contain(ConvexHull& ch, Vertex& q);
        static void updateFVC(ConvexHull& ch, FVC& fvc, Vertex& q);
        static void updateFVC(Vertex& p, FVC& fvc, Vertex& q);

        // range query
        static void rangeQuery(RStarTree& cTree, FVC& fvc, size_t k, Result& result, Result& notResult);
        static bool contain(FVC& fvc, Vertex& data, size_t k);
};

#endif // CHFC_H
