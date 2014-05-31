#include "California.h"

/**********************************************************************
 * CPoint class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, CPoint& p)
{
    in >> p.longitude >> p.latitude >> p.categoryID;
    // check that the inputs succeeded
    if (!in)
        p = CPoint();
    return in;
}

std::ostream&
operator<<(std::ostream& out, const CPoint& p)
{
    out << p.categoryID << " " << p.dis << " ";
    out << p.longitude << " " << p.latitude << endl;
    return out;
}

/**********************************************************************
 * CNode class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, CNode& n)
{
    in >> n.nodeID >> n.longitude >> n.latitude;
    // check that the inputs succeeded
    if (!in)
        n = CNode();
    return in;
}

std::ostream&
operator<<(std::ostream& out, const CNode& n)
{
    out << n.nodeID << " " << n.longitude << " "
        << n.latitude << endl;
    return out;
}

/**********************************************************************
 * CEdge class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, CEdge& e)
{
    in >> e.edgeID >> e.startNodeID >> e.endNodeID >> e.l2dis;
    // check that the inputs succeeded
    if (!in)
        e = CEdge();
    return in;
}

std::ostream&
operator<<(std::ostream& out, const CEdge& e)
{
    out << e.edgeID << " " << e.startNodeID << " " << e.endNodeID << " "
        << e.l2dis << endl;
    return out;
}

/**********************************************************************
 * CMap class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, CMap& e)
{
    in >> e.startNodeID >> e.endNodeID >> e.length >> e.numPoints;
    // check that the inputs succeeded
    if (!in)
        e = CMap();
    e.points.resize(e.numPoints);
    for(int i=0; i<e.numPoints; i++)
    {
        e.points.push_back(CPoint());
        in >> e.points.at(i).categoryID >> e.points.at(i).dis;
        // check that the inputs succeeded
        if (!in)
            e = CMap();
    }
    return in;
}

std::ostream&
operator<<(std::ostream& out, const CMap& e)
{
    out << e.edgeID << " " << e.startNodeID << " " << e.endNodeID << " "
        << e.length << " " << e.numPoints << endl;
    for(int i=0; i<e.numPoints; i++)
    {
        CPoint point = e.points.at(i);
        out << point.categoryID << " " << point.dis << endl;
    }
    return out;
}

/**********************************************************************
 * CaliforniaDataLoader class
 **********************************************************************/

void
CaliforniaDataLoader::load()
{
    readNodes();
    readEdges();
    readMap();

    addNodes();
    addEdges();
    addPoints();

    addIndex();
}

void
CaliforniaDataLoader::readNodes()
{
    ifstream cnode ("data/California/cal.cnode");
    numNodes = 0;
    if(cnode.is_open())
    {
        while(cnode.good())
        {
            CNode node;
            cnode >> node;
            if(node.isGood())
            {
                numNodes++;
                nodes.push_back(node);
            }
        }
        cnode.close();
    }
    else
    {
        cout << "cal.cnode is not open" << endl;
    }
    // cout << numNodes << " number of vertex has been loaded" << endl;
}

void
CaliforniaDataLoader::readEdges()
{
    ifstream cedge ("data/California/cal.cedge");
    numEdges = 0;
    if(cedge.is_open())
    {
        while(cedge.good())
        {
            CEdge edge;
            cedge >> edge;
            if(edge.isGood())
            {
                numEdges++;
                edges.push_back(edge);
            }
        }
        cedge.close();
    }
    else
    {
        cout << "cal.cedge is not open" << endl;
    }
    // cout << numEdges << " number of edges has been loaded" << endl;
}

void
CaliforniaDataLoader::readMap()
{
    ifstream merge ("data/California/calmap.txt");
    numMap = 0;
    if(merge.is_open())
    {
        while(merge.good())
        {
            CMap edge;
            merge >> edge;
            if(edge.isGood())
            {
                numMap++;
                maps.push_back(edge);
            }
        }
        merge.close();
    }
    else
    {
        cout << "calmap is not open" << endl;
    }
    // cout << numMap << " number of edge maps has been loaded" << endl;
}

