#include "Data.h"

/**********************************************************************
 * struct InfluenceZone
 **********************************************************************/

std::ostream&
operator<<(std::ostream& out, const InfluenceZone& iz)
{
    // zoneNodes
    {
        out << "zoneNodes" << endl;
        int i = 0;
        for(NIDSet_t::const_iterator iter = iz.zoneNodes->begin(); iter != iz.zoneNodes->end(); iter++, i++)
        {
            if(i%10==0)
                out << endl;
            out << *iter << endl;
        }
    }

    // fullEdges
    {
        out << endl << "fullEdges" << endl;
        int i = 0;
        for(EIDSet_t::const_iterator iter = iz.fullEdges->begin(); iter != iz.fullEdges->end(); iter++, i++)
        {
            if(i%10==0)
                out << endl;
            out << *iter << endl;
        }
    }

    // partial Edges
    {
        out << endl << "partialEdges" << endl;
        for(EIDSegmentSetMap_t::const_iterator itere = iz.partialEdges->begin(); itere != iz.partialEdges->end(); itere++)
        {
            out << itere->first << endl;

            Edge &edge = Data::edges.at(itere->first);
            out << "  " << edge.start << " " << edge.end << " " << edge.length << endl;
            bool prunedStart = iz.zoneNodes->find(edge.start) == iz.zoneNodes->end();
            bool prunedEnd = iz.zoneNodes->find(edge.end) == iz.zoneNodes->end();
            out << "  " << (prunedStart?"pruned ":"ok     ") << (prunedEnd?"pruned ":"ok     ") << endl;

            SegmentSet_t segments = itere->second;
            for(SegmentSet_t::const_iterator iters = segments.begin(); iters != segments.end(); iters++)
                out << "  [" << iters->first << ", " << iters->second << "]" << endl;
            out << endl;
        }
    }

    return out;
}

/**********************************************************************
 * struct BoundaryID_t
 **********************************************************************/

std::ostream&
operator<<(std::ostream &out, const BoundaryID_t &bid)
{
    out << bid.query << " " << bid.edgeID << " " << bid.dis << " " << bid.innerNode << endl;
    return out;
}

bool
operator==(const BoundaryID_t &bid1, const BoundaryID_t &bid2)
{
    if(bid1.query != bid2.query)
        return false;
    if(bid1.edgeID != bid2.edgeID)
        return false;
    if(bid1.dis != bid2.dis)
        return false;
    return true;
}

bool
operator!=(const BoundaryID_t &bid1, const BoundaryID_t &bid2)
{
    if(bid1.query != bid2.query)
        return true;
    if(bid1.edgeID != bid2.edgeID)
        return true;
    if(bid1.dis != bid2.dis)
        return true;
    return false;
}

bool
operator<(const BoundaryID_t &bid1, const BoundaryID_t &bid2)
{
    if(bid1.query != bid2.query)
        return bid1.query < bid2.query;
    if(bid1.edgeID != bid2.edgeID)
        return bid1.edgeID < bid2.edgeID;
    if(bid1.dis != bid2.dis)
        return bid1.dis < bid2.dis;
    return false;
}

bool
operator<=(const BoundaryID_t &bid1, const BoundaryID_t &bid2)
{
    if(bid1.query < bid2.query)
        return true;
    if(bid1.query > bid2.query)
        return false;
    if(bid1.edgeID < bid2.edgeID)
        return true;
    if(bid1.edgeID > bid2.edgeID)
        return false;
    if(bid1.dis <= bid2.dis)
        return true;
    return false;
}

bool
operator>(const BoundaryID_t &bid1, const BoundaryID_t &bid2)
{
    if(bid1.query != bid2.query)
        return bid1.query > bid2.query;
    if(bid1.edgeID != bid2.edgeID)
        return bid1.edgeID > bid2.edgeID;
    if(bid1.dis != bid2.dis)
        return bid1.dis > bid2.dis;
    return false;
}

bool
operator>=(const BoundaryID_t &bid1, const BoundaryID_t &bid2)
{
    if(bid1.query > bid2.query)
        return true;
    if(bid1.query < bid2.query)
        return false;
    if(bid1.edgeID > bid2.edgeID)
        return true;
    if(bid1.edgeID < bid2.edgeID)
        return false;
    if(bid1.dis >= bid2.dis)
        return true;
    return false;
}

/**********************************************************************
 * struct Object
 **********************************************************************/

std::istream&
operator>>(std::istream& in, Object& o)
{
    in >> o.objectID >> o.preEdgeID >> o.preDis >> o.curEdgeID >> o.curDis;
    // check that the inputs succeeded
    if (!in)
        o = Object();
    return in;
}

std::ostream&
operator<<(std::ostream& out, const Object& o)
{
    out << o.objectID << " " << o.preEdgeID << " " << o.preDis << " " << o.curEdgeID << " " << o.curDis << endl;
    return out;
}

/**********************************************************************
 * struct Edge
 **********************************************************************/

void
Edge::closePoint(PointID_t pid)
{
    for(DisPIDPairSet_t::const_iterator iterp = points->begin(); iterp != points->end(); iterp++)
        if(iterp->second == pid)
        {
            closedPoints->insert(make_pair(iterp->first, iterp->second));
            points->erase(iterp);
            break;
        }
}

void
Edge::openPoint(PointID_t pid)
{
    for(DisPIDPairSet_t::const_iterator iterp = closedPoints->begin(); iterp != closedPoints->end(); iterp++)
        if(iterp->second == pid)
        {
            points->insert(make_pair(iterp->first, iterp->second));
            closedPoints->erase(iterp);
            break;
        }
}

void
Edge::removeQuery(PointID_t queryPointID)
{
    fullEdgeQueries->erase(queryPointID);
    for(PIDSegmentSetMap_t::iterator iterm = partialEdgeQueries->begin(); iterm != partialEdgeQueries->end(); iterm++)
        if(iterm->first == queryPointID)
        {
            partialEdgeQueries->erase(iterm);
            break;
        }
}

/**********************************************************************
 * class Data
 **********************************************************************/

Amount_t Data::numNodes = 0;
NodeV_t Data::nodes;

Amount_t Data::numEdges = 0;
EdgeV_t Data::edges;

Amount_t Data::numPoints = 0;
PointV_t Data::points;

Amount_t Data::numFacilities = 0;
PointPV_t Data::facilities;

NIDEdgePVMap_t Data::adjMap;

void
Data::clean()
{
    for(Index_t ie=0; ie<Data::numEdges; ie++)
        Data::edges.at(ie).clean();
    Data::facilities.clear();
    for(Index_t ip=0; ip<Data::numPoints; ip++)
        Data::points.at(ip).clean();
    for(Index_t in=0; in<Data::numNodes; in++)
        Data::nodes.at(in).clean();
}
