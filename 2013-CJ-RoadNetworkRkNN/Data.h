#ifndef DATA_H
#define DATA_H

#include <vector>
#include <map>
#include <set>

#include <limits>

#include "Argument.h"

#define BAD_ID (1<<30)
#define BAD_DISTANCE -1
#define BAD_K 0

#define ptrNULL 0

/**********************************************************************
 * Data object ID types
 **********************************************************************/

typedef unsigned int NodeID_t;
typedef unsigned int EdgeID_t;
typedef unsigned int CategoryID_t;
typedef unsigned int PointID_t;
typedef unsigned int ObjectID_t;

/**********************************************************************
 * Simple object ID collection structures
 **********************************************************************/

typedef set<NodeID_t> NIDSet_t;
typedef set<EdgeID_t> EIDSet_t;
typedef set<PointID_t> PIDSet_t;

typedef vector<NodeID_t> NIDV_t;
typedef vector<PointID_t> PIDV_t;

/**********************************************************************
 * Data structures used by algorithms
 **********************************************************************/

typedef pair<Distance_t, NodeID_t> DisNIDPair_t;
typedef set<DisNIDPair_t> NIDMinHeap_t;
typedef map<PointID_t, NIDMinHeap_t> PIDNIDMinHeapMap_t;
typedef map<NodeID_t, Distance_t> NIDDisMap_t;
typedef map<PointID_t, NIDDisMap_t> PIDNIDDisMap_t;
typedef map<PointID_t, Distance_t> PIDDisMap_t;
typedef map<NodeID_t, PIDDisMap_t> NIDPIDDisMap_t;

/**********************************************************************
 * Data structures to represent subnetwork
 **********************************************************************/

typedef pair<Distance_t, Distance_t> Segment_t;
typedef set<Segment_t> SegmentSet_t;

/**********************************************************************
 * Data structures for holding calculation result
 **********************************************************************/

typedef map<EdgeID_t, SegmentSet_t> EIDSegmentSetMap_t;
typedef map<PointID_t, SegmentSet_t> PIDSegmentSetMap_t;

struct InfluenceZone
{
    friend std::ostream& operator<<(std::ostream&, const InfluenceZone&);

    InfluenceZone()
        : zoneNodes(ptrNULL), fullEdges(ptrNULL), partialEdges(ptrNULL), touchedNodes(ptrNULL), touchedEdges(ptrNULL)
        { zoneNodes = new NIDSet_t();
          fullEdges = new EIDSet_t();
          partialEdges = new EIDSegmentSetMap_t();
          touchedNodes = new NIDSet_t();
          touchedEdges = new EIDSet_t(); };

    NIDSet_t* zoneNodes;
    EIDSet_t* fullEdges;
    EIDSegmentSetMap_t* partialEdges;

    NIDSet_t* touchedNodes;
    EIDSet_t* touchedEdges;

    void clean() { if(zoneNodes) delete zoneNodes;
                   if(fullEdges) delete fullEdges;
                   if(partialEdges) delete partialEdges;
                   if(touchedNodes) delete touchedNodes;
                   if(touchedEdges) delete touchedEdges; };
};

/**********************************************************************
 * Update Data structure
 **********************************************************************/

struct BoundaryID_t
{
    friend std::ostream& operator<<(std::ostream&, const BoundaryID_t&);

    friend bool operator==(const BoundaryID_t &bid1, const BoundaryID_t &bid2);
    friend bool operator!=(const BoundaryID_t &bid1, const BoundaryID_t &bid2);
    friend bool operator<(const BoundaryID_t &bid1, const BoundaryID_t &bid2);
    friend bool operator<=(const BoundaryID_t &bid1, const BoundaryID_t &bid2);
    friend bool operator>(const BoundaryID_t &bid1, const BoundaryID_t &bid2);
    friend bool operator>=(const BoundaryID_t &bid1, const BoundaryID_t &bid2);

    BoundaryID_t()
        : query(BAD_ID), edgeID(BAD_ID), dis(BAD_DISTANCE), innerNode(BAD_ID)
        { };

    BoundaryID_t(PointID_t pid, EdgeID_t eid, Distance_t distance)
        : query(pid), edgeID(eid), dis(distance), innerNode(BAD_ID)
        { };

