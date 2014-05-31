#include "SAC.h"

void
SAC::computeUnprunedNetworkForAll()
{
    // cout << "SAC computeUnprunedNetworkForAll..." << endl;
    queryFileName = Argument::getQueryFileName();
    queryFile.open(queryFileName.c_str());
    if (!queryFile.is_open())
    {
        cout << "Unable to open file : " << queryFileName << endl;
        return;
    }

    clock_t start, stop;
    INT64 totalCalculation = 0;

    for(Index_t n=0; n<Argument::numQueries; n++)
    {
        Index_t queryPointIndex;
        PointID_t queryPointID;
        Amount_t k;
        queryFile >> queryPointIndex >> queryPointID >> k;
        Point &query = Data::points.at(queryPointID);
        query.k = k;
        Argument::queryPointID = queryPointID;

        start = clock();
        UnprunedNetwork* unprunedNetwork = computeUnprunedNetwork(query);
        stop = clock();
        totalCalculation += stop - start;

        indexUnprunedNetwork(unprunedNetwork, query);

        unprunedNetwork->clean();
        if(unprunedNetwork)
            delete unprunedNetwork;
    }

    queryFile.close();

    cout << "calculation time : " << totalCalculation << endl;
    Argument::calculation = totalCalculation;
}

void
SAC::monitor()
{
    // cout << "SAC monitor..." << endl;
    objectFileName = Argument::getObjectFileName();
    objectFile.open(objectFileName.c_str());
    if (!objectFile.is_open())
    {
        cout << "Unable to open file : " << objectFileName << endl;
        return;
    }

    int timestamp;
    // int count = 0;

    clock_t start, stop;
    INT64 totalClock = 0;

    for(Index_t t=0; t<Argument::timeStamp; t++)
    {
        // load object location info from file for 1 time stamp
        objectFile >> timestamp;
        for(Index_t oi=0; oi<Argument::numObjects * Argument::movePercent / 100; oi++)
        {
            if(t==0)
            {
                Object object;
                objectFile >> object;
                objects.push_back(object);
            }
            else
                objectFile >> objects.at(oi);
            if(objects.at(oi).objectID != oi)
            {
                cout << "SACMonitor::monitor() objectID error" << endl;
                exit(EXIT_SUCCESS);
            }
        }

        start = clock();
        // check if they are in the influence zone or not
        for(ObjectV_t::iterator itero = objects.begin(); itero != objects.end(); itero++)
        {
            // erase invalid monitored network
            for(MonNetworkMap_t::iterator itermn = itero->monitoredNetworkMap->begin(); itermn != itero->monitoredNetworkMap->end();)
                if(!itermn->second.isValid(itero->curEdgeID, itero->curDis))
                    itero->monitoredNetworkMap->erase(itermn++);
                else
                    itermn++;

            // update monitored network
            Edge &edge = Data::edges.at(itero->curEdgeID);
            for(PIDSet_t::const_iterator iterp = edge.fullEdgeQueries->begin(); iterp != edge.fullEdgeQueries->end(); iterp++)
            {
                // check if the query point is on the same edge with the car, if so, no need to compute monitored network, since such case is simple to verify
                EdgeID_t queryEdge = Data::points.at(*iterp).edgeID;
                if(edge.edgeID == queryEdge)
                    continue;
                if(t==0 || itero->monitoredNetworkMap->find(*iterp) == itero->monitoredNetworkMap->end())
                    computeMonitoredNetwork(*iterp, *itero, edge, make_pair(0, edge.length));
            }
            for(PIDSegmentSetMap_t::const_iterator itersm = edge.partialEdgeQueries->begin(); itersm != edge.partialEdgeQueries->end(); itersm++)
            {
                // check if the query point is on the same edge with the car, if so, no need to compute monitored network, since such case is simple to verify
                EdgeID_t queryEdge = Data::points.at(itersm->first).edgeID;
                if(edge.edgeID == queryEdge)
                    continue;
                if(t==0 || itero->monitoredNetworkMap->find(itersm->first) == itero->monitoredNetworkMap->end())
                {
                    SegmentSet_t segments = itersm->second;
                    for(SegmentSet_t::const_iterator iters = segments.begin(); iters != segments.end(); iters++)
                        if(iters->first <= itero->curDis && iters->second >= itero->curDis)
                        {
                            computeMonitoredNetwork(itersm->first, *itero, edge, *iters);
                            break;
                        }
                }
            }

            // report for query point on the same edge
            for(PIDSet_t::const_iterator iterp = edge.fullEdgeQueries->begin(); iterp != edge.fullEdgeQueries->end(); iterp++)
            {
                EdgeID_t queryEdge = Data::points.at(*iterp).edgeID;
                if(edge.edgeID != queryEdge)
                    continue;
                if(verifyByLimitedDijkstra(itero->curDis, *iterp, edge))
                {
                    // count++;
                    // cout << (*itero) << " in " << (*iterp) << "'s "<< edge.edgeID << " f" << endl; // in
                }
                else
                    ; // out
            }
            for(PIDSegmentSetMap_t::const_iterator itersm = edge.partialEdgeQueries->begin(); itersm != edge.partialEdgeQueries->end(); itersm++)
            {
                EdgeID_t queryEdge = Data::points.at(itersm->first).edgeID;
                if(edge.edgeID != queryEdge)
                    continue;
                if(verifyByLimitedDijkstra(itero->curDis, itersm->first, edge))
                {
                    // count++;
                    // cout << (*itero) << " in " << (itersm->first) << "'s "<< edge.edgeID << " p" << endl; // in
                }
                else
                    ; // out
            }

            // report according to monitored network
            for(MonNetworkMap_t::iterator itermn = itero->monitoredNetworkMap->begin(); itermn != itero->monitoredNetworkMap->end(); itermn++)
            {
                Point &query = Data::points.at(itermn->first);
                if(itermn->second.preferredFacilities.size() + edge.points->size() < query.k)
                {
                    // count++;
                    // cout << (*itero) << " in " << (itermn->first) << "'s "<< edge.edgeID << " uv" << endl; // in
                }
                else
                {
                    if(verifyByMonitoredNetwork(itermn->first, itermn->second, itero->curDis))
                    {
                        // count++;
                        // cout << (*itero) << " in " << (itermn->first) << "'s "<< edge.edgeID << " v" << endl; // in
                    }
                    else
                        ; // out
                }
            }
        }
        stop = clock();
        totalClock += stop - start;
    }

    for(Index_t io = 0; io < objects.size(); io++)
        objects.at(io).clean();
    objectFile.close();

    // cout << count << " number of in"<< endl;
    cout << "monitor time : " << totalClock << endl;
    Argument::monitor = totalClock;
}

