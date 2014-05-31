#include "Zone.h"

InfluenceZone*
Zone::computeZone(PointID_t queryPointID)
{
    Point &query = Data::points.at(queryPointID);

    NIDDisMap_t zMinDis; // ........... min distance from the query point to all nodes
    NIDMinHeap_t zQueue; // ........... priority queue / min heap

    NIDSet_t bounding;   // ........... pruned nodes connect to at least one zone nodes or the query point directly
    NIDPIDDisMap_t nfMinDis; // ....... shortest network distance from nodes to their pruner facilities

    InfluenceZone* influenceZone = new InfluenceZone();

    init(query, zMinDis, zQueue, influenceZone);
    while(!zQueue.empty())
    {
        NodeID_t node = zQueue.begin()->second;
        Distance_t disToQ = zQueue.begin()->first;
        zQueue.erase(zQueue.begin());
        zMinDis[node] = disToQ;
        bool dead = !nDijkstra(query, node, disToQ, nfMinDis, influenceZone);
        if(dead)
            bounding.insert(node);
        else
        {
            influenceZone->zoneNodes->insert(node);
            EdgePV_t &epv = Data::adjMap[node];
            for(Index_t ie=0; ie<epv.size(); ie++)
            {
                Edge &edge = *epv.at(ie);
                // find the node at the other end of the edge
                NodeID_t next = ( node==edge.start ? edge.end : edge.start );

                // if the next node hasn't been explored or a better path found for it
                if(zMinDis.find(next) == zMinDis.end() || zMinDis[node] + edge.length < zMinDis[next])
                {
                    zQueue.erase(make_pair(zMinDis[next], next)); // for better path
                    // update priority queue
                    zMinDis[next] = zMinDis[node] + edge.length;
                    zQueue.insert(make_pair(zMinDis[next], next));
                    influenceZone->touchedNodes->insert(next);
                }
            }
        }
    }

    // collect edges intersect with influence zone
    EIDSet_t zoneEdges;
    zoneEdges.insert(query.edgeID);
    for(NIDSet_t::const_iterator itern = influenceZone->zoneNodes->begin(); itern != influenceZone->zoneNodes->end(); itern++)
    {
        EdgePV_t &epv = Data::adjMap[*itern];
            for(Index_t ie=0; ie<epv.size(); ie++)
            {
                Edge &edge = *epv.at(ie);
                zoneEdges.insert(edge.edgeID);
            }
    }

    // check the set of candidate influence zone edge
    for(EIDSet_t::const_iterator itere = zoneEdges.begin(); itere != zoneEdges.end(); itere++)
    {
        Edge &edge = Data::edges.at(*itere);
        bool startIn = (influenceZone->zoneNodes->find(edge.start) != influenceZone->zoneNodes->end());
        bool endIn = (influenceZone->zoneNodes->find(edge.end) != influenceZone->zoneNodes->end());
        if(startIn && endIn)
        {
            if(edge.edgeID == query.edgeID)
            {
                influenceZone->fullEdges->insert(edge.edgeID);
                continue;
            }
            Distance_t disQStart = zMinDis[edge.start];
            Distance_t disQEnd = zMinDis[edge.end];
            if(disQStart + edge.length == disQEnd || disQEnd + edge.length == disQStart)
            {
                influenceZone->fullEdges->insert(edge.edgeID);
                continue;
            }
            computeBoundaryDInIn(query, edge, zMinDis, nfMinDis, influenceZone);
        }
        else if(startIn && !endIn)
        {
            if(edge.edgeID == query.edgeID)
            {
                computeBoundaryC(query, edge, zMinDis, nfMinDis, influenceZone);
                continue;
            }
            NodeID_t in = edge.start, out = edge.end;
            Distance_t disQOut = zMinDis[out];
            Distance_t disQIn = zMinDis[in];
            if(disQOut + edge.length == disQIn)
            {
                cout << "Zone::computeZone error : impossible case" << endl;
                exit(EXIT_SUCCESS);
            }
            else if(disQIn + edge.length == disQOut)
                computeBoundaryB(query, edge, in, out, zMinDis, nfMinDis, influenceZone);
            else
                computeBoundaryD(query, edge, in, out, zMinDis, nfMinDis, influenceZone);
        }
        else if(!startIn && endIn)
        {
            if(edge.edgeID == query.edgeID)
            {
                computeBoundaryC(query, edge, zMinDis, nfMinDis, influenceZone);
                continue;
            }
            NodeID_t in = edge.end, out = edge.start;
            Distance_t disQOut = zMinDis[out];
            Distance_t disQIn = zMinDis[in];
            if(disQOut + edge.length == disQIn)
            {
                cout << "Zone::computeZone error : impossible case" << endl;
                exit(EXIT_SUCCESS);
            }
            else if(disQIn + edge.length == disQOut)
                computeBoundaryB(query, edge, in, out, zMinDis, nfMinDis, influenceZone);
            else
                computeBoundaryD(query, edge, in, out, zMinDis, nfMinDis, influenceZone);
        }
        else
        {
            if(edge.edgeID == query.edgeID)
            {
                computeBoundaryC(query, edge, zMinDis, nfMinDis, influenceZone);
                continue;
            }
            cout << "Zone::computeZone error" << endl;
            exit(EXIT_SUCCESS);
        }
    }

    return influenceZone;
}