    BoundaryID_t(PointID_t pid, EdgeID_t eid, Distance_t distance, NodeID_t inNode)
        : query(pid), edgeID(eid), dis(distance), innerNode(inNode)
        { };

    PointID_t query;
    EdgeID_t edgeID;
    Distance_t dis;
    NodeID_t innerNode;
};

typedef map<BoundaryID_t, Distance_t> BIDDisMap_t;
typedef set<BoundaryID_t> BIDSet_t;
typedef map<BoundaryID_t, SegmentSet_t> BIDSegmentSetMap_t;

struct BoundaryZone
{
    BoundaryZone()
        : zoneNodes(ptrNULL), fullEdges(ptrNULL), partialEdges(ptrNULL)
        { zoneNodes = new NIDSet_t();
          fullEdges = new EIDSet_t();
          partialEdges = new EIDSegmentSetMap_t(); };

    NIDSet_t* zoneNodes;
    EIDSet_t* fullEdges;
    EIDSegmentSetMap_t* partialEdges;

    void clean() { if(zoneNodes) delete zoneNodes;
                   if(fullEdges) delete fullEdges;
                   if(partialEdges) delete partialEdges; };
};

/**********************************************************************
 * Data structures for holding sac result
 **********************************************************************/

struct UnprunedNetwork
{
    friend std::ostream& operator<<(std::ostream&, const UnprunedNetwork&);

    UnprunedNetwork()
        : nodesDis(ptrNULL), fullEdges(ptrNULL), partialEdges(ptrNULL)
        { nodesDis = new NIDDisMap_t();
          fullEdges = new EIDSet_t();
          partialEdges = new EIDSegmentSetMap_t(); };

    NIDDisMap_t* nodesDis;
    EIDSet_t* fullEdges;
    EIDSegmentSetMap_t* partialEdges;

    void clean() { if(nodesDis) delete nodesDis;
                   if(fullEdges) delete fullEdges;
                   if(partialEdges) delete partialEdges; };
};

typedef bool ToSafeRegionStart_t;
typedef pair<PointID_t, ToSafeRegionStart_t> PointRegionPath;
typedef map<PointRegionPath, Distance_t> FacilityDisMap_t;

struct MonitoredNetwork
{
    MonitoredNetwork()
        : validOnEdge(BAD_ID)
        { };

    MonitoredNetwork(EdgeID_t eid, Segment_t segment)
        : validOnEdge(eid), validOnSegment(segment)
        { };

    EdgeID_t validOnEdge;
    Segment_t validOnSegment; // safe region
    PIDSet_t preferredFacilities;
    FacilityDisMap_t preferrence;

    inline bool isValid(EdgeID_t eid, Distance_t loc) { return (validOnEdge==eid && loc>=validOnSegment.first && loc <= validOnSegment.second); };
    inline void addPreferredFacility(PointID_t pid) { preferredFacilities.insert(pid); };
    inline void addPreferrence(PointID_t pid, ToSafeRegionStart_t isToStart, Distance_t dis) { preferrence[make_pair(pid, isToStart)] = dis; };
};

typedef map<PointID_t, MonitoredNetwork> MonNetworkMap_t;

/**********************************************************************
 * Data structures for facilities
 **********************************************************************/

struct Point
{
    Point()
        : pointID(BAD_ID), categoryID(BAD_ID), edgeID(BAD_ID), disToStart(BAD_DISTANCE), k(BAD_K),
          unprunedNetworkNodesDis(ptrNULL)
        { unprunedNetworkNodesDis = new NIDDisMap_t();
          boundaryDis = new BIDDisMap_t(); };

    Point(CategoryID_t cid, EdgeID_t eid, Distance_t distance)
        : pointID(BAD_ID), categoryID(cid), edgeID(eid), disToStart(distance), k(BAD_K),
          unprunedNetworkNodesDis(ptrNULL)
        { unprunedNetworkNodesDis = new NIDDisMap_t();
          boundaryDis = new BIDDisMap_t(); };

    Point(PointID_t pid, CategoryID_t cid, EdgeID_t eid, Distance_t distance)
        : pointID(pid), categoryID(cid), edgeID(eid), disToStart(distance), k(BAD_K),
          unprunedNetworkNodesDis(ptrNULL)
        { unprunedNetworkNodesDis = new NIDDisMap_t();
          boundaryDis = new BIDDisMap_t(); };

