#ifndef UPDATABLEZONE_H
#define UPDATABLEZONE_H

#include <algorithm>
#include <fstream>

#include "Data.h"

class UpdatableZone
{
    public:
        UpdatableZone() { };
        InfluenceZone* computeZone(PointID_t queryPointID);
        // void outputZoneNetwork(InfluenceZone* influenceZone);
        void indexZone(InfluenceZone* influenceZone, PointID_t queryPointID);
        void removeZone(PointID_t queryPointID);

        void shrink(PointID_t queryPointID, BIDSet_t &expiredBoundaries);
        void enlarge(PointID_t queryPointID, BIDSet_t &expiredBoundaries);
        void removeBoundaryZone(BoundaryID_t boundaryID);
        // void outputZoneNetworkUpdateBefore(PointID_t queryPointID);
        // void outputZoneNetworkUpdate(PointID_t queryPointID);

    protected:

    private:
        void init(Point &query, NIDDisMap_t &zMinDis, NIDMinHeap_t &zQueue, InfluenceZone* influenceZone);
        bool nDijkstra(Point &query, NodeID_t nid, Distance_t limit, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryB(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryC(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);
        void computeBoundaryDInIn(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone);

        /// for updating
        BoundaryZone* computeBoundaryZone(Point &query, BoundaryID_t boundaryID, Distance_t dis2Query);
        void indexBoundaryZone(BoundaryID_t boundaryID, BoundaryZone* boundaryZone);
        bool nDijkstra(Point &query, NodeID_t nid, Distance_t &limit, NIDPIDDisMap_t &nfMinDis);
        void computeBoundaryBD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, Distance_t disQIn, Distance_t disQOut, NIDPIDDisMap_t &nfMinDis);
        void computeBoundaryB(Point &query, Edge &edge, NodeID_t in, NodeID_t out, Distance_t disQIn, Distance_t disQOut, NIDPIDDisMap_t &nfMinDis);
        void computeBoundaryC(Point &query, Edge &edge, bool startIn, bool endIn, NIDPIDDisMap_t &nfMinDis);
        void computeBoundaryD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, Distance_t disQIn, Distance_t disQOut, NIDPIDDisMap_t &nfMinDis);
        void computeBoundaryDInIn(Point &query, Edge &edge, Distance_t disQStart, Distance_t disQEnd, NIDPIDDisMap_t &nfMinDis);

};

#endif // UPDATABLEZONE_H
