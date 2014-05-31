#include "UpdatableZone.h"

InfluenceZone*
UpdatableZone::computeZone(PointID_t queryPointID)
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
UpdatableZone::init(Point &query, NIDDisMap_t &zMinDis, NIDMinHeap_t &zQueue, InfluenceZone* influenceZone)
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
UpdatableZone::nDijkstra(Point &query, NodeID_t nid, Distance_t limit, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
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
        Distance_t disToN = nQueue.begin()->first;
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
                    Distance_t disFToN = disToN + iterp->first;
                    if(disFToN + Argument::EPS < limit)
                    {
                        facilities.insert(iterp->second);
                        if(nfMinDis[nid].find(iterp->second) == nfMinDis[nid].end() || nfMinDis[nid][iterp->second] > disFToN)
                            nfMinDis[nid][iterp->second] = disFToN;
                    }
                }
            else
                for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++)
                {
                    Distance_t disFToN = disToN + edge.length - iterp->first;
                    if(disFToN + Argument::EPS < limit)
                    {
                        facilities.insert(iterp->second);
                        if(nfMinDis[nid].find(iterp->second) == nfMinDis[nid].end() || nfMinDis[nid][iterp->second] > disFToN)
                            nfMinDis[nid][iterp->second] = disFToN;
                    }
                }
        }
    }
    if(facilities.size() >= query.k)
        return false;
    return true;
}

void
UpdatableZone::computeBoundaryB(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
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
    Distance_t distance = 0;
    Distance_t dis2Query = 0;
    if(out == edge.start)
    {
        distance = length.at(length.size() - index);
        segments.insert(make_pair(distance, edge.length));
        dis2Query = zMinDis[in] + edge.length - distance;
    }
    else
    {
        distance = edge.length - length.at(length.size() - index);
        segments.insert(make_pair(0, distance));
        dis2Query = zMinDis[in] + distance;
    }
    influenceZone->partialEdges->insert(make_pair(edge.edgeID, segments));

    // calculate boundary point and boundary zone
    BoundaryID_t boundaryID(query.pointID, edge.edgeID, distance, in);
    BoundaryZone* boundaryZone;
    boundaryZone = computeBoundaryZone(query, boundaryID, dis2Query);
    indexBoundaryZone(boundaryID, boundaryZone);
}

void
UpdatableZone::computeBoundaryC(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
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

        // calculate boundary point and boundary zone
        if(from != 0)
        {
            BoundaryID_t boundaryID(query.pointID, edge.edgeID, from);
            BoundaryZone* boundaryZone;
            boundaryZone = computeBoundaryZone(query, boundaryID, query.disToStart - from);
            indexBoundaryZone(boundaryID, boundaryZone);
        }
        if(to != edge.length)
        {
            BoundaryID_t boundaryID(query.pointID, edge.edgeID, to);
            BoundaryZone* boundaryZone;
            boundaryZone = computeBoundaryZone(query, boundaryID, to - query.disToStart);
            indexBoundaryZone(boundaryID, boundaryZone);
        }
    }
}

void
UpdatableZone::computeBoundaryD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
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
    Distance_t distance = 0;
    Distance_t dis2Query = 0;
    if(out == edge.start)
    {
        distance = length.at(length.size() - index);
        segments.insert(make_pair(distance, edge.length));
        dis2Query = zMinDis[in] + edge.length - distance;
    }
    else
    {
        distance = edge.length - length.at(length.size() - index);
        segments.insert(make_pair(0, distance));
        dis2Query = zMinDis[in] + distance;
    }
    influenceZone->partialEdges->insert(make_pair(edge.edgeID, segments));

    // calculate boundary point and boundary zone
    BoundaryID_t boundaryID(query.pointID, edge.edgeID, distance, in);
    BoundaryZone* boundaryZone;
    boundaryZone = computeBoundaryZone(query, boundaryID, dis2Query);
    indexBoundaryZone(boundaryID, boundaryZone);
}

