#ifndef SKYRIDER_H
#define SKYRIDER_H

#include "RStarTreeUtil.h"
#include "Util2D.h"

typedef pair<Coord, Coord> Junctions; // <entering, departing> junctions

typedef struct Envelope
{
    Vertex_V vertex;
    Line_V line;
}Envelope;

typedef struct KDepthContour
{
    Vertex_V upperHullVertex;
    Line_V upperHullLine;

    Vertex_V lowerHullVertex;
    Line_V lowerHullLine;

    Vertex left;
    Vertex right;
    Line axis;

    void print()
    {
        ofstream kdcOFS("data/debug/kdc.txt");
        assert(kdcOFS.good());
        for(size_t iv = 1; iv < upperHullVertex.size(); iv++)
        {
            Vertex pre = upperHullVertex.at(iv - 1);
            Vertex cur = upperHullVertex.at(iv);
            kdcOFS << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        for(size_t iv = 1; iv < lowerHullVertex.size(); iv++)
        {
            Vertex pre = lowerHullVertex.at(iv - 1);
            Vertex cur = lowerHullVertex.at(iv);
            kdcOFS << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        kdcOFS.close();
    }
}KDepthContour;

class SkyRider
{
    public:
        SkyRider() { }

        static void kDepthContour(RStarTree& tree, KDepthContour& contour, size_t k);

    protected:
    private:
        // envelope
        static void upperEnvelopeStart(RStarTree& tree, size_t k, Line& start);
        static void lowerEnvelopeStart(RStarTree& tree, size_t k, Line& start);
        static void envelope(RStarTree& tree, Envelope& envelope, Line& start);
        static bool getIntersection(RStarTree& tree, Line& lc, Coord& loc, Vertex& candidate, Line& next);
        //static bool getIntersectionBF(RStarTree& tree, Line& lc, Coord& loc, Vertex& candidate, Line& next);

        //static void validateEnvelope(RTree& tree, Envelope& upperEnvelope, Envelope& lowerEnvelope, size_t k);
        //static void cnt(RTree& tree, Coord x, Line& cur, size_t& cntAbove, size_t& cntBelow);
        //static bool find(Node_P nodePtr, Coord x, Coord y);

        // hull line
        static void hull(Line_V& hullLine, Envelope& envelope, int turn);
        static bool checkTurn(Vertex_V& vertex, vector<int>& pre, size_t iv, int wrongTurn);

        //static void validateCandidateHull(RTree& tree, Line_V& upperHullLine, Line_V& lowerHullLine, size_t k);
        //static void cntP(RTree& tree, Line& cur, size_t& cntAbove, size_t& cntBelow, size_t& cntOn);

        // contour
        static void constructContour(KDepthContour& contour, Line_V& upperHullLine, Line_V& lowerHullLine);

        //static void validateContour(KDepthContour& contour);

        // inline functions
        static Coord enteringJunction(Line& l1, Line& l2);
        static Junctions junction(Line& line, Mbr& mbr);
};

inline Coord SkyRider::enteringJunction(Line& l1, Line& l2)
{
    return (l2.b - l1.b) / (l1.a - l2.a);
}

inline Junctions SkyRider::junction(Line& line, Mbr& mbr)
{
    if(line.a >= mbr.coord[0][0] && line.a <= mbr.coord[0][1] && line.b >= mbr.coord[1][0] && line.b <= mbr.coord[1][1])
        return make_pair(INF_N, INF_P);

    Line ul;
    ul.a = mbr.coord[0][0];
    ul.b = mbr.coord[1][1];
    Line ur;
    ur.a = mbr.coord[0][1];
    ur.b = mbr.coord[1][1];
    Line ll;
    ll.a = mbr.coord[0][1];
    ll.b = mbr.coord[1][0];
    Line lr;
    lr.a = mbr.coord[0][0];
    lr.b = mbr.coord[1][0];

    Vertex upper, lower;
    Util2D::lineCross(ul, ur, upper);
    Util2D::lineCross(ll, lr, lower);

    Vertex crossUL, crossUR;
    Util2D::lineCross(line, ul, crossUL);
    Util2D::lineCross(line, ur, crossUR);
    if(crossUL.x - EPS < upper.x && crossUR.x + EPS > upper.x)
        return make_pair(INF_N, INF_P);

    Vertex crossLL, crossLR;
    Util2D::lineCross(line, ll, crossLL);
    Util2D::lineCross(line, lr, crossLR);
    if(crossLL.x - EPS < upper.x && crossLR.x + EPS > upper.x)
        return make_pair(INF_N, INF_P);

    Coord entering = INF_P, departing = INF_N, x;

    x = enteringJunction(line, ul);
    entering = min(entering, x);
    departing = max(departing, x);

    x = enteringJunction(line, ur);
    entering = min(entering, x);
    departing = max(departing, x);

    x = enteringJunction(line, ll);
    entering = min(entering, x);
    departing = max(departing, x);

    x = enteringJunction(line, lr);
    entering = min(entering, x);
    departing = max(departing, x);

    return make_pair(entering, departing);
}

#endif // SKYRIDER_H