UnprunedNetwork*
SAC::computeUnprunedNetwork(Point &query)
{
    /// unpruned network
    NIDDisMap_t unMinDis; // ........... min distance from the query point to all nodes
    NIDMinHeap_t unQueue; // ........... priority queue / min heap
    NIDSet_t unNodes; // ............... visited (decided) nodes

    UnprunedNetwork* unprunedNetwork = new UnprunedNetwork();

    init(query, unprunedNetwork, unMinDis, unQueue);
    while(!unQueue.empty())
    {
        NodeID_t node = unQueue.begin()->second;
        Distance_t disToQ = unQueue.begin()->first;
        unQueue.erase(unQueue.begin());
        unNodes.insert(node);
        bool dead = !nDijkstra(query, node, disToQ);
        if(dead)
            continue;

        // for all the unvisited adjacent node
        EdgePV_t &epv = Data::adjMap[node];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            if(edge.edgeID == query.edgeID)
                continue;
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );
            if(unNodes.find(next) != unNodes.end())
                continue;
            if(edge.points->size() >= query.k)
            {
                Distance_t segStart = 0, segEnd = edge.length;
                if(node == edge.start)
                {
                    Index_t index = 1;
                    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++, index++)
                        if(index == query.k - 1)
                            segEnd = iterp->first;
                        else if(index == query.k)
                        {
                            segEnd = (segEnd + iterp->first) / 2;
                            if(iterp->first < disToQ)
                                dead = true;
                            break;
                        }
                }
                else
                {
                    Index_t index = 1;
                    for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++, index++)
                        if(index == query.k - 1)
                            segStart = iterp->first;
                        else if(index == query.k)
                        {
                            segStart = (segStart + iterp->first) / 2;
                            if(edge.length - iterp->first < disToQ)
                                dead = true;
                            break;
                        }
                }
                if(!dead)
                {
                    SegmentSet_t segments;
                    segments.insert(make_pair(segStart, segEnd));
                    unprunedNetwork->partialEdges->insert(make_pair(edge.edgeID, segments));
                }
            }
            else
                unprunedNetwork->fullEdges->insert(edge.edgeID);
        }

        if(dead)
            continue;

        // for each unvisited adjacent node
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            if(edge.edgeID == query.edgeID)
                continue;
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );
            if(unNodes.find(next) != unNodes.end())
                continue;
            if(edge.points->size() >= query.k)
                continue;
            if(unMinDis.find(next) == unMinDis.end() || unMinDis[node] + edge.length < unMinDis[next])
            {
                unQueue.erase(make_pair(unMinDis[next], next)); // for better path
                // update priority queue
                unMinDis[next] = unMinDis[node] + edge.length;
                unQueue.insert(make_pair(unMinDis[next], next));
            }
        }
    }

    for(NIDSet_t::const_iterator itern = unNodes.begin(); itern != unNodes.end(); itern++)
        unprunedNetwork->nodesDis->insert(make_pair(*itern, unMinDis[*itern]));

    return unprunedNetwork;
}

