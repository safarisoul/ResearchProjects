#include "Update.h"

Update::Update()
{
    srand((unsigned)time(0));
}

void
Update::zoneForAll()
{
    // cout << "Update calculateAndIndexZoneForAll..." << endl;
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
        kQueries.insert(queryPointID);
        Point &query = Data::points.at(queryPointID);
        query.k = k;
        Argument::queryPointID = queryPointID;

        InfluenceZone* influenceZone;

        UpdatableZone zone;

        start = clock();
        influenceZone = zone.computeZone(queryPointID);
        stop = clock();
        totalCalculation += stop - start;

        zone.indexZone(influenceZone, queryPointID);

        influenceZone->clean();
        if(influenceZone)
            delete influenceZone;
    }

    queryFile.close();

    cout << "calculation time : " << totalCalculation << endl;
    Argument::calculation = totalCalculation;
}

void
Update::update()
{

    for(Index_t index = 0; index < Data::numFacilities; index++)
        if(kQueries.find(Data::facilities.at(index)->pointID) == kQueries.end())
            updates.push_back(Data::facilities.at(index)->pointID);
    while(updates.size() > Argument::numUpdates)
    {
        Index_t index = getRandom(updates.size());
        updates.erase(updates.begin() + index);
    }

    UpdatableZone zone;

    clock_t start, stop;
    INT64 totalClock = 0;

    start = clock();

    /// deletion
    for(Index_t iu = 0; iu < updates.size(); iu++)
    {
        PointID_t pid = updates.at(iu);
        Point &facility = Data::points.at(pid);
        Edge &edge = Data::edges.at(facility.edgeID);
        edge.closePoint(pid);

        BIDSet_t boundaryIDs;
        PIDSet_t queries;

        for(BIDSet_t::const_iterator iterb = edge.fullEdgeBoundaries->begin(); iterb != edge.fullEdgeBoundaries->end(); iterb++)
        {
            boundaryIDs.insert(*iterb);
            queries.insert(iterb->query);
        }

        for(BIDSegmentSetMap_t::const_iterator itersm = edge.partialEdgeBoundaries->begin(); itersm != edge.partialEdgeBoundaries->end(); itersm++)
            for(SegmentSet_t::const_iterator iters = itersm->second.begin(); iters != itersm->second.end(); iters++)
                if(iters->first <= facility.disToStart && iters->second >= facility.disToStart)
                {
                    boundaryIDs.insert(itersm->first);
                    queries.insert(itersm->first.query);
                }

        for(PIDSet_t::const_iterator iterp = queries.begin(); iterp != queries.end(); iterp++)
        {
            BIDSet_t expiredBoundaries;
            for(BIDSet_t::const_iterator iterb = boundaryIDs.begin(); iterb != boundaryIDs.end(); iterb++)
                if(iterb->query == *iterp)
                    expiredBoundaries.insert(*iterb);

            // deletion
            Argument::updateCounts++;
            zone.enlarge(*iterp, expiredBoundaries);

            // clean up
            for(BIDSet_t::const_iterator iterb = expiredBoundaries.begin(); iterb != expiredBoundaries.end(); iterb++)
                zone.removeBoundaryZone(*iterb);
        }
    }

    /// insertion
    for(Index_t iu = 0; iu < updates.size(); iu++)
    {
        PointID_t pid = updates.at(iu);
        Point &facility = Data::points.at(pid);
        Edge &edge = Data::edges.at(facility.edgeID);
        edge.openPoint(pid);

        BIDSet_t boundaryIDs;
        PIDSet_t queries;

        for(BIDSet_t::const_iterator iterb = edge.fullEdgeBoundaries->begin(); iterb != edge.fullEdgeBoundaries->end(); iterb++)
        {
            boundaryIDs.insert(*iterb);
            queries.insert(iterb->query);
        }

        for(BIDSegmentSetMap_t::const_iterator itersm = edge.partialEdgeBoundaries->begin(); itersm != edge.partialEdgeBoundaries->end(); itersm++)
            for(SegmentSet_t::const_iterator iters = itersm->second.begin(); iters != itersm->second.end(); iters++)
                if(iters->first <= facility.disToStart && iters->second >= facility.disToStart)
                {
                    boundaryIDs.insert(itersm->first);
                    queries.insert(itersm->first.query);
                }

        for(PIDSet_t::const_iterator iterp = queries.begin(); iterp != queries.end(); iterp++)
        {
            BIDSet_t expiredBoundaries;
            for(BIDSet_t::const_iterator iterb = boundaryIDs.begin(); iterb != boundaryIDs.end(); iterb++)
                if(iterb->query == *iterp)
                    expiredBoundaries.insert(*iterb);

            // insertion
            Argument::updateCounts++;
            zone.shrink(*iterp, expiredBoundaries);

            // clean up
            for(BIDSet_t::const_iterator iterb = expiredBoundaries.begin(); iterb != expiredBoundaries.end(); iterb++)
                zone.removeBoundaryZone(*iterb);
        }
    }

    stop = clock();
    totalClock = stop - start;

    cout << "update time : " << totalClock << endl;
    cout << "update counts : " << Argument::updateCounts << endl;
    Argument::update = totalClock;
}