void
UpdatableZone::computeBoundaryDInIn(Point &query, Edge &edge, NIDDisMap_t &zMinDis, NIDPIDDisMap_t &nfMinDis, InfluenceZone* influenceZone)
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
    Distance_t distanceFrom = ending.at(ending.size() - (query.k - pruner - beginning.size()));
    Distance_t distanceTo = edge.length - beginning.at(beginning.size() - (query.k - pruner - ending.size()));
    segments.insert(make_pair(0, distanceTo));
    segments.insert(make_pair(distanceFrom, edge.length));
    influenceZone->partialEdges->insert(make_pair(edge.edgeID, segments));

    // calculate boundary point and boundary zone
    BoundaryID_t boundaryID1(query.pointID, edge.edgeID, distanceFrom, edge.end);
    BoundaryZone* boundaryZone1;
    boundaryZone1 = computeBoundaryZone(query, boundaryID1, zMinDis[edge.end] + edge.length - distanceFrom);
    indexBoundaryZone(boundaryID1, boundaryZone1);

    BoundaryID_t boundaryID2(query.pointID, edge.edgeID, distanceTo, edge.start);
    BoundaryZone* boundaryZone2;
    boundaryZone2 = computeBoundaryZone(query, boundaryID2, zMinDis[edge.start] + distanceTo);
    indexBoundaryZone(boundaryID2, boundaryZone2);
}
/*
void
UpdatableZone::outputZoneNetwork(InfluenceZone* influenceZone)
{
    cout << "output zone network..." << endl;
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

void
UpdatableZone::outputZoneNetworkUpdateBefore(PointID_t queryPointID)
{
    cout << "output zone network..." << endl;
    string resultFileName = Argument::getZonePrefixUpdateBefore();
    ofstream resultFile;
    resultFile.open(resultFileName.c_str());
    if (resultFile.is_open())
    {
        for(Index_t ie=0; ie<Data::numEdges; ie++)
        {
            Edge &edge = Data::edges.at(ie);
            if(edge.fullEdgeQueries->find(queryPointID) != edge.fullEdgeQueries->end())
                resultFile << edge.edgeID << endl;
            for(PIDSegmentSetMap_t::const_iterator iterm = edge.partialEdgeQueries->begin(); iterm != edge.partialEdgeQueries->end(); iterm++)
                if(iterm->first == queryPointID)
                    for(SegmentSet_t::const_iterator iters = iterm->second.begin(); iters != iterm->second.end(); iters++)
                        resultFile << edge.edgeID << " [" << iters->first << ", " << iters->second << "]" << endl;
        }
        resultFile.close();
    }
    else cout << "Unable to open file : " << resultFileName << endl;
}

void
UpdatableZone::outputZoneNetworkUpdate(PointID_t queryPointID)
{
    cout << "output zone network..." << endl;
    string resultFileName = Argument::getZonePrefixUpdate();
    ofstream resultFile;
    resultFile.open(resultFileName.c_str());
    if (resultFile.is_open())
    {
        for(Index_t ie=0; ie<Data::numEdges; ie++)
        {
            Edge &edge = Data::edges.at(ie);
            if(edge.fullEdgeQueries->find(queryPointID) != edge.fullEdgeQueries->end())
                resultFile << edge.edgeID << endl;
            for(PIDSegmentSetMap_t::const_iterator iterm = edge.partialEdgeQueries->begin(); iterm != edge.partialEdgeQueries->end(); iterm++)
                if(iterm->first == queryPointID)
                    for(SegmentSet_t::const_iterator iters = iterm->second.begin(); iters != iterm->second.end(); iters++)
                        resultFile << edge.edgeID << " [" << iters->first << ", " << iters->second << "]" << endl;
        }
        resultFile.close();
    }
    else cout << "Unable to open file : " << resultFileName << endl;
}
*/
void
UpdatableZone::indexZone(InfluenceZone* influenceZone, PointID_t queryPointID)
{
    // cout << "influenceZone.zoneNodes.size() : " << influenceZone->zoneNodes->size() << endl;
    Argument::zoneNodes += influenceZone->zoneNodes->size();
    Argument::zoneFullEdges += influenceZone->fullEdges->size();
    Argument::zonePartialEdges += influenceZone->partialEdges->size();
    Argument::touchedNodes += influenceZone->touchedNodes->size();
    Argument::touchedEdges += influenceZone->touchedEdges->size();

    for(NIDSet_t::const_iterator itern = influenceZone->zoneNodes->begin(); itern != influenceZone->zoneNodes->end(); itern++)
        Data::nodes.at(*itern).addQueryAsInnerNode(queryPointID);

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

void
UpdatableZone::removeZone(PointID_t queryPointID)
{
    Point &query = Data::points.at(queryPointID);

    // remove boundary zone
    while(query.boundaryDis->size() > 0)
    {
        BoundaryID_t bid = query.boundaryDis->begin()->first;
        removeBoundaryZone(bid);
    }

    // remove zone
    NIDDisMap_t zMinDis; // ........... min distance from the query point to all nodes
    NIDMinHeap_t zQueue; // ........... priority queue / min heap

    Edge &edge = Data::edges.at(query.edgeID);

    zMinDis[edge.start] = query.disToStart;
    zMinDis[edge.end] = edge.length - query.disToStart;

    zQueue.insert(make_pair(zMinDis[edge.start], edge.start));
    zQueue.insert(make_pair(zMinDis[edge.end], edge.end));

    while(!zQueue.empty())
    {
        NodeID_t node = zQueue.begin()->second;
        Distance_t disToQ = zQueue.begin()->first;
        zQueue.erase(zQueue.begin());
        zMinDis[node] = disToQ;
        bool dead = Data::nodes.at(node).isInnerNode(queryPointID);
        if(!dead)
        {
            Data::nodes.at(node).removeQueryAsInnerNode(queryPointID);
            EdgePV_t &epv = Data::adjMap[node];
            for(Index_t ie=0; ie<epv.size(); ie++)
            {
                Edge &edge = *epv.at(ie);
                edge.removeQuery(queryPointID);

                // find the node at the other end of the edge
                NodeID_t next = ( node==edge.start ? edge.end : edge.start );

                // if the next node hasn't been explored or a better path found for it
                if(zMinDis.find(next) == zMinDis.end() || zMinDis[node] + edge.length < zMinDis[next])
                {
                    zQueue.erase(make_pair(zMinDis[next], next)); // for better path
                    // update priority queue
                    zMinDis[next] = zMinDis[node] + edge.length;
                    zQueue.insert(make_pair(zMinDis[next], next));
                }
            }
        }
    }
}

BoundaryZone*
UpdatableZone::computeBoundaryZone(Point &query, BoundaryID_t boundaryID, Distance_t dis2Query)
{
    query.addBoundaryDis(boundaryID, dis2Query);

    NIDDisMap_t zMinDis; // ........... min distance from the boundary point to all nodes
    NIDMinHeap_t zQueue; // ........... priority queue / min heap

    BoundaryZone* boundaryZone = new BoundaryZone();

    Edge &edge = Data::edges.at(boundaryID.edgeID);

    Distance_t from = (boundaryID.dis <= dis2Query ? 0 : boundaryID.dis - dis2Query);
    Distance_t to = (boundaryID.dis + dis2Query < edge.length ? boundaryID.dis + dis2Query : edge.length);

    if(from == 0 && to == edge.length)
        boundaryZone->fullEdges->insert(edge.edgeID);
    else
    {
        SegmentSet_t segments;
        segments.insert(make_pair(from, to));
        boundaryZone->partialEdges->insert(make_pair(edge.edgeID, segments));
    }

    zMinDis[edge.start] = boundaryID.dis;
    zMinDis[edge.end] = edge.length - boundaryID.dis;

    if(from == 0)
        zQueue.insert(make_pair(zMinDis[edge.start], edge.start));
    if(to == edge.length)
        zQueue.insert(make_pair(zMinDis[edge.end], edge.end));

    while(!zQueue.empty() && zQueue.begin()->first < dis2Query)
    {
        NodeID_t node = zQueue.begin()->second;
        zQueue.erase(zQueue.begin());
        boundaryZone->zoneNodes->insert(node);

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
            }
        }
    }

    // collect edges intersect with boundary zone
    EIDSet_t zoneEdges;
    zoneEdges.insert(boundaryID.edgeID);
    for(NIDSet_t::const_iterator itern = boundaryZone->zoneNodes->begin(); itern != boundaryZone->zoneNodes->end(); itern++)
    {
        EdgePV_t &epv = Data::adjMap[*itern];
            for(Index_t ie=0; ie<epv.size(); ie++)
            {
                Edge &edge = *epv.at(ie);
                zoneEdges.insert(edge.edgeID);
            }
    }
    zoneEdges.erase(boundaryID.edgeID);

    // check the set of candidate boundary zone edge
    for(EIDSet_t::const_iterator itere = zoneEdges.begin(); itere != zoneEdges.end(); itere++)
    {
        Edge &edge = Data::edges.at(*itere);
        bool startIn = (boundaryZone->zoneNodes->find(edge.start) != boundaryZone->zoneNodes->end());
        bool endIn = (boundaryZone->zoneNodes->find(edge.end) != boundaryZone->zoneNodes->end());
        if(startIn && endIn)
        {
            if(dis2Query - zMinDis[edge.start] + dis2Query - zMinDis[edge.end] >= edge.length)
            {
                boundaryZone->fullEdges->insert(edge.edgeID);
                continue;
            }
            else
            {
                SegmentSet_t segments;
                Distance_t distanceFrom = zMinDis[edge.end] + edge.length - dis2Query;
                Distance_t distanceTo = dis2Query - zMinDis[edge.start];
                segments.insert(make_pair(0, distanceTo));
                segments.insert(make_pair(distanceFrom, edge.length));
                boundaryZone->partialEdges->insert(make_pair(edge.edgeID, segments));
            }
        }
        else if(startIn && !endIn)
        {
            SegmentSet_t segments;
            Distance_t distanceTo = dis2Query - zMinDis[edge.start];
            segments.insert(make_pair(0, distanceTo));
            boundaryZone->partialEdges->insert(make_pair(edge.edgeID, segments));
        }
        else if(!startIn && endIn)
        {
            SegmentSet_t segments;
            Distance_t distanceFrom = zMinDis[edge.end] + edge.length - dis2Query;
            segments.insert(make_pair(distanceFrom, edge.length));
            boundaryZone->partialEdges->insert(make_pair(edge.edgeID, segments));
        }
        else
        {
            cout << "UpdatableZone::computeBoundaryZone error" << endl;
            exit(EXIT_SUCCESS);
        }
    }

    return boundaryZone;
}

