#ifndef NORTHAMERICA_H
#define NORTHAMERICA_H

#include <fstream>

#include "Data.h"
#include "Argument.h"

#define NUM_NODES_NA 175813
#define NUM_EDGES_NA 179179
#define NUM_POINTS_NA 876350
#define NUM_CATEGORIES_NA 1

struct NAPoint
{
    friend std::istream& operator>>(std::istream&, NAPoint&);
    friend std::ostream& operator<<(std::ostream&, const NAPoint&);

    NAPoint():categoryID(BAD_ID) { };

    int categoryID;
    double dis;
    double longitude;
    double latitude;

    inline bool isGood() { return categoryID != BAD_ID; }
};

struct NANode
{
    friend std::istream& operator>>(std::istream&, NANode&);
    friend std::ostream& operator<<(std::ostream&, const NANode&);

    NANode(): nodeID(BAD_ID), longitude(0.0), latitude(0.0) { }

    int nodeID;
    double longitude;
    double latitude;

    inline bool isGood() { return nodeID != BAD_ID; }
};

struct NAEdge
{
    friend std::istream& operator>>(std::istream&, NAEdge&);
    friend std::ostream& operator<<(std::ostream&, const NAEdge&);

    NAEdge(): edgeID(BAD_ID), startNodeID(0), endNodeID(0), l2dis(0.0) { }

    int edgeID;
    int startNodeID;
    int endNodeID;
    double l2dis;

    inline bool isGood() { return edgeID != BAD_ID; }
};

struct NAMap
{
    friend std::istream& operator>>(std::istream&, NAMap&);
    friend std::ostream& operator<<(std::ostream&, const NAMap&);

    NAMap(): edgeID(BAD_ID), startNodeID(0), endNodeID(0), numPoints(0), length(BAD_DISTANCE) { }

    int edgeID;
    int startNodeID;
    int endNodeID;
    int numPoints;
    double length;
    vector<NAPoint> points;

    inline bool isGood() { return length != BAD_DISTANCE; }
};

class NorthAmericaDataLoader
{
    public:
        NorthAmericaDataLoader(): numNodes(0), numEdges(0), numMap(0) { srand((unsigned)time(0)); };

        void load();
        void checkData();

        void generateMap();

        inline int getRandom(int max) { return rand() % max; }; // [0, max)
        inline double getRandom() { double num = rand() * 1e-9; return num - (int) num; }; // (0, 1)

    protected:

    private:
        int numNodes;
        vector<NANode> nodes;
        int numEdges;
        vector<NAEdge> edges;
        int numMap;
        vector<NAMap> maps;
        int numPoints;
        map<int, vector<NAPoint> > points;
        vector<int> facilityID;

        void readNodes();
        void readEdges();
        void readMap();

        void addNodes();
        void addEdges();
        void addPoints();

        void addIndex();
};

#endif // NORTHAMERICA_H