    PointID_t pointID;
    CategoryID_t categoryID;
    EdgeID_t edgeID;
    Distance_t disToStart;

    Amount_t k;

    inline bool isQuery() { return k; };

    /// for SAC
    NIDDisMap_t* unprunedNetworkNodesDis;

    /// for update
    BIDDisMap_t* boundaryDis;

    inline void addUnprunedNetworkNodeDis(NodeID_t nid, Distance_t dis) { unprunedNetworkNodesDis->insert(make_pair(nid, dis)); };

    inline void addBoundaryDis(BoundaryID_t bid, Distance_t dis) { boundaryDis->insert(make_pair(bid, dis)); };

    void clean() { if(unprunedNetworkNodesDis) delete unprunedNetworkNodesDis;
                   if(boundaryDis) delete boundaryDis; };
};

struct Object
{
    friend std::istream& operator>>(std::istream&, Object&);
    friend std::ostream& operator<<(std::ostream&, const Object&);

    Object()
        : objectID(BAD_ID), preEdgeID(BAD_ID), preDis(BAD_DISTANCE), curEdgeID(BAD_ID), curDis(BAD_DISTANCE), speed(BAD_DISTANCE),
          monitoredNetworkMap(ptrNULL)
        { monitoredNetworkMap = new MonNetworkMap_t(); };

    Object(ObjectID_t oid, EdgeID_t pEid, Distance_t pDis, EdgeID_t cEid, Distance_t cDis, Distance_t s)
        : objectID(oid), preEdgeID(pEid), preDis(pDis), curEdgeID(cEid), curDis(cDis), speed(s),
          monitoredNetworkMap(ptrNULL)
        { monitoredNetworkMap = new MonNetworkMap_t(); };

    ObjectID_t objectID;
    EdgeID_t preEdgeID;
    Distance_t preDis;
    EdgeID_t curEdgeID;
    Distance_t curDis;
    Distance_t speed;

    /// SAC
    MonNetworkMap_t* monitoredNetworkMap;

    inline void addMonitoredNetwork(PointID_t pid, MonitoredNetwork monNetwork) { monitoredNetworkMap->insert(make_pair(pid, monNetwork)); };

    void clean() { if(monitoredNetworkMap) delete monitoredNetworkMap; };
};

/**********************************************************************
 * Data structures for network
 **********************************************************************/

 struct Node
{
    Node()
        : nodeID(BAD_ID), longitude(0.0), latitude(0.0)
        { queryAsInnerNode = new PIDSet_t(); };

    Node(Distance_t longi, Distance_t lati)
        : nodeID(BAD_ID), longitude(longi), latitude(lati)
        { queryAsInnerNode = new PIDSet_t(); };

    Node(NodeID_t nid, Distance_t longi, Distance_t lati)
        : nodeID(nid), longitude(longi), latitude(lati)
        { queryAsInnerNode = new PIDSet_t(); };

    NodeID_t nodeID;
    Distance_t longitude;
    Distance_t latitude;

    PIDSet_t* queryAsInnerNode;

    inline void addQueryAsInnerNode(PointID_t queryPointID) { queryAsInnerNode->insert(queryPointID); };
    inline void removeQueryAsInnerNode(PointID_t queryPointID) { queryAsInnerNode->erase(queryPointID); };

    inline bool isInnerNode(PointID_t queryPointID) { return queryAsInnerNode->find(queryPointID) != queryAsInnerNode->end(); };

    void clean() { if(queryAsInnerNode) delete queryAsInnerNode; };
};

typedef pair<Distance_t, PointID_t> DisPIDPair_t;
typedef set<DisPIDPair_t> DisPIDPairSet_t;

struct Edge
{
    Edge()
        : edgeID(BAD_ID), start(BAD_ID), end(BAD_ID), length(BAD_DISTANCE),
          fullEdgeQueries(ptrNULL), partialEdgeQueries(ptrNULL), fullEdgeBoundaries(ptrNULL), partialEdgeBoundaries(ptrNULL)
        { points = new DisPIDPairSet_t();
          closedPoints = new DisPIDPairSet_t();
          fullEdgeQueries = new PIDSet_t();
          partialEdgeQueries = new PIDSegmentSetMap_t();
          fullEdgeBoundaries = new BIDSet_t();
          partialEdgeBoundaries = new BIDSegmentSetMap_t(); };