void
Zone::init(Point &query, NIDDisMap_t &zMinDis, NIDMinHeap_t &zQueue, InfluenceZone* influenceZone)
{
    Edge &edge = Data::edges.at(query.edgeID);
    influenceZone->touchedEdges->insert(edge.edgeID);

    zMinDis[edge.start] = query.disToStart;
    zMinDis[edge.end] = edge.length - query.disToStart;

    zQueue.insert(make_pair(zMinDis[edge.start], edge.start));
    zQueue.insert(make_pair(zMinDis[edge.end], edge.end));
    influenceZone->touchedNodes->insert(edge.start);
    influenceZone->touchedNodes->insert(edge.end);
}

bool
Zone::nDijkstra(Point &query, NodeID_t nid, Distance_t limit, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
{
    /// encounter nodes
    NIDDisMap_t nMinDis; // ............ min distance from a node to all nodes
    NIDMinHeap_t nQueue; // ............ priority queue for a node
    PIDSet_t facilities; // ............ collect of encountered facilities
    NIDSet_t nNodes; // ................ visited (decided) nodes

    nQueue.insert(make_pair(0, nid));

    while(!nQueue.empty() && nQueue.begin()->first < limit)
    {
        /* /// this can not be included, as without a full list of pruners, the calculation of boundary would fail
        if(facilities.size() >= query.k)
            return false;*/

        NodeID_t node = nQueue.begin()->second;
        Distance_t disToQ = nQueue.begin()->first;
        nQueue.erase(nQueue.begin());
        nNodes.insert(node);

        EdgePV_t &epv = Data::adjMap[node];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            influenceZone->touchedEdges->insert(edge.edgeID);
            // find the node at the other end of the edge
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );

            // if the next node hasn't been explored or a better path found for it
            if(nMinDis.find(next) == nMinDis.end() || nMinDis[node] + edge.length < nMinDis[next])
            {
                nQueue.erase(make_pair(nMinDis[next], next)); // for better path
                // update priority queue
                nMinDis[next] = nMinDis[node] + edge.length;
                nQueue.insert(make_pair(nMinDis[next], next));
                influenceZone->touchedNodes->insert(next);
            }

            // find facilities along this edge
            if(node == edge.start)
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                {
                    Distance_t disFToQ = disToQ + iterp->first;
                    if(disFToQ + Argument::EPS < limit)
                    {
                        facilities.insert(iterp->second);
                        if(nfMinDis[nid].find(iterp->second) == nfMinDis[nid].end() || nfMinDis[nid][iterp->second] > disFToQ)
                            nfMinDis[nid][iterp->second] = disFToQ;
                    }
                }
            else
                for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++)
                {
                    Distance_t disFToQ = disToQ + edge.length - iterp->first;
                    if(disFToQ + Argument::EPS < limit)
                    {
                        facilities.insert(iterp->second);
                        if(nfMinDis[nid].find(iterp->second) == nfMinDis[nid].end() || nfMinDis[nid][iterp->second] > disFToQ)
                            nfMinDis[nid][iterp->second] = disFToQ;
                    }
                }
        }
    }
    if(facilities.size() >= query.k)
        return false;
    return true;
}

