#include "NorthAmerica.h"

/**********************************************************************
 * NAPoint class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, NAPoint& p)
{
    in >> p.longitude >> p.latitude >> p.categoryID;
    // check that the inputs succeeded
    if (!in)
        p = NAPoint();
    return in;
}

std::ostream&
operator<<(std::ostream& out, const NAPoint& p)
{
    out << p.categoryID << " " << p.dis << " ";
    out << p.longitude << " " << p.latitude << endl;
    return out;
}

/**********************************************************************
 * NANode class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, NANode& n)
{
    in >> n.nodeID >> n.longitude >> n.latitude;
    // check that the inputs succeeded
    if (!in)
        n = NANode();
    return in;
}

std::ostream&
operator<<(std::ostream& out, const NANode& n)
{
    out << n.nodeID << " " << n.longitude << " "
        << n.latitude << endl;
    return out;
}

/**********************************************************************
 * NAEdge class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, NAEdge& e)
{
    in >> e.edgeID >> e.startNodeID >> e.endNodeID >> e.l2dis;
    // check that the inputs succeeded
    if (!in)
        e = NAEdge();
    return in;
}

std::ostream&
operator<<(std::ostream& out, const NAEdge& e)
{
    out << e.edgeID << " " << e.startNodeID << " " << e.endNodeID << " "
        << e.l2dis << endl;
    return out;
}

/**********************************************************************
 * NAMap class
 **********************************************************************/

std::istream&
operator>>(std::istream& in, NAMap& e)
{
    in >> e.startNodeID >> e.endNodeID >> e.length >> e.numPoints;
    // check that the inputs succeeded
    if (!in)
        e = NAMap();
    e.points.resize(e.numPoints);
    for(int i=0; i<e.numPoints; i++)
    {
        e.points.push_back(NAPoint());
        in >> e.points.at(i).categoryID >> e.points.at(i).dis;
        // check that the inputs succeeded
        if (!in)
            e = NAMap();
    }
    return in;
}

std::ostream&
operator<<(std::ostream& out, const NAMap& e)
{
    out << e.startNodeID << " " << e.endNodeID << " "
        << e.length << " " << e.numPoints << endl;
    for(int i=0; i<e.numPoints; i++)
    {
        NAPoint point = e.points.at(i);
        out << point.categoryID << " " << point.dis << endl;
    }
    return out;
}

/**********************************************************************
 * NorthAmericaDataLoader class
 **********************************************************************/

void
NorthAmericaDataLoader::load()
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
NorthAmericaDataLoader::readNodes()
{
    ifstream cnode ("data/NorthAmerica/NA.cnode");
    numNodes = 0;
    if(cnode.is_open())
    {
        while(cnode.good())
        {
            NANode node;
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
        cout << "NA.cnode is not open" << endl;
    }
    // cout << numNodes << " number of vertex has been loaded" << endl;
}

void
NorthAmericaDataLoader::readEdges()
{
    ifstream cedge ("data/NorthAmerica/NA.cedge");
    numEdges = 0;
    if(cedge.is_open())
    {
        while(cedge.good())
        {
            NAEdge edge;
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
        cout << "NA.cedge is not open" << endl;
    }
    // cout << numEdges << " number of edges has been loaded" << endl;
}

void
NorthAmericaDataLoader::readMap()
{
    ifstream merge ("data/NorthAmerica/NAmap.txt");
    numMap = 0;
    if(merge.is_open())
    {
        while(merge.good())
        {
            NAMap edge;
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
        cout << "NAmap is not open" << endl;
    }
    // cout << numMap << " number of edge maps has been loaded" << endl;
}

void
NorthAmericaDataLoader::generateMap()
{
    // read in edges
    readEdges();
    cout << "finish readEdges() " << numEdges << endl;

    // initialize map
    for(int iedge=0; iedge<numEdges; iedge++)
    {
        NAMap map;
        NAEdge &edge = edges.at(iedge);
        map.edgeID = edge.edgeID;
        map.startNodeID = edge.startNodeID;
        map.endNodeID = edge.endNodeID;
        map.length = edge.l2dis;
        map.numPoints = 0;
        maps.push_back(map);
    }
    cout << "finish initialize map " << maps.size() << endl;

    // generate points
    for(int ipoint=0; ipoint<NUM_POINTS_NA; ipoint++)
    {
        int mapID = getRandom(numEdges);
        NAMap &map = maps.at(mapID);
        NAPoint point;
        point.categoryID = 0;
        point.latitude = 0;
        point.longitude = 0;
        point.dis = map.length * getRandom();
        map.points.push_back(point);
        map.numPoints++;
    }
    cout << "finish generation " << endl;

    // output
    string mapFileName = "data/NorthAmerica/NAmap.txt";
    ofstream mapFile;
    mapFile.open(mapFileName.c_str());
    if (!mapFile.is_open())
    {
        cout << "Unable to open file : " << mapFileName << endl;
        return;
    }
    numMap = numEdges;
    for(int imap=0; imap<numMap; imap++)
    {
        NAMap &naMap = maps.at(imap);
        mapFile << naMap;
    }
    mapFile.close();
}

void
NorthAmericaDataLoader::addNodes()
{
    Data::numNodes = numNodes;
    for(int i=0; i<numNodes; i++)
    {
        NANode cnode = nodes.at(i);
        Node node(i, cnode.longitude, cnode.latitude);
        Data::nodes.push_back(node);
    }
    nodes.clear();
}

void
NorthAmericaDataLoader::addEdges()
{
    Data::numEdges = numMap;
    for(int iedge=0; iedge<numMap; iedge++)
    {
        NAMap cmap = maps.at(iedge);
        int num = cmap.numPoints;
        Edge edge(iedge, cmap.startNodeID, cmap.endNodeID, cmap.length);
        for(int ipoint=0; ipoint<num; ipoint++)
        {
            NAPoint cpoint = cmap.points.at(ipoint);
            Point point(Data::numPoints, cpoint.categoryID, iedge, cpoint.dis);
            bool isFacility = Argument::isFacility(point.categoryID);
            if(isFacility)
            {
                if(Argument::category == Argument::uniCategory)
                {
                    double passmark = Argument::numFacilities * 1.0 / NUM_POINTS_NA;
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
    }
    edges.clear();
    maps.clear();
}

void
NorthAmericaDataLoader::addPoints()
{
    points.clear();
}

void
NorthAmericaDataLoader::addIndex()
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
NorthAmericaDataLoader::checkData()
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