void
UpdatableZone::indexBoundaryZone(BoundaryID_t boundaryID, BoundaryZone* boundaryZone)
{
    Argument::bZoneNodes += boundaryZone->zoneNodes->size();
    Argument::bZoneFullEdges += boundaryZone->fullEdges->size();
    Argument::bZonePartialEdges += boundaryZone->partialEdges->size();

    for(EIDSet_t::const_iterator iter = boundaryZone->fullEdges->begin(); iter != boundaryZone->fullEdges->end(); iter++)
    {
        Edge &edge = Data::edges.at(*iter);
        edge.addFullEdgeBoundary(boundaryID);
        Argument::bZoneLength += edge.length;
    }

    for(EIDSegmentSetMap_t::const_iterator itere = boundaryZone->partialEdges->begin(); itere != boundaryZone->partialEdges->end(); itere++)
    {
        Edge &edge = Data::edges.at(itere->first);
        edge.addPartialEdgeBoundary(boundaryID, itere->second);
        for(SegmentSet_t::const_iterator iters = itere->second.begin(); iters != itere->second.end(); iters++)
            Argument::bZoneLength += iters->second - iters->first;
    }

    boundaryZone->clean();
    if(boundaryZone) delete boundaryZone;
}

void
UpdatableZone::removeBoundaryZone(BoundaryID_t boundaryID)
{
    Point &query = Data::points.at(boundaryID.query);
    Distance_t dis2Query = (*query.boundaryDis)[boundaryID];
    query.boundaryDis->erase(boundaryID);

    NIDDisMap_t zMinDis; // ........... min distance from the boundary point to all nodes
    NIDMinHeap_t zQueue; // ........... priority queue / min heap

    Edge &edge = Data::edges.at(boundaryID.edgeID);
    edge.fullEdgeBoundaries->erase(boundaryID);
    edge.partialEdgeBoundaries->erase(boundaryID);

    zMinDis[edge.start] = boundaryID.dis;
    zMinDis[edge.end] = edge.length - query.disToStart;

    zQueue.insert(make_pair(zMinDis[edge.start], edge.start));
    zQueue.insert(make_pair(zMinDis[edge.end], edge.end));

    while(!zQueue.empty() && zQueue.begin()->first < dis2Query)
    {
        NodeID_t node = zQueue.begin()->second;
        Distance_t disToB = zQueue.begin()->first;
        zQueue.erase(zQueue.begin());
        zMinDis[node] = disToB;

        EdgePV_t &epv = Data::adjMap[node];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            edge.fullEdgeBoundaries->erase(boundaryID);
            edge.partialEdgeBoundaries->erase(boundaryID);

            // find the node at the other end of the edge
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );

            // if the next node hasn't been explored or a better path found for it
            if(zMinDis.find(next) == zMinDis.end() || zMinDis[node] + edge.length < zMinDis[next])
            {
                zQueue.erase(make_pair(zMinDis[next], next)); // for better path
                // update priority queue
                zMinDis[next] = zMinDis[node] + edge.length;
                zQueue.insert(make_pair(zMinDis[next], next));
            }
        }
    }
}

