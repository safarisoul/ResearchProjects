#ifndef DCCR_H
#define DCCR_H

#include "BBS.h"
#include "SkyRider.h"

typedef struct Tangent
{
    Vertex tPoint1;
    Line tLine1;
    Vertex tPoint2;
    Line tLine2;

    Line connection;
}Tangent;

typedef ConvexPolygon CandidateRegion;

typedef struct RadiansPoint
{
    RadiansPoint()
        : cw(0), ccw(0)
        { }

    Radians angle;
    Vertex point;
    size_t cw; // clockwise
    size_t ccw; // counterclockwise

    bool operator< (const RadiansPoint& rp) const;
}RadiansPoint;

inline bool RadiansPoint::operator< (const RadiansPoint& rp) const
{
    return angle < rp.angle;
}

typedef vector<RadiansPoint> RadiansPoint_V;

// counterclockwise
typedef struct RadiansRange
{
    RadiansRange()
        : ok(false)
        { }

    Radians from;
    Radians to;
    HalfSpace start;
    HalfSpace end;
    bool ok;
}RadiansRange;

typedef vector<RadiansRange> RadiansRange_V;

typedef struct Sector
{
    Radians from;
    Radians to;
    Coord dis;
}Sector;

typedef list<Sector> Sector_L;

class DCCR
{
    public:
        DCCR() { };

        static void rkfn(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult);
        static void vara(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult);
        static void tpru(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult);

    protected:
    private:
        // candidate region
        static void candidateRegion(RStarTree& fTree, RStarTree& band, Vertex& q, size_t k, KDepthContour& kdc, CandidateRegion& cr);

        static bool enclose(KDepthContour& kdc, Vertex& q);
        static void tangent(KDepthContour& kdc, Vertex& q, Tangent& tangent);
        static void initPruners(KDepthContour& kdc, Tangent& tangent, Vertex& q, Vertex_V& pruners);
        static void updatePruners(CandidateRegion& cr, Vertex_V& pruners, Vertex& q);

        static void candidateRange(RadiansRange_V& crr, RadiansPoint_V& skirt_v, size_t k, Vertex& q);

    public:
        // outer skirt
        static void outerSkirt(RStarTree& band, RStarTree& skirt, RadiansPoint_V& skirt_v, KDepthContour& kdc, Vertex& q);
        static void insert(RStarTree& skirt, RadiansPoint_V& skirt_v, Node_P nodePtr, Vertex& q);
        static int contain(KDepthContour& kdc, Mbr& mbr);
        static int contain(KDepthContour& kdc, Vertex& p);
    private:

        // filtering and verification
        static void filterVerify(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr);
        static void filterVerifyS(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr);
        static void tpruFVCR(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr);
        static void tpruFVCRR(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr);
        static void tpruFVB(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr);
        static void tpruFVS(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr);
        static void filterVerify(RStarTree& cTree, CandidateRegion& cr, Result& result, Result& notResult);
        static void addResult(Result& result, Node_P nodePtr);

        static const int CONTAIN_COMPLETE = 2;
        static const int CONTAIN_PARTIAL = 1;
        static const int CONTAIN_NOT = 0;
        static int contain(CandidateRegion& cr, Entry_P entryPtr);
        static int contain(CandidateRegion& cr, Node_P nodePtr);
        static int contain(RadiansRange_V& crr, Vertex& p, Vertex& q);
        static int contain(RadiansRange_V& crr, Entry_P entryPtr, Vertex& q);
        static int contain(RadiansRange_V& crr, Node_P nodePtr, Vertex& q);
        static void minmax(RStarTree& skirt, Point& query, Vertex& q, Mbr& mbr, size_t& min, size_t& max);

        static void getSector(Node_P nodePtr, Vertex& q, Sector& sec);
        static void addSector(Sector_L& secs, Sector& sec);
        static int contain(Sector_L& secs, Node_P nodePtr, Vertex& q);
        static int contain(Sector_L& secs, Entry_P entryPtr, Vertex& q);
        static int contain(Sector_L& secs, Radians from, Radians to, Coord dis);

        // inline functions
        static bool verifyTangent(Line& tLine, Vertex& prePoint, Vertex& nextPoint);
        static size_t addTangent(Tangent& tangent, Vertex& tPoint, Line& tLine, size_t& n);
        //static void halfSpace(Vertex& p, Vertex& q, HalfSpace& hSpace);
        static bool contain(Mbr& mbr, Vertex& v);
        //static Coord minDis2(Mbr& mbr1, Mbr& mbr2);
        //static Coord maxDis2(Mbr& mbr1, Mbr& mbr2);
};

inline bool DCCR::verifyTangent(Line& tLine, Vertex& prePoint, Vertex& nextPoint)
{
    int pre = Util2D::isAboveLine(tLine, prePoint);
    int next = Util2D::isAboveLine(tLine, nextPoint);
    return pre == next || pre == Util2D::AROUND || next == Util2D::AROUND;
}

inline size_t DCCR::addTangent(Tangent& tangent, Vertex& tPoint, Line& tLine, size_t& n)
{
    if(n == 0)
    {
        tangent.tPoint1 = tPoint;
        tangent.tLine1 = tLine;
        n++;
    }
    else
    {
        if(Util2D::isAboveLine(tangent.tLine1, tPoint) == Util2D::AROUND)
            return n;
        if(n == 1)
        {
            tangent.tPoint2 = tPoint;
            tangent.tLine2 = tLine;
            n++;
        }
    }
    return n;
}

inline bool DCCR::contain(Mbr& mbr, Vertex& v)
{
    return v.x + EPS > mbr.coord[0][0] && v.x - EPS < mbr.coord[0][1] && v.y + EPS > mbr.coord[1][0] && v.y - EPS < mbr.coord[1][1];
}
/*
inline Coord DCCR::minDis2(Mbr& mbr1, Mbr& mbr2)
{
    Coord dis = 0;
    for(size_t dim = 0; dim < DIM; dim++)
        if(mbr1.coord[dim][0] > mbr2.coord[dim][1])
        {
            Coord diff = mbr1.coord[dim][0] - mbr2.coord[dim][1];
            dis += diff * diff;
        }
        else if(mbr1.coord[dim][1] < mbr2.coord[dim][0])
        {
            Coord diff = mbr2.coord[dim][0] - mbr1.coord[dim][1];
            dis += diff * diff;
        }
    return dis;
}

inline Coord DCCR::maxDis2(Mbr& mbr1, Mbr& mbr2)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++)
    {
        double diff = max(abs(mbr1.coord[dim][0] - mbr2.coord[dim][1]), abs(mbr1.coord[dim][1] - mbr2.coord[dim][0]));
        dis += diff * diff;
    }
    return dis;
}
*/
#endif // DCCR_H