void
SAC::init(Point &query, UnprunedNetwork* unprunedNetwork, NIDDisMap_t &unMinDis, NIDMinHeap_t &unQueue)
{
    Edge &edge = Data::edges.at(query.edgeID);

    if(edge.points->size() <= query.k)
    {
        unMinDis[edge.start] = query.disToStart;
        unMinDis[edge.end] = edge.length - query.disToStart;

        unQueue.insert(make_pair(unMinDis[edge.start], edge.start));
        unQueue.insert(make_pair(unMinDis[edge.end], edge.end));

        unprunedNetwork->fullEdges->insert(edge.edgeID);
        return;
    }
    // else : there are at least K points share the same edge with the query point

    // find out where is the query point
    int index = 0, qindex = 0;
    DisPIDPairSet_t::const_iterator qiter;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++, index++)
        if(iterp->second == query.pointID)
        {
            qindex = index;
            qiter = iterp;
            break;
        }

    // check if the start point and end point of the query edge should be added into frontier of dijkstra's algorithm
    int first = qindex - query.k, last = qindex + query.k;
    bool full = true;
    if(first < 0)
    {
        unMinDis[edge.start] = query.disToStart;
        unQueue.insert(make_pair(unMinDis[edge.start], edge.start));
    }
    else
        full = false;
    if((unsigned) last >= edge.points->size())
    {
        unMinDis[edge.end] = edge.length - query.disToStart;
        unQueue.insert(make_pair(unMinDis[edge.end], edge.end));
    }
    else
        full = false;
    if(full)
    {
        unprunedNetwork->fullEdges->insert(query.edgeID);
        return;
    }

    // compute segment if there are at least K points at either side of the query point on the query edge
    index = 0;
    Distance_t segStart = 0, segEnd = edge.length;
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end() && index <= last; iterp++, index++)
        if(index < first)
            continue;
        else if(index == first)
            segStart = iterp->first;
        else if(index == first + 1 && index > 0)
            segStart = (segStart + iterp->first) / 2;
        else if(index == last - 1 && (unsigned) last < edge.points->size())
            segEnd = iterp->first;
        else if(index == last)
            segEnd = (segEnd + iterp->first) / 2;
    SegmentSet_t segments;
    segments.insert(make_pair(segStart, segEnd));
    unprunedNetwork->partialEdges->insert(make_pair(query.edgeID, segments));
}

bool
SAC::nDijkstra(Point &query, NodeID_t nid, Distance_t limit)
{
    /// encounter nodes
    NIDDisMap_t nMinDis; // ............ min distance from a node to all nodes
    NIDMinHeap_t nQueue; // ............ priority queue for a node
    PIDSet_t facilities; // ............ collect of encountered facilities
    NIDSet_t nNodes; // ................ visited (decided) nodes

    nQueue.insert(make_pair(0, nid));

    while(!nQueue.empty() && nQueue.begin()->first + Argument::EPS < limit)
    {
        if(facilities.size() >= query.k) // inaccurate as facilities could contain the query point
            return false;

        NodeID_t node = nQueue.begin()->second;
        Distance_t disToQ = nQueue.begin()->first;
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
                    if(disToQ + iterp->first + Argument::EPS < limit)
                        facilities.insert(iterp->second);
                }
            else
                for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++)
                {
                    if(disToQ + edge.length - iterp->first + Argument::EPS < limit)
                        facilities.insert(iterp->second);
                }
        }
    }
    if(facilities.size() >= query.k)
        return false;
    return true;
}