void
UpdatableZone::shrink(PointID_t queryPointID, BIDSet_t &expiredBoundaries)
{
    // cout << "shrink " << queryPointID << " " << expiredBoundaries.size() << endl;
    NIDPIDDisMap_t nfMinDis; // ....... shortest network distance from nodes to their pruner facilities
    EIDSet_t recalculatedEdges;
    NIDMinHeap_t zQueue;     // ........... priority queue / min heap
    Point &query = Data::points.at(queryPointID);

    for(BIDSet_t::const_iterator iterb = expiredBoundaries.begin(); iterb != expiredBoundaries.end(); iterb++)
    {
        // clean up boundary edge
        Edge &edge = Data::edges.at(iterb->edgeID);
        if(recalculatedEdges.find(edge.edgeID) != recalculatedEdges.end())
            continue;
        edge.removeQuery(queryPointID);

        // calculate boundary on the original boundary edge, or, init search for new boundary edge
        if(iterb->innerNode == BAD_ID) // indicates the boundary edge is the query edge
        {
            if(edge.edgeID != query.edgeID)
            {
                cout << "UpdatableZone::shrink error edge.edgeID != query.edgeID" << endl;
                exit(EXIT_SUCCESS);
            }
            Distance_t limit = Argument::INF;
            bool startIn = nDijkstra(query, edge.start, limit, nfMinDis);
            if(!startIn)
                Data::nodes.at(edge.start).removeQueryAsInnerNode(queryPointID);
            limit = Argument::INF;
            bool endIn = nDijkstra(query, edge.end, limit, nfMinDis);
            if(!endIn)
                Data::nodes.at(edge.end).removeQueryAsInnerNode(queryPointID);
            computeBoundaryC(query, edge, startIn, endIn, nfMinDis);
            recalculatedEdges.insert(edge.edgeID);
        }
        else
        {
            Distance_t inDis2Q = Argument::INF;
            if(nDijkstra(query, iterb->innerNode, inDis2Q, nfMinDis))
            {
                NodeID_t out = (iterb->innerNode == edge.start ? edge.end : edge.start);
                Distance_t outDis2Q = Argument::INF;
                if(nDijkstra(query, out, outDis2Q, nfMinDis))
                {
                    if(out == edge.start)
                        computeBoundaryDInIn(query, edge, outDis2Q, inDis2Q, nfMinDis);
                    else
                        computeBoundaryDInIn(query, edge, inDis2Q, outDis2Q, nfMinDis);
                }
                else
                    computeBoundaryBD(query, edge, iterb->innerNode, out, inDis2Q, outDis2Q, nfMinDis);
                recalculatedEdges.insert(edge.edgeID);
            }
            else
                zQueue.insert(make_pair(inDis2Q, iterb->innerNode));
        }
    }

    // search for new boundary edge
    while(zQueue.size() > 0)
    {
        Distance_t outDis2Q = zQueue.begin()->first;
        NodeID_t out = zQueue.begin()->second;
        zQueue.erase(zQueue.begin());
        Data::nodes.at(out).removeQueryAsInnerNode(queryPointID);

        EdgePV_t &epv = Data::adjMap[out];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            if(recalculatedEdges.find(edge.edgeID) != recalculatedEdges.end())
                continue;

            // find the node at the other end of the edge
            NodeID_t in = ( out==edge.start ? edge.end : edge.start );

            if(edge.edgeID == query.edgeID)
            {
                edge.removeQuery(queryPointID);
                Distance_t limit = Argument::INF;
                bool startIn = false, endIn = false;
                if(in == edge.start)
                    startIn = nDijkstra(query, in, limit, nfMinDis);
                else
                    endIn = nDijkstra(query, in, limit, nfMinDis);
                computeBoundaryC(query, edge, startIn, endIn, nfMinDis);
                recalculatedEdges.insert(edge.edgeID);
                continue;
            }

            if(Data::nodes.at(in).isInnerNode(queryPointID))
            {
                edge.removeQuery(queryPointID);
                Distance_t inDis2Q = Argument::INF;
                if(nDijkstra(query, in, inDis2Q, nfMinDis))
                {
                    computeBoundaryBD(query, edge, in, out, inDis2Q, outDis2Q, nfMinDis);
                    recalculatedEdges.insert(edge.edgeID);
                }
                else
                    zQueue.insert(make_pair(inDis2Q, in));
            }
            else
                ; // do nothing, must already be parsed
        }
    }
}