void
CaliforniaDataLoader::addNodes()
{
    Data::numNodes = numNodes;
    for(int i=0; i<numNodes; i++)
    {
        CNode cnode = nodes.at(i);
        Node node(i, cnode.longitude, cnode.latitude);
        Data::nodes.push_back(node);
    }
    nodes.clear();
}

void
CaliforniaDataLoader::addEdges()
{
    Data::numEdges = numMap;
    for(int iedge=0; iedge<numMap; iedge++)
    {
        CMap cmap = maps.at(iedge);
        int num = cmap.numPoints;
        Edge edge(iedge, cmap.startNodeID, cmap.endNodeID, cmap.length);
        for(int ipoint=0; ipoint<num; ipoint++)
        {
            CPoint cpoint = cmap.points.at(ipoint);
            Point point(Data::numPoints, cpoint.categoryID, iedge, cpoint.dis);
            bool isFacility = Argument::isFacility(point.categoryID);
            if(isFacility)
            {
                if(Argument::category == Argument::uniCategory)
                {
                    double passmark = Argument::numFacilities * 1.0 / NUM_POINTS;
                    double mark = (facilityID.size() + 1.0) / (Data::numPoints + 1.0);
                    if(mark < passmark + Argument::EPS)
                    {
                        point.categoryID = Argument::category;
                        edge.points->insert(make_pair(point.disToStart, point.pointID));
                    }
                    else
                        isFacility = false;
                }
                else
                {
                    edge.points->insert(make_pair(point.disToStart, point.pointID));
                }
            }
            Data::points.push_back(point);
            Data::numPoints++;
            if(isFacility)
                facilityID.push_back(point.pointID);
        }
        Data::edges.push_back(edge);
        if(edges.at(iedge).l2dis != Data::edges.at(iedge).length)
        {
            cout << "data error [edge] " << edges.at(iedge).l2dis << " != " << Data::edges.at(iedge).length << endl;
        }
    }
    edges.clear();
    maps.clear();
}

void
CaliforniaDataLoader::addPoints()
{
    points.clear();
}

void
CaliforniaDataLoader::addIndex()
{
    for(Index_t i=0; i<Data::edges.size(); i++)
    {
        Edge &edge = Data::edges.at(i);
        Data::adjMap[edge.start].push_back(&edge);
        Data::adjMap[edge.end].push_back(&edge);
    }

    for(Index_t i=0; i<facilityID.size(); i++)
    {
        Point &point = Data::points.at(facilityID.at(i));
        Data::facilities.push_back(&point);
        Data::numFacilities++;
    }
}

void
CaliforniaDataLoader::checkData()
{
    for(NIDEdgePVMap_t::iterator iterM = Data::adjMap.begin(); iterM != Data::adjMap.end(); iterM++)
    {
        NodeID_t end = iterM->first;
        EdgePV_t &epv = iterM->second;
        for(Index_t i=0; i<epv.size(); i++)
        {
            Edge &edge = *epv.at(i);
            if(edge.start != end && edge.end != end)
                cout << "data error [adjMap] " << edge.edgeID << " " << edge.start << " " << edge.end << " " << edge.length << endl;
        }
    }
    for(Index_t i=0; i<Data::facilities.size(); i++)
    {
        Point &point = *Data::facilities.at(i);
        if(!Argument::isFacility(point.categoryID))
            cout << "data error [facilities] " << point.pointID << " " << point.categoryID << " " << point.disToStart << " " << point.k << endl;
    }
    cout << "data checked" << endl;
    // cout << Data::numNodes << " number of nodes" << endl;
    // cout << Data::numEdges << " number of edges" << endl;
    // cout << Data::numFacilities << " number of facilities" << endl;
}
