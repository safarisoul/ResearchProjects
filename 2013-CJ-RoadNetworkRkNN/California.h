#ifndef CALIFORNIA_H
#define CALIFORNIA_H

#include <fstream>

#include "Data.h"
#include "Argument.h"

#define NUM_NODES 21048
#define NUM_EDGES 21693
#define NUM_POINTS 87635
#define NUM_CATEGORIES 63

struct CPoint
{
    friend std::istream& operator>>(std::istream&, CPoint&);
    friend std::ostream& operator<<(std::ostream&, const CPoint&);

    CPoint():categoryID(BAD_ID) { };

    int categoryID;
    double dis;
    double longitude;
    double latitude;

    inline bool isGood() { return categoryID != BAD_ID; }
};

struct CNode
{
    friend std::istream& operator>>(std::istream&, CNode&);
    friend std::ostream& operator<<(std::ostream&, const CNode&);

    CNode(): nodeID(BAD_ID), longitude(0.0), latitude(0.0) { }

    int nodeID;
    double longitude;
    double latitude;

    inline bool isGood() { return nodeID != BAD_ID; }
};

struct CEdge
{
    friend std::istream& operator>>(std::istream&, CEdge&);
    friend std::ostream& operator<<(std::ostream&, const CEdge&);

    CEdge(): edgeID(BAD_ID), startNodeID(0), endNodeID(0), l2dis(0.0) { }

    int edgeID;
    int startNodeID;
    int endNodeID;
    double l2dis;

    inline bool isGood() { return edgeID != BAD_ID; }
};

struct CMap
{
    friend std::istream& operator>>(std::istream&, CMap&);
    friend std::ostream& operator<<(std::ostream&, const CMap&);

    CMap(): edgeID(BAD_ID), startNodeID(0), endNodeID(0), numPoints(0), length(BAD_DISTANCE) { }

    int edgeID;
    int startNodeID;
    int endNodeID;
    int numPoints;
    double length;
    vector<CPoint> points;

    inline bool isGood() { return length != BAD_DISTANCE; }
};

class CaliforniaDataLoader
{
    public:
        CaliforniaDataLoader(): numNodes(0), numEdges(0), numMap(0) { };

        void load();
        void checkData();

    protected:

    private:
        int numNodes;
        vector<CNode> nodes;
        int numEdges;
        vector<CEdge> edges;
        int numMap;
        vector<CMap> maps;
        int numPoints;
        map<int, vector<CPoint> > points;
        vector<int> facilityID;

        void readNodes();
        void readEdges();
        void readMap();

        void addNodes();
        void addEdges();
        void addPoints();

        void addIndex();
};

#endif // CALIFORNIA_H