void
Zone::computeBoundaryB(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
{
    PIDDisMap_t &minDisIn = nfMinDis[in];
    PIDDisMap_t &minDisOut = nfMinDis[out];

    if(query.k - minDisIn.size() < 1)
    {
        cout << "Zone::computeBoundaryB error " << query.k << " - " << minDisIn.size() << " < 1" << endl;
        exit(EXIT_SUCCESS);
    }

    PIDSet_t onEdgePoints;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
        onEdgePoints.insert(iterp->second);

    Index_t index = query.k - minDisIn.size();

    vector<Distance_t> length;
    Distance_t disQOut = zMinDis[out];
    // for points prunes out
    for(PIDDisMap_t::const_iterator iterp = minDisOut.begin(); iterp != minDisOut.end(); iterp++)
        // but not prunes in
        if(minDisIn.find(iterp->first) == minDisIn.end())
        {
            // if on edge
            if(onEdgePoints.find(iterp->first) != onEdgePoints.end())
                length.push_back((disQOut + minDisOut[iterp->first]) / 2);
            else
                length.push_back((disQOut - minDisOut[iterp->first]) / 2);
        }

    sort(length.begin(), length.end());
    SegmentSet_t segments;
    if(out == edge.start)
        segments.insert(make_pair(length.at(length.size() - index), edge.length));
    else
        segments.insert(make_pair(0, edge.length - length.at(length.size() - index)));
    influenceZone->partialEdges->insert(make_pair(edge.edgeID, segments));
}

void
Zone::computeBoundaryC(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
{
    vector<Distance_t> disToStart;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
        disToStart.push_back((iterp->first + query.disToStart) / 2);

    // find out where is the query point
    Amount_t startSide = 0, endSide = 0;
    PIDSet_t onEdgePoints;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
    {
        onEdgePoints.insert(iterp->second);
        if(iterp->first < query.disToStart)
            startSide++;
        else if(iterp->first > query.disToStart)
            endSide++;
    }

    Distance_t from = 0, to = edge.length;

    if(startSide >= query.k)
    {
        Index_t ip = 0;
        for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++, ip++)
            if(ip == startSide - query.k)
            {
                from = ( iterp->first + query.disToStart ) / 2;
                break;
            }
    }
    else if(influenceZone->zoneNodes->find(edge.start) == influenceZone->zoneNodes->end())
    {
        vector<Distance_t> disToStart;
        PIDDisMap_t &minDisStart = nfMinDis[edge.start];
        for(PIDDisMap_t::const_iterator iterd = minDisStart.begin(); iterd != minDisStart.end(); iterd++)
            if(onEdgePoints.find(iterd->first) == onEdgePoints.end())
                disToStart.push_back(iterd->second);
        sort(disToStart.begin(), disToStart.end());
        Distance_t kth = disToStart.at(query.k - startSide - 1);
        Distance_t mid = ( kth + query.disToStart ) / 2;
        if(mid > query.disToStart)
        {
            cout << "Zone::computeBoundaryC error" << endl;
            exit(EXIT_SUCCESS);
        }
        from = query.disToStart - mid;
    }

    if(endSide >= query.k)
    {
        Index_t ip = 0;
        for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++, ip++)
            if(ip == endSide - query.k)
            {
                to = ( iterp->first + query.disToStart ) / 2;
                break;
            }
    }
    else if(influenceZone->zoneNodes->find(edge.end) == influenceZone->zoneNodes->end())
    {
        vector<Distance_t> disToEnd;
        PIDDisMap_t &minDisEnd = nfMinDis[edge.end];
        for(PIDDisMap_t::const_iterator iterd = minDisEnd.begin(); iterd != minDisEnd.end(); iterd++)
            if(onEdgePoints.find(iterd->first) == onEdgePoints.end())
                disToEnd.push_back(iterd->second);
        sort(disToEnd.begin(), disToEnd.end());
        Distance_t kth = disToEnd.at(query.k - endSide - 1);
        Distance_t mid = ( kth + edge.length - query.disToStart ) / 2;
        if(mid > edge.length - query.disToStart)
        {
            cout << "Zone::computeBoundaryC error" << endl;
            exit(EXIT_SUCCESS);
        }
        to = query.disToStart + mid;
    }

    if(from == 0 && to == edge.length)
        influenceZone->fullEdges->insert(edge.edgeID);
    else
    {
        SegmentSet_t segments;
        segments.insert(make_pair(from, to));
        influenceZone->partialEdges->insert(make_pair(edge.edgeID, segments));
    }
}

void
Zone::computeBoundaryD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
{
    PIDDisMap_t &minDisIn = nfMinDis[in];
    PIDDisMap_t &minDisOut = nfMinDis[out];

    if(query.k - minDisIn.size() < 1)
    {
        cout << "Zone::computeBoundaryD error" << endl;
        exit(EXIT_SUCCESS);
    }

    PIDSet_t onEdgePoints;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
        onEdgePoints.insert(iterp->second);

    Index_t index = query.k - minDisIn.size();

    vector<Distance_t> length;
    Distance_t disQIn = zMinDis[in];
    Distance_t disQOut = zMinDis[out];
    Distance_t qMax = (edge.length + disQIn - disQOut) / 2;
    Distance_t high = qMax * 2 + disQOut;
    // for points prunes out
    for(PIDDisMap_t::const_iterator iterp = minDisOut.begin(); iterp != minDisOut.end(); iterp++)
        // but not prunes in
        if(minDisIn.find(iterp->first) == minDisIn.end())
        {
            // if on edge
            if(onEdgePoints.find(iterp->first) != onEdgePoints.end())
                length.push_back((high + minDisOut[iterp->first]) / 2);
            else
                length.push_back((high - minDisOut[iterp->first]) / 2);
        }

    sort(length.begin(), length.end());
    SegmentSet_t segments;
    if(out == edge.start)
        segments.insert(make_pair(length.at(length.size() - index), edge.length));
    else
        segments.insert(make_pair(0, edge.length - length.at(length.size() - index)));
    influenceZone->partialEdges->insert(make_pair(edge.edgeID, segments));
}