void
UpdatableZone::enlarge(PointID_t queryPointID, BIDSet_t &expiredBoundaries)
{
    // cout << "enlarge " << queryPointID << " " << expiredBoundaries.size() << endl;
    NIDPIDDisMap_t nfMinDis; // ....... shortest network distance from nodes to their pruner facilities
    EIDSet_t recalculatedEdges;
    NIDMinHeap_t zQueue;     // ........... priority queue / min heap
    Point &query = Data::points.at(queryPointID);

    for(BIDSet_t::const_iterator iterb = expiredBoundaries.begin(); iterb != expiredBoundaries.end(); iterb++)
    {
        // clean up boundary edge
        Edge &edge = Data::edges.at(iterb->edgeID);
        if(recalculatedEdges.find(edge.edgeID) != recalculatedEdges.end())
            continue;
        edge.removeQuery(queryPointID);

        // calculate boundary on the original boundary edge, or, init search for new boundary edge
        if(iterb->innerNode == BAD_ID) // indicates the boundary edge is the query edge
        {
            bool startInBefore = Data::nodes.at(edge.start).isInnerNode(queryPointID);
            bool endInBefore = Data::nodes.at(edge.end).isInnerNode(queryPointID);
            Distance_t disQStart = Argument::INF;
            bool startIn = nDijkstra(query, edge.start, disQStart, nfMinDis);
            Distance_t disQEnd = Argument::INF;
            bool endIn = nDijkstra(query, edge.end, disQEnd, nfMinDis);
            computeBoundaryC(query, edge, startIn, endIn, nfMinDis);
            recalculatedEdges.insert(edge.edgeID);
            if(!startInBefore && startIn)
                zQueue.insert(make_pair(disQStart, edge.start));
            if(!endInBefore && endIn)
                zQueue.insert(make_pair(disQEnd, edge.end));
        }
        else
        {
            NodeID_t out = ( iterb->innerNode == edge.start ? edge.end : edge.start);
            bool outInBefore = Data::nodes.at(out).isInnerNode(queryPointID);
            Distance_t inDis2Q = Argument::INF;
            nDijkstra(query, iterb->innerNode, inDis2Q, nfMinDis); // inner node must still in
            Distance_t outDis2Q = Argument::INF;
            bool outIn = nDijkstra(query, out, outDis2Q, nfMinDis);
            if(outIn)
            {
                if(out == edge.start)
                    computeBoundaryDInIn(query, edge, outDis2Q, inDis2Q, nfMinDis);
                else
                    computeBoundaryDInIn(query, edge, inDis2Q, outDis2Q, nfMinDis);
            }
            else
                computeBoundaryBD(query, edge, iterb->innerNode, out, inDis2Q, outDis2Q, nfMinDis);
            recalculatedEdges.insert(edge.edgeID);
            if(!outInBefore && outIn)
                zQueue.insert(make_pair(outDis2Q, out));
        }
    }

    // search for new boundary edge
    while(zQueue.size() > 0)
    {
        Distance_t inDis2Q = zQueue.begin()->first;
        NodeID_t in = zQueue.begin()->second;
        zQueue.erase(zQueue.begin());
        Data::nodes.at(in).addQueryAsInnerNode(queryPointID);

        EdgePV_t &epv = Data::adjMap[in];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            if(recalculatedEdges.find(edge.edgeID) != recalculatedEdges.end())
                continue;

            // find the node at the other end of the edge
            NodeID_t out = ( in==edge.start ? edge.end : edge.start );
            bool outInBefore = Data::nodes.at(out).isInnerNode(queryPointID);
            if(outInBefore)
                continue;

            edge.removeQuery(queryPointID);

            Distance_t outDis2Q = Argument::INF;
            bool outIn = nDijkstra(query, out, outDis2Q, nfMinDis);
            if(outIn)
            {
                if(out == edge.start)
                    computeBoundaryDInIn(query, edge, outDis2Q, inDis2Q, nfMinDis);
                else
                    computeBoundaryDInIn(query, edge, inDis2Q, outDis2Q, nfMinDis);
            }
            else
                computeBoundaryBD(query, edge, in, out, inDis2Q, outDis2Q, nfMinDis);
            recalculatedEdges.insert(edge.edgeID);
            if(!outInBefore && outIn)
                zQueue.insert(make_pair(outDis2Q, out));
        }
    }
}

