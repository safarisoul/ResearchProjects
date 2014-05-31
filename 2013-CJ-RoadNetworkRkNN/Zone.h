#ifndef ZONE_H
#define ZONE_H

#include <algorithm>
#include <fstream>

#include "Data.h"

class Zone
{
    public:
        Zone() { };
        InfluenceZone* computeZone(PointID_t queryPointID);
        // void outputZoneNetwork(InfluenceZone* influenceZone);
        void indexZone(InfluenceZone* influenceZone, PointID_t queryPointID);

    protected:

    private:
        void init(Point &query, NIDDisMap_t &zMinDis, NIDMinHeap_t &zQueue, InfluenceZone* influenceZone);
        bool nDijkstra(Point &query, NodeID_t nid, Distance_t limit, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryB(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryC(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryDInIn(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);

};

#endif // ZONE_H