void
Zone::computeBoundaryDInIn(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
{
    PIDDisMap_t &minDisStart = nfMinDis[edge.start];
    PIDDisMap_t &minDisEnd = nfMinDis[edge.end];

    Amount_t pruner = 0;
    for(PIDDisMap_t::const_iterator iterp = minDisStart.begin(); iterp != minDisStart.end(); iterp++)
        if(minDisEnd.find(iterp->first) != minDisEnd.end())
            pruner++;

    if(minDisStart.size() + minDisEnd.size() - pruner < query.k)
    {
        influenceZone->fullEdges->insert(edge.edgeID);
        return;
    }

    PIDSet_t onEdgePoints;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
        onEdgePoints.insert(iterp->second);

    Distance_t disQStart = zMinDis[edge.start];
    Distance_t disQEnd = zMinDis[edge.end];
    Distance_t qMax = (edge.length + disQEnd - disQStart) / 2;
    Distance_t highStart = disQStart + qMax * 2;
    qMax = edge.length - qMax;
    Distance_t highEnd = disQEnd + qMax * 2;
    vector<Distance_t> beginning, ending;

    for(PIDDisMap_t::const_iterator iterp = minDisStart.begin(); iterp != minDisStart.end(); iterp++)
        if(minDisEnd.find(iterp->first) == minDisEnd.end())
        {
            // off edge points
            if(onEdgePoints.find(iterp->first) == onEdgePoints.end())
                ending.push_back((highStart - iterp->second) / 2);
            else
                ending.push_back((highStart + iterp->second) / 2);
        }

    for(PIDDisMap_t::const_iterator iterp = minDisEnd.begin(); iterp != minDisEnd.end(); iterp++)
        if(minDisStart.find(iterp->first) == minDisStart.end())
        {
            if(onEdgePoints.find(iterp->first) == onEdgePoints.end())
                beginning.push_back((highEnd - iterp->second) / 2);
            else
                beginning.push_back((highEnd + iterp->second) / 2);
        }

    sort(beginning.begin(), beginning.end());
    sort(ending.begin(), ending.end());
    SegmentSet_t segments;
    segments.insert(make_pair(0, edge.length - beginning.at(beginning.size() - (query.k - pruner - ending.size()))));
    segments.insert(make_pair(ending.at(ending.size() - (query.k - pruner - beginning.size())), edge.length));
    influenceZone->partialEdges->insert(make_pair(edge.edgeID, segments));
}
/*
void
Zone::outputZoneNetwork(InfluenceZone* influenceZone)
{
    // cout << "output zone network..." << endl;
    string resultFileName = Argument::getZonePrefix();
    ofstream resultFile;
    resultFile.open(resultFileName.c_str());
    if (resultFile.is_open())
    {
        resultFile << *influenceZone << endl;
        resultFile.close();
    }
    else cout << "Unable to open file : " << resultFileName << endl;
}
*/
void
Zone::indexZone(InfluenceZone* influenceZone, PointID_t queryPointID)
{
    // cout << "influenceZone.zoneNodes.size() : " << influenceZone->zoneNodes->size() << endl;
    Argument::zoneNodes += influenceZone->zoneNodes->size();
    Argument::zoneFullEdges += influenceZone->fullEdges->size();
    Argument::zonePartialEdges += influenceZone->partialEdges->size();
    Argument::touchedNodes += influenceZone->touchedNodes->size();
    Argument::touchedEdges += influenceZone->touchedEdges->size();

    for(EIDSet_t::const_iterator iter = influenceZone->fullEdges->begin(); iter != influenceZone->fullEdges->end(); iter++)
    {
        Edge &edge = Data::edges.at(*iter);
        edge.addFullEdge(queryPointID);
        Argument::zoneLength += edge.length;
    }

    for(EIDSegmentSetMap_t::const_iterator itere = influenceZone->partialEdges->begin(); itere != influenceZone->partialEdges->end(); itere++)
    {
        Edge &edge = Data::edges.at(itere->first);
        edge.addPartialEdge(queryPointID, itere->second);
        for(SegmentSet_t::const_iterator iters = itere->second.begin(); iters != itere->second.end(); iters++)
            Argument::zoneLength += iters->second - iters->first;
    }
}