bool
UpdatableZone::nDijkstra(Point &query, NodeID_t nid, Distance_t &limit, NIDPIDDisMap_t &nfMinDis)
{
    /// encounter nodes
    NIDDisMap_t nMinDis; // ............ min distance from a node to all nodes
    NIDMinHeap_t nQueue; // ............ priority queue for a node
    PIDSet_t facilities; // ............ collect of encountered facilities
    NIDSet_t nNodes; // ................ visited (decided) nodes

    nQueue.insert(make_pair(0, nid));

    while(!nQueue.empty() && nQueue.begin()->first < limit)
    {
        /* /// this can not be included, as false positive may present (limit is to be determined on the fly)
        if(facilities.size() >= query.k)
            return false;*/

        NodeID_t node = nQueue.begin()->second;
        Distance_t disToN = nQueue.begin()->first;
        nQueue.erase(nQueue.begin());
        nNodes.insert(node);

        EdgePV_t &epv = Data::adjMap[node];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            // find the node at the other end of the edge
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );

            // if the next node hasn't been explored or a better path found for it
            if(nMinDis.find(next) == nMinDis.end() || nMinDis[node] + edge.length < nMinDis[next])
            {
                nQueue.erase(make_pair(nMinDis[next], next)); // for better path
                // update priority queue
                nMinDis[next] = nMinDis[node] + edge.length;
                nQueue.insert(make_pair(nMinDis[next], next));
            }

            // find facilities along this edge
            if(node == edge.start)
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                {
                    Distance_t disFToN = disToN + iterp->first;
                    if(iterp->second == query.pointID && disFToN < limit)
                    {
                        limit = disFToN;
                        break;
                    }
                    if(disFToN + Argument::EPS < limit)
                    {
                        facilities.insert(iterp->second);
                        if(nfMinDis[nid].find(iterp->second) == nfMinDis[nid].end() || nfMinDis[nid][iterp->second] > disFToN)
                            nfMinDis[nid][iterp->second] = disFToN;
                    }
                }
            else
                for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++)
                {
                    Distance_t disFToN = disToN + edge.length - iterp->first;
                    if(iterp->second == query.pointID && disFToN < limit)
                    {
                        limit = disFToN;
                        break;
                    }
                    if(disFToN + Argument::EPS < limit)
                    {
                        facilities.insert(iterp->second);
                        if(nfMinDis[nid].find(iterp->second) == nfMinDis[nid].end() || nfMinDis[nid][iterp->second] > disFToN)
                            nfMinDis[nid][iterp->second] = disFToN;
                    }
                }
        }
    }
    Amount_t pruner = 0;
    for(PIDSet_t::iterator iterf = facilities.begin(); iterf != facilities.end(); iterf++)
        if(nfMinDis[nid][*iterf] + Argument::EPS < limit)
            pruner++;
        else
            nfMinDis[nid].erase(*iterf);
    if(pruner >= query.k)
        return false;
    return true;
}