    Edge(NodeID_t from, NodeID_t to, Distance_t len)
        : edgeID(BAD_ID), start(from), end(to), length(len),
          fullEdgeQueries(ptrNULL), partialEdgeQueries(ptrNULL), fullEdgeBoundaries(ptrNULL), partialEdgeBoundaries(ptrNULL)
        { points = new DisPIDPairSet_t();
          closedPoints = new DisPIDPairSet_t();
          fullEdgeQueries = new PIDSet_t();
          partialEdgeQueries = new PIDSegmentSetMap_t();
          fullEdgeBoundaries = new BIDSet_t();
          partialEdgeBoundaries = new BIDSegmentSetMap_t(); };

    Edge(EdgeID_t eid, NodeID_t from, NodeID_t to, Distance_t len)
        : edgeID(eid), start(from), end(to), length(len),
          fullEdgeQueries(ptrNULL), partialEdgeQueries(ptrNULL), fullEdgeBoundaries(ptrNULL), partialEdgeBoundaries(ptrNULL)
        { points = new DisPIDPairSet_t();
          closedPoints = new DisPIDPairSet_t();
          fullEdgeQueries = new PIDSet_t();
          partialEdgeQueries = new PIDSegmentSetMap_t();
          fullEdgeBoundaries = new BIDSet_t();
          partialEdgeBoundaries = new BIDSegmentSetMap_t(); };

    EdgeID_t edgeID;
    NodeID_t start;
    NodeID_t end;
    Distance_t length;

    DisPIDPairSet_t* points;
    DisPIDPairSet_t* closedPoints;

    /// influence zone
    PIDSet_t* fullEdgeQueries;
    PIDSegmentSetMap_t* partialEdgeQueries;

    /// boundary zone
    BIDSet_t* fullEdgeBoundaries;
    BIDSegmentSetMap_t* partialEdgeBoundaries;

    inline void addFullEdge(PointID_t queryPointID) { fullEdgeQueries->insert(queryPointID); };
    inline void addPartialEdge(PointID_t queryPointID, SegmentSet_t segments) { partialEdgeQueries->insert(make_pair(queryPointID, segments)); };

    inline void addFullEdgeBoundary(BoundaryID_t boundaryID) { fullEdgeBoundaries->insert(boundaryID); };
    inline void addPartialEdgeBoundary(BoundaryID_t boundaryID, SegmentSet_t segments) { partialEdgeBoundaries->insert(make_pair(boundaryID, segments)); };

    void closePoint(PointID_t pid);
    void openPoint(PointID_t pid);
    void removeQuery(PointID_t queryPointID);

    void clean() { if(points) delete points;
                   if(closedPoints) delete closedPoints;
                   if(fullEdgeQueries) delete fullEdgeQueries;
                   if(partialEdgeQueries) delete partialEdgeQueries;
                   if(fullEdgeBoundaries) delete fullEdgeBoundaries;
                   if(partialEdgeBoundaries) delete partialEdgeBoundaries; };
};

/**********************************************************************
 * Simple data collection structures
 **********************************************************************/

typedef vector<Node> NodeV_t;
typedef vector<Edge> EdgeV_t;
typedef vector<Point> PointV_t;
typedef vector<Object> ObjectV_t;

/**********************************************************************
 * Data collection
 **********************************************************************/

typedef Edge* Edge_p;
typedef vector<Edge_p> EdgePV_t;
typedef map<NodeID_t, EdgePV_t> NIDEdgePVMap_t;

typedef Point* Point_p;
typedef vector<Point_p> PointPV_t;

struct Data
{
    Data();

    static Amount_t numNodes;
    static NodeV_t nodes;

    static Amount_t numEdges;
    static EdgeV_t edges;

    static Amount_t numPoints;
    static PointV_t points;

    static Amount_t numFacilities;
    static PointPV_t facilities;

    static NIDEdgePVMap_t adjMap;

    static void clean();
};

/**********************************************************************
 * SAC Data structure
 **********************************************************************/

struct SafeRegion
{
    EdgeID_t eid;
    Distance_t start;
    Distance_t end;
};

typedef map<ObjectID_t, SafeRegion> OIDSafeEdgeMap_t;

#endif // DATA_H