void
SAC::indexUnprunedNetwork(UnprunedNetwork* unprunedNetwork, Point &query)
{
    for(NIDDisMap_t::const_iterator iterm = unprunedNetwork->nodesDis->begin(); iterm != unprunedNetwork->nodesDis->end(); iterm++)
         query.addUnprunedNetworkNodeDis(iterm->first, iterm->second);

    for(EIDSet_t::const_iterator itere = unprunedNetwork->fullEdges->begin(); itere != unprunedNetwork->fullEdges->end(); itere++)
    {
        Edge &edge = Data::edges.at(*itere);
        edge.addFullEdge(query.pointID);
        Argument::zoneLength += edge.length;
    }

    for(EIDSegmentSetMap_t::const_iterator itere = unprunedNetwork->partialEdges->begin(); itere != unprunedNetwork->partialEdges->end(); itere++)
    {
        Edge &edge = Data::edges.at(itere->first);
        edge.addPartialEdge(query.pointID, itere->second);
        for(SegmentSet_t::const_iterator iters = itere->second.begin(); iters != itere->second.end(); iters++)
            Argument::zoneLength += iters->second - iters->first;
    }
}

void
SAC::computeMonitoredNetwork(PointID_t pid, Object& object, Edge& safeEdge, Segment_t safeRegion)
{
    /// monitored network
    NIDDisMap_t mnMinDis; // ........... min distance from the end of safeRegion to all nodes
    NIDMinHeap_t mnQueue; // ........... priority queue / min heap
    NIDSet_t mnNodes; // ............... visited (decided) nodes

    Point &query = Data::points.at(pid);
    MonitoredNetwork moniNetwork(safeEdge.edgeID, safeRegion);

    // limit : maximum possible distance from the query point to the object ( any where on the safeRegion )
    Distance_t safeRegionLen = safeRegion.second - safeRegion.first;
    Distance_t limit;
    if(query.unprunedNetworkNodesDis->find(safeEdge.start) != query.unprunedNetworkNodesDis->end() &&
       query.unprunedNetworkNodesDis->find(safeEdge.end) != query.unprunedNetworkNodesDis->end())
    {
        Distance_t disQToStart = (*query.unprunedNetworkNodesDis)[safeEdge.start];
        Distance_t disQToEnd = (*query.unprunedNetworkNodesDis)[safeEdge.end];
        if(disQToStart + safeEdge.length == disQToEnd || disQToEnd + safeEdge.length == disQToStart)
            limit = (disQToStart < disQToEnd ? disQToStart : disQToEnd) + safeRegionLen;
        else
        {
            Distance_t qMax = ( safeEdge.length + disQToStart + disQToEnd ) / 2;
            Distance_t qMaxLoc = qMax - disQToStart;
            if(qMaxLoc >= safeRegion.first && qMaxLoc <= safeRegion.second)
                limit = qMax;
            else
                limit = ( safeRegion.first == 0 ? disQToStart : disQToEnd) + safeRegionLen;
        }
    }
    else if(query.unprunedNetworkNodesDis->find(safeEdge.start) != query.unprunedNetworkNodesDis->end())
    {
        Distance_t disQToStart = (*query.unprunedNetworkNodesDis)[safeEdge.start];
        limit = disQToStart + safeRegion.second;
    }
    else if(query.unprunedNetworkNodesDis->find(safeEdge.end) != query.unprunedNetworkNodesDis->end())
    {
        Distance_t disQToEnd = (*query.unprunedNetworkNodesDis)[safeEdge.end];
        limit = disQToEnd + safeEdge.length - safeRegion.first;
    }
    else
    {
        cout << "SAC::computeMonitoredNetwork error" << endl;
        exit(EXIT_SUCCESS);
    }

    // Dijkstra : safeRegion.first as source
    mnMinDis.clear();
    mnQueue.clear();
    mnNodes.clear();

    mnQueue.insert(make_pair(safeRegion.first, safeEdge.start));
    while(!mnQueue.empty() && mnQueue.begin()->first < limit)
    {
        NodeID_t node = mnQueue.begin()->second;
        Distance_t disToSource = mnQueue.begin()->first;
        mnQueue.erase(mnQueue.begin());
        mnNodes.insert(node);

        // for each unvisited adjacent node
        EdgePV_t &epv = Data::adjMap[node];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);

            // for the query edge, no need to go further than the query point
            if(edge.edgeID == query.edgeID)
            {
                if(node == edge.start)
                {
                    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                        if(iterp->second != query.pointID)
                        {
                            if(iterp->first > query.disToStart)
                                break;
                            Distance_t disPToSource = disToSource + iterp->first;
                            if(disPToSource >= limit)
                                continue;
                            PointID_t point = iterp->second;
                            PointRegionPath key = make_pair(point, true);
                            if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                            {
                                moniNetwork.addPreferrence(point, true, disPToSource);
                                moniNetwork.addPreferredFacility(point);
                            }
                        }
                }
                else
                {
                    for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++)
                        if(iterp->second != query.pointID)
                        {
                            if(iterp->first < query.disToStart)
                                break;
                            Distance_t disPToSource = disToSource + edge.length - iterp->first;
                            if(disPToSource >= limit)
                                continue;
                            PointID_t point = iterp->second;
                            PointRegionPath key = make_pair(point, true);
                            if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                            {
                                moniNetwork.addPreferrence(point, true, disPToSource);
                                moniNetwork.addPreferredFacility(point);
                            }
                        }
                }
                continue;
            }

            // collect monitored facility points
            if(node == edge.start)
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                {
                    Distance_t disPToSource = disToSource + iterp->first;
                    if(disPToSource >= limit)
                        continue;
                    PointID_t point = iterp->second;
                    PointRegionPath key = make_pair(point, true);
                    if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                    {
                        moniNetwork.addPreferrence(point, true, disPToSource);
                        moniNetwork.addPreferredFacility(point);
                    }
                }
            else
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                {
                    Distance_t disPToSource = disToSource + edge.length - iterp->first;
                    if(disPToSource >= limit)
                        continue;
                    PointID_t point = iterp->second;
                    PointRegionPath key = make_pair(point, true);
                    if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                    {
                        moniNetwork.addPreferrence(point, true, disPToSource);
                        moniNetwork.addPreferredFacility(point);
                    }
                }

            // update Dijkstra's expanding info
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );
            if(mnNodes.find(next) != mnNodes.end())
                continue;
            if(mnMinDis.find(next) == mnMinDis.end() || mnMinDis[node] + edge.length < mnMinDis[next])
            {
                mnQueue.erase(make_pair(mnMinDis[next], next)); // for better path
                // update priority queue
                mnMinDis[next] = mnMinDis[node] + edge.length;
                mnQueue.insert(make_pair(mnMinDis[next], next));
            }
        }
    }

    // Dijkstra : sageRegion.second as source
    mnMinDis.clear();
    mnQueue.clear();
    mnNodes.clear();

    mnQueue.insert(make_pair(safeEdge.length - safeRegion.second, safeEdge.end));
    while(!mnQueue.empty() && mnQueue.begin()->first < limit)
    {
        NodeID_t node = mnQueue.begin()->second;
        Distance_t disToSource = mnQueue.begin()->first;
        mnQueue.erase(mnQueue.begin());
        mnNodes.insert(node);

        // for each unvisited adjacent node
        EdgePV_t &epv = Data::adjMap[node];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);

            // for the query edge, no need to go further than the query point
            if(edge.edgeID == query.edgeID)
            {
                if(node == edge.start)
                {
                    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                        if(iterp->second != query.pointID)
                        {
                            if(iterp->first > query.disToStart)
                                break;
                            Distance_t disPToSource = disToSource + iterp->first;
                            if(disPToSource >= limit)
                                continue;
                            PointID_t point = iterp->second;
                            PointRegionPath key = make_pair(point, false);
                            if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                            {
                                moniNetwork.addPreferrence(point, false, disPToSource);
                                moniNetwork.addPreferredFacility(point);
                            }
                        }
                }
                else
                {
                    for(DisPIDPairSet_t::const_reverse_iterator iterp = edge.points->rbegin(); iterp != edge.points->rend(); iterp++)
                        if(iterp->second != query.pointID)
                        {
                            if(iterp->first < query.disToStart)
                                break;
                            Distance_t disPToSource = disToSource + edge.length - iterp->first;
                            if(disPToSource >= limit)
                                continue;
                            PointID_t point = iterp->second;
                            PointRegionPath key = make_pair(point, false);
                            if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                            {
                                moniNetwork.addPreferrence(point, false, disPToSource);
                                moniNetwork.addPreferredFacility(point);
                            }
                        }
                }
                continue;
            }

            // collect monitored facility points
            if(node == edge.start)
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                {
                    Distance_t disPToSource = disToSource + iterp->first;
                    if(disPToSource >= limit)
                        continue;
                    PointID_t point = iterp->second;
                    PointRegionPath key = make_pair(point, false);
                    if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                    {
                        moniNetwork.addPreferrence(point, false, disPToSource);
                        moniNetwork.addPreferredFacility(point);
                    }
                }
            else
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                {
                    Distance_t disPToSource = disToSource + edge.length - iterp->first;
                    if(disPToSource >= limit)
                        continue;
                    PointID_t point = iterp->second;
                    PointRegionPath key = make_pair(point, false);
                    if(moniNetwork.preferrence.find(key) == moniNetwork.preferrence.end() || moniNetwork.preferrence[key] > disPToSource + Argument::EPS )
                    {
                        moniNetwork.addPreferrence(point, false, disPToSource);
                        moniNetwork.addPreferredFacility(point);
                    }
                }

            // update Dijkstra's expanding info
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );
            if(mnNodes.find(next) != mnNodes.end())
                continue;
            if(mnMinDis.find(next) == mnMinDis.end() || mnMinDis[node] + edge.length < mnMinDis[next])
            {
                mnQueue.erase(make_pair(mnMinDis[next], next)); // for better path
                // update priority queue
                mnMinDis[next] = mnMinDis[node] + edge.length;
                mnQueue.insert(make_pair(mnMinDis[next], next));
            }
        }
    }

    // associate computed monitored network to the object and the query point
    object.addMonitoredNetwork(pid, moniNetwork);
}