void
UpdatableZone::computeBoundaryBD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, Distance_t disQIn, Distance_t disQOut, NIDPIDDisMap_t &nfMinDis)
{
    if(disQIn + edge.length < disQOut + Argument::EPS && disQIn + edge.length > disQOut - Argument::EPS)
        computeBoundaryB(query, edge, in, out, disQIn, disQOut, nfMinDis);
    else if(disQOut + edge.length < disQIn + Argument::EPS && disQOut + edge.length > disQIn - Argument::EPS)
    {
        cout << "UpdatableZone::computeBoundaryBD error" << endl;
        exit(EXIT_SUCCESS);
    }
    else
        computeBoundaryD(query, edge, in, out, disQIn, disQOut, nfMinDis);
}

void
UpdatableZone::computeBoundaryB(Point &query, Edge &edge, NodeID_t in, NodeID_t out, Distance_t disQIn, Distance_t disQOut, NIDPIDDisMap_t &nfMinDis)
{
    // cout << "UpdatableZone::computeBoundaryB" << endl;
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
    Distance_t distance = 0;
    Distance_t dis2Query = 0;
    if(out == edge.start)
    {
        distance = length.at(length.size() - index);
        segments.insert(make_pair(distance, edge.length));
        dis2Query = disQIn + edge.length - distance;
    }
    else
    {
        distance = edge.length - length.at(length.size() - index);
        segments.insert(make_pair(0, distance));
        dis2Query = disQIn + distance;
    }
    edge.addPartialEdge(query.pointID, segments);

    // calculate boundary point and boundary zone
    BoundaryID_t boundaryID(query.pointID, edge.edgeID, distance, in);
    BoundaryZone* boundaryZone;
    boundaryZone = computeBoundaryZone(query, boundaryID, dis2Query);
    indexBoundaryZone(boundaryID, boundaryZone);
}

