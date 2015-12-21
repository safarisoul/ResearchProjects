#ifndef CONHULL_H
#define CONHULL_H

#include "RStarTreeUtil.h"
#include "Util2D.h"

typedef struct ConvexHull
{
    Vertex_V upperLeftHullVertex;
    Line_V upperLeftHullLine;

    Vertex_V upperRightHullVertex;
    Line_V upperRightHullLine;

    Vertex_V lowerLeftHullVertex;
    Line_V lowerLeftHullLine;

    Vertex_V lowerRightHullVertex;
    Line_V lowerRightHullLine;

    Vertex minX;
    Vertex minY;
    Vertex maxX;
    Vertex maxY;

    size_t size();
}ConvexHull;

inline size_t ConvexHull::size()
{
    size_t cnt = 0;
    Vertex first;

    first = upperLeftHullVertex.at(0);
    for(size_t iv = 1; iv < upperLeftHullVertex.size(); iv++)
    {
        Vertex& cur = upperLeftHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        cnt += upperLeftHullVertex.size() - iv;
        break;
    }

    first = upperRightHullVertex.at(0);
    for(size_t iv = 1; iv < upperRightHullVertex.size(); iv++)
    {
        Vertex& cur = upperRightHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        cnt += upperRightHullVertex.size() - iv;
        break;
    }

    first = lowerLeftHullVertex.at(0);
    for(size_t iv = 1; iv < lowerLeftHullVertex.size(); iv++)
    {
        Vertex& cur = lowerLeftHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        cnt += lowerLeftHullVertex.size() - iv;
        break;
    }

    first = lowerRightHullVertex.at(0);
    for(size_t iv = 1; iv < lowerRightHullVertex.size(); iv++)
    {
        Vertex& cur = lowerRightHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        cnt += lowerRightHullVertex.size() - iv;
        break;
    }

    return cnt;
}

class ConHull
{
    public:
        ConHull() { }

        // convex hull
        static bool convexHull(RStarTree& tree, ConvexHull& hull);

        // k-th convex hull, k != 1
        static bool convexHull(RStarTree& tree, ConvexHull& hull, ConvexHull& outer);

    protected:
    private:
        // convex hull
        static bool init(RStarTree& tree, ConvexHull& hull);
        static bool minX(RStarTree& tree, ConvexHull& hull);
        static bool minY(RStarTree& tree, ConvexHull& hull);
        static bool maxX(RStarTree& tree, ConvexHull& hull);
        static bool maxY(RStarTree& tree, ConvexHull& hull);

        static Coord maxDis(Mbr& mbr, ConvexHull& hull);
        static Coord maxDis(Vertex& vertex, ConvexHull& hull);

        // k-th convex hull, k != 1
        static bool init(RStarTree& tree, ConvexHull& hull, ConvexHull& outer);
        static bool minX(RStarTree& tree, ConvexHull& hull, ConvexHull& outer);
        static bool minY(RStarTree& tree, ConvexHull& hull, ConvexHull& outer);
        static bool maxX(RStarTree& tree, ConvexHull& hull, ConvexHull& outer);
        static bool maxY(RStarTree& tree, ConvexHull& hull, ConvexHull& outer);

        static bool exclude(ConvexHull& outer, Vertex& vertex);

        // union
        static void unionV(ConvexHull& hull, Vertex& vertex);
        static void unionVUpper(Vertex_V& upperHullVertex, Line_V& upperHullLine, Vertex& vertex);
        static void unionVLower(Vertex_V& lowerHullVertex, Line_V& lowerHullLine, Vertex& vertex);
};

#endif // CONHULL_H
