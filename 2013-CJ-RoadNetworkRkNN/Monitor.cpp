#include "Monitor.h"

Monitor::Monitor()
{
    srand((unsigned)time(0));
}

void
Monitor::zoneForAll()
{
    // cout << "Monitor calculateAndIndexZoneForAll..." << endl;
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

        Zone zone;

        start = clock();
        influenceZone = zone.computeZone(queryPointID);
        stop = clock();
        totalCalculation += stop - start;

        // zone.outputZoneNetwork(influenceZone);
        zone.indexZone(influenceZone, queryPointID);

        influenceZone->clean();
        if(influenceZone)
            delete influenceZone;
    }

    queryFile.close();

    // cout << "calculation time : " << totalCalculation << endl;
    Argument::calculation = totalCalculation;
}

void
Monitor::monitor()
{
    // cout << "Monitor monitor..." << endl;
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
                cout << "Monitor::monitor() objectID error" << endl;
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

void
Monitor::update()
{

    for(Index_t index = 0; index < Data::numFacilities; index++)
        if(kQueries.find(Data::facilities.at(index)->pointID) == kQueries.end())
            updates.push_back(Data::facilities.at(index)->pointID);
    while(updates.size() > Argument::numUpdates)
    {
        Index_t index = getRandom(updates.size());
        updates.erase(updates.begin() + index);
    }

    Zone zone;
    Argument::update = 0;

    /// deletion
    for(Index_t iu = 0; iu < updates.size(); iu++)
    {
        PointID_t pid = updates.at(iu);
        Point &facility = Data::points.at(pid);
        Edge &edge = Data::edges.at(facility.edgeID);

        edge.closePoint(pid);

        zoneForAll();
        Argument::update += Argument::calculation;
    }

    /// insertion
    for(Index_t iu = 0; iu < updates.size(); iu++)
    {
        PointID_t pid = updates.at(iu);
        Point &facility = Data::points.at(pid);
        Edge &edge = Data::edges.at(facility.edgeID);

        /// insertion

        edge.openPoint(pid);

        zoneForAll();
        Argument::update += Argument::calculation;
    }

    cout << "update time : " << Argument::update << endl;
}