bool
SAC::verifyByLimitedDijkstra(Distance_t locationO, PointID_t pid, Edge& objectEdge)
{
    /// verify by limited dijkstra
    NIDDisMap_t vMinDis; // ............ min distance from the object to all nodes
    NIDMinHeap_t vQueue; // ............ priority queue / min heap
    NIDSet_t vNodes; // ................ visited (decided) nodes
    PIDSet_t closerFacilities; // ...... collect of facilities closer than the query point

    Point &query = Data::points.at(pid);
    Distance_t locationQ = query.disToStart;
    Distance_t limit = ( locationO > locationQ ? locationO - locationQ : locationQ - locationO );

    // collect closer facilities on the same edge with the object
    for(DisPIDPairSet_t::const_iterator iterp = objectEdge.points->begin(); iterp != objectEdge.points->end(); iterp++)
    {
        if(iterp->second == pid)
            continue;
        if(iterp->first <= locationO)
        {
            if(locationO - iterp->first < limit){
                closerFacilities.insert(iterp->second);
                //cout << "add : " << iterp->first << endl;
            }
        }
        else if(iterp->first - locationO < limit){
            closerFacilities.insert(iterp->second);
                //cout << "add : " << iterp->first << endl;
        }
    }

    // init min heap for Dijkstra's algorithm
    if(locationO < limit)
    {
        vQueue.insert(make_pair(locationO, objectEdge.start));
        vMinDis[objectEdge.start] = locationO;
        //cout << "init edge start" << endl;
    }
    if(objectEdge.length - locationO < limit)
    {
        vQueue.insert(make_pair(objectEdge.length - locationO, objectEdge.end));
        vMinDis[objectEdge.end] = objectEdge.length - locationO;
        //cout << "init edge end" << endl;
    }

    // Dijkstra's algorithm
    while(!vQueue.empty() && vQueue.begin()->first < limit)
    {
        if(closerFacilities.size() >= query.k)
            return false;
        NodeID_t node = vQueue.begin()->second;
        Distance_t disToO = vQueue.begin()->first;
        vQueue.erase(vQueue.begin());
        vNodes.insert(node);

        // for each unvisited adjacent node
        EdgePV_t &epv = Data::adjMap[node];
        for(Index_t ie=0; ie<epv.size(); ie++)
        {
            Edge &edge = *epv.at(ie);
            if(edge.edgeID == objectEdge.edgeID)
                continue;

            // collect closer facilities
            if(node == edge.start)
            {
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                    if(disToO + iterp->first < limit)
                        closerFacilities.insert(iterp->second);
            }
            else
            {
                for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
                    if(disToO + edge.length - iterp->first < limit)
                        closerFacilities.insert(iterp->second);
            }

            // update Dijkstra's expanding info
            NodeID_t next = ( node==edge.start ? edge.end : edge.start );
            if(vNodes.find(next) != vNodes.end())
                continue;
            if(vMinDis.find(next) == vMinDis.end() || vMinDis[node] + edge.length < vMinDis[next])
            {
                vQueue.erase(make_pair(vMinDis[next], next)); // for better path
                // update priority queue
                vMinDis[next] = vMinDis[node] + edge.length;
                vQueue.insert(make_pair(vMinDis[next], next));
            }
        }
    }

    if(closerFacilities.size() >= query.k)
        return false;
    return true;
}