void
UpdatableZone::computeBoundaryC(Point &query, Edge &edge, bool startIn, bool endIn, NIDPIDDisMap_t &nfMinDis)
{
    // cout << "UpdatableZone::computeBoundaryC" << endl;
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
    else if(!startIn)
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
    else if(!endIn)
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
        edge.addFullEdge(query.pointID);
    else
    {
        SegmentSet_t segments;
        segments.insert(make_pair(from, to));
        edge.addPartialEdge(query.pointID, segments);

        // calculate boundary point and boundary zone
        if(from != 0)
        {
            BoundaryID_t boundaryID(query.pointID, edge.edgeID, from);
            BoundaryZone* boundaryZone;
            boundaryZone = computeBoundaryZone(query, boundaryID, query.disToStart - from);
            indexBoundaryZone(boundaryID, boundaryZone);
        }
        if(to != edge.length)
        {
            BoundaryID_t boundaryID(query.pointID, edge.edgeID, to);
            BoundaryZone* boundaryZone;
            boundaryZone = computeBoundaryZone(query, boundaryID, to - query.disToStart);
            indexBoundaryZone(boundaryID, boundaryZone);
        }
    }
}

void
UpdatableZone::computeBoundaryD(Point &query, Edge &edge, NodeID_t in, NodeID_t out, Distance_t disQIn, Distance_t disQOut, NIDPIDDisMap_t &nfMinDis)
{
    // cout << "UpdatableZone::computeBoundaryD" << endl;
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
    Distance_t distance = 0;
    Distance_t dis2Query = 0;
    if(out == edge.start)
    {
        distance = length.at(length.size() - index);
        segments.insert(make_pair(distance, edge.length));
        dis2Query = disQIn + edge.length - distance;
    }
    else
    {
        distance = edge.length - length.at(length.size() - index);
        segments.insert(make_pair(0, distance));
        dis2Query = disQIn + distance;
    }
    edge.addPartialEdge(query.pointID, segments);

    // calculate boundary point and boundary zone
    BoundaryID_t boundaryID(query.pointID, edge.edgeID, distance, in);
    BoundaryZone* boundaryZone;
    boundaryZone = computeBoundaryZone(query, boundaryID, dis2Query);
    indexBoundaryZone(boundaryID, boundaryZone);
}

void
UpdatableZone::computeBoundaryDInIn(Point &query, Edge &edge, Distance_t disQStart, Distance_t disQEnd, NIDPIDDisMap_t &nfMinDis)
{
    // cout << "UpdatableZone::computeBoundaryDInIn" << endl;
    PIDDisMap_t &minDisStart = nfMinDis[edge.start];
    PIDDisMap_t &minDisEnd = nfMinDis[edge.end];

    Amount_t pruner = 0;
    for(PIDDisMap_t::const_iterator iterp = minDisStart.begin(); iterp != minDisStart.end(); iterp++)
        if(minDisEnd.find(iterp->first) != minDisEnd.end())
            pruner++;

    if(minDisStart.size() + minDisEnd.size() - pruner < query.k)
    {
        edge.addFullEdge(query.pointID);
        return;
    }

    PIDSet_t onEdgePoints;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
        onEdgePoints.insert(iterp->second);

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
    Distance_t distanceFrom = ending.at(ending.size() - (query.k - pruner - beginning.size()));
    Distance_t distanceTo = edge.length - beginning.at(beginning.size() - (query.k - pruner - ending.size()));
    segments.insert(make_pair(0, distanceTo));
    segments.insert(make_pair(distanceFrom, edge.length));
    edge.addPartialEdge(query.pointID, segments);

    // calculate boundary point and boundary zone
    BoundaryID_t boundaryID1(query.pointID, edge.edgeID, distanceFrom, edge.end);
    BoundaryZone* boundaryZone1;
    boundaryZone1 = computeBoundaryZone(query, boundaryID1, disQEnd + edge.length - distanceFrom);
    indexBoundaryZone(boundaryID1, boundaryZone1);

    BoundaryID_t boundaryID2(query.pointID, edge.edgeID, distanceTo, edge.start);
    BoundaryZone* boundaryZone2;
    boundaryZone2 = computeBoundaryZone(query, boundaryID2, disQStart + distanceTo);
    indexBoundaryZone(boundaryID2, boundaryZone2);
}

