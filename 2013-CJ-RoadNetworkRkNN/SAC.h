#ifndef SAC_H
#define SAC_H

#include <fstream>

#include "Data.h"

class SAC
{
    public:
        SAC() { };
        void computeUnprunedNetworkForAll();
        void monitor();

    protected:

    private:
        ObjectV_t objects;
        string objectFileName;
        ifstream objectFile;
        string queryFileName;
        ifstream queryFile;

        OIDSafeEdgeMap_t safeRegions;

        UnprunedNetwork* computeUnprunedNetwork(Point &query);
        void init(Point &query, UnprunedNetwork* unprunedNetwork, NIDDisMap_t &unMinDis, NIDMinHeap_t &unQueue);
        bool nDijkstra(Point &query, NodeID_t nid, Distance_t limit);

        void indexUnprunedNetwork(UnprunedNetwork* unprunedNetwork, Point &query);

        void computeMonitoredNetwork(PointID_t pid, Object& object, Edge& safeEdge, Segment_t safeRegion);
        bool verifyByLimitedDijkstra(Distance_t locationO, PointID_t pid, Edge& edge);
        bool verifyByMonitoredNetwork(PointID_t pid, MonitoredNetwork &monNetwork, Distance_t locationO);

};

#endif // SAC_H