bool
SAC::verifyByMonitoredNetwork(PointID_t pid, MonitoredNetwork &monNetwork, Distance_t locationO)
{
    Edge &edge = Data::edges.at(monNetwork.validOnEdge);
    Point &query = Data::points.at(pid);
    Distance_t disQToOViaStart = numeric_limits<double>::infinity();
    Distance_t disQToOViaEnd = numeric_limits<double>::infinity();
    if(query.unprunedNetworkNodesDis->find(edge.start) != query.unprunedNetworkNodesDis->end())
        disQToOViaStart = (*query.unprunedNetworkNodesDis)[edge.start] + locationO;
    if(query.unprunedNetworkNodesDis->find(edge.end) != query.unprunedNetworkNodesDis->end())
        disQToOViaEnd = (*query.unprunedNetworkNodesDis)[edge.end] + edge.length - locationO;
    if(disQToOViaStart == numeric_limits<double>::infinity() && disQToOViaEnd == numeric_limits<double>::infinity())
    {
        cout << "SAC::verifyByMonitoredNetwork() disQToOViaStart && disQToOViaEnd error" << endl;
        exit(EXIT_SUCCESS);
    }
    Distance_t disQToO = ( disQToOViaStart < disQToOViaEnd ? disQToOViaStart : disQToOViaEnd );
    PIDSet_t closerFacilities; // ...... collect of facilities closer than the query point

    // check facilities on the edge
    for(DisPIDPairSet_t::const_iterator iterp = edge.points->begin(); iterp != edge.points->end(); iterp++)
        if(iterp->first <= locationO)
        {
            if(locationO - iterp->first + Argument::EPS < disQToO)
                closerFacilities.insert(iterp->second);
        }
        else if(iterp->first - locationO + Argument::EPS < disQToO)
            closerFacilities.insert(iterp->second);

    // check facilities in the monitored network
    for(PIDSet_t::const_iterator iterp = monNetwork.preferredFacilities.begin(); iterp != monNetwork.preferredFacilities.end(); iterp++)
    {
        PointID_t keyFacility = *iterp;

        Distance_t disToSegStart = numeric_limits<double>::infinity();
        Distance_t disToSegEnd = numeric_limits<double>::infinity();
        PointRegionPath key = make_pair(keyFacility, true);
        if(monNetwork.preferrence.find(key) != monNetwork.preferrence.end())
            disToSegStart = monNetwork.preferrence[key];
        key = make_pair(keyFacility, false);
        if(monNetwork.preferrence.find(key) != monNetwork.preferrence.end())
            disToSegEnd = monNetwork.preferrence[key];

        Distance_t disToOViaSegStart = numeric_limits<double>::infinity();
        Distance_t disToOViaSegEnd = numeric_limits<double>::infinity();
        if(disToSegStart != numeric_limits<double>::infinity())
            disToOViaSegStart = disToSegStart + locationO - monNetwork.validOnSegment.first;
        if(disToSegEnd != numeric_limits<double>::infinity())
            disToOViaSegEnd = disToSegEnd + monNetwork.validOnSegment.second - locationO;

        if(disToOViaSegStart + Argument::EPS < disQToO || disToOViaSegEnd + Argument::EPS < disQToO)
            closerFacilities.insert(keyFacility);
    }

    if(closerFacilities.size() < query.k)
        return true;
    return false;
}