void
Update::updateSimple()
{

    for(Index_t index = 0; index < Data::numFacilities; index++)
        if(kQueries.find(Data::facilities.at(index)->pointID) == kQueries.end())
            updates.push_back(Data::facilities.at(index)->pointID);
    while(updates.size() > Argument::numUpdates)
    {
        Index_t index = getRandom(updates.size());
        updates.erase(updates.begin() + index);
    }

    UpdatableZone zone;

    clock_t start, stop;
    INT64 totalClock = 0;

    start = clock();

    /// deletion
    for(Index_t iu = 0; iu < updates.size(); iu++)
    {
        PointID_t pid = updates.at(iu);
        Point &facility = Data::points.at(pid);
        Edge &edge = Data::edges.at(facility.edgeID);
        edge.closePoint(pid);

        BIDSet_t boundaryIDs;
        PIDSet_t queries;

        for(BIDSet_t::const_iterator iterb = edge.fullEdgeBoundaries->begin(); iterb != edge.fullEdgeBoundaries->end(); iterb++)
        {
            boundaryIDs.insert(*iterb);
            queries.insert(iterb->query);
        }

        for(BIDSegmentSetMap_t::const_iterator itersm = edge.partialEdgeBoundaries->begin(); itersm != edge.partialEdgeBoundaries->end(); itersm++)
            for(SegmentSet_t::const_iterator iters = itersm->second.begin(); iters != itersm->second.end(); iters++)
                if(iters->first <= facility.disToStart && iters->second >= facility.disToStart)
                {
                    boundaryIDs.insert(itersm->first);
                    queries.insert(itersm->first.query);
                }

        for(PIDSet_t::const_iterator iterp = queries.begin(); iterp != queries.end(); iterp++)
        {
            Argument::updateCounts++;

            // clean up
            zone.removeZone(*iterp);

            // recalculate
            InfluenceZone* influenceZone;
            influenceZone = zone.computeZone(*iterp);
            zone.indexZone(influenceZone, *iterp);
            influenceZone->clean();
            if(influenceZone)
                delete influenceZone;
        }
    }

    /// insertion
    for(Index_t iu = 0; iu < updates.size(); iu++)
    {
        PointID_t pid = updates.at(iu);
        Point &facility = Data::points.at(pid);
        Edge &edge = Data::edges.at(facility.edgeID);
        edge.openPoint(pid);

        BIDSet_t boundaryIDs;
        PIDSet_t queries;

        for(BIDSet_t::const_iterator iterb = edge.fullEdgeBoundaries->begin(); iterb != edge.fullEdgeBoundaries->end(); iterb++)
        {
            boundaryIDs.insert(*iterb);
            queries.insert(iterb->query);
        }

        for(BIDSegmentSetMap_t::const_iterator itersm = edge.partialEdgeBoundaries->begin(); itersm != edge.partialEdgeBoundaries->end(); itersm++)
            for(SegmentSet_t::const_iterator iters = itersm->second.begin(); iters != itersm->second.end(); iters++)
                if(iters->first <= facility.disToStart && iters->second >= facility.disToStart)
                {
                    boundaryIDs.insert(itersm->first);
                    queries.insert(itersm->first.query);
                }

        for(PIDSet_t::const_iterator iterp = queries.begin(); iterp != queries.end(); iterp++)
        {
            Argument::updateCounts++;

            // clean up
            zone.removeZone(*iterp);

            // recalculate
            InfluenceZone* influenceZone;
            influenceZone = zone.computeZone(*iterp);
            zone.indexZone(influenceZone, *iterp);
            influenceZone->clean();
            if(influenceZone)
                delete influenceZone;
        }
    }

    stop = clock();
    totalClock = stop - start;

    cout << "update time : " << totalClock << endl;
    cout << "update counts : " << Argument::updateCounts << endl;
    Argument::update = totalClock;
}

void
Update::monitor()
{
    // cout << "Update monitor..." << endl;
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
                cout << "Update::monitor() objectID error" << endl;
                cout << objects.at(oi);
                exit(EXIT_SUCCESS);
            }
        }

        start = clock();
        // check if they are in the influence zone or not
        for(ObjectV_t::const_iterator itero = objects.begin(); itero != objects.end(); itero++)
        {
            Edge &edge = Data::edges.at(itero->curEdgeID);

            for(PIDSet_t::const_iterator iterp = edge.fullEdgeQueries->begin(); iterp != edge.fullEdgeQueries->end(); iterp++)
            {
                // count++;
                // cout << (*itero) << " in " << (*iterp) << "'s "<< edge.edgeID << " f" << endl; // *iterp is the query point id that has the current object in its influence zone
            }
            for(PIDSegmentSetMap_t::const_iterator itersm = edge.partialEdgeQueries->begin(); itersm != edge.partialEdgeQueries->end(); itersm++)
                for(SegmentSet_t::const_iterator iters = itersm->second.begin(); iters != itersm->second.end(); iters++)
                    if(iters->first <= itero->curDis && iters->second >= itero->curDis)
                    {
                        // count++;
                        // cout << (*itero) << " in " << (itersm->first) << "'s "<< edge.edgeID << " p" << endl; // itersm->first is the query point id that has the current object in its influence zone
                        break;
                    }
        }
        stop = clock();
        totalClock += stop - start;
    }

    for(Index_t io = 0; io < objects.size(); io++)
        objects.at(io).clean();
    objectFile.close();

    // cout << count << " number of in" << endl;
    cout << "monitor time : " << totalClock << endl;
    Argument::monitor = totalClock;
}

