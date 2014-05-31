#include "Generator.h"

/**********************************************************************
 * QueryGenerator
 **********************************************************************/

QueryGenerator::QueryGenerator()
{
    srand((unsigned)time(0));
}

void
QueryGenerator::run()
{
    cout << "QueryGenerator running..." << endl;

    queryFileName = Argument::getQueryFileName();
    queryFile.open(queryFileName.c_str());

    if(queryFile.good())
    {
        Amount_t count = 0;
        for(Index_t index = 0; index < Data::numFacilities; index++)
        {
            if(Data::numFacilities - index <= Argument::numQueries - count)
            {
                queryFile << index << " " << Data::facilities.at(index)->pointID << " " << Argument::k << endl;
                count++;
            }
            else if(getRandom() < ( Argument::numQueries - count * 1.0 ) / ( Data::numFacilities - index ) )
            {
                queryFile << index << " " << Data::facilities.at(index)->pointID << " " << Argument::k << endl;
                count++;
            }
        }
        queryFile.close();
    }
    else
    {
        cout << "queryFile error" << endl;
        exit(EXIT_SUCCESS);
    }
}

/**********************************************************************
 * ObjectGenerator
 **********************************************************************/

ObjectGenerator::ObjectGenerator()
: total(0), min(numeric_limits<double>::infinity()), max(0), avg(0)
{
    srand((unsigned)time(0));

    for(Index_t i=0; i<Data::numEdges; i++)
    {
        Edge &edge = Data::edges.at(i);
        if(edge.length < min)
            min = edge.length;
        else if(edge.length > max)
            max = edge.length;
        total += edge.length;
    }
    avg = total / Data::numEdges;
}

void
ObjectGenerator::run()
{
    cout << "ObjectGenerator running..." << endl;

    objectFileName = Argument::getObjectFileName();
    objectFile.open(objectFileName.c_str());

    objectFile << "0" << endl;
    for(Index_t i=0; i<Argument::numObjects * Argument::movePercent / 100; i++)
    {
        Object object;
        object.objectID = i;
        do
        {
            EdgeID_t eid = getRandom(Data::numEdges);
            Edge &edge = Data::edges.at(eid);
            double passmark = edge.length / max ;
            double score = getRandom();
            if(score >= passmark)
            {
                object.curEdgeID = eid;
                object.curDis = edge.length * getRandom();
                // if(getRandom(100) + 1 > (int) Argument::movePercent)
                    // object.speed = 0;
                // else
                object.speed = Argument::speed;
            }
        }while(object.curEdgeID == BAD_ID);

        objects.push_back(object);
        objectFile << objects.at(i);
    }

    for(Index_t t=1; t<Argument::timeStamp; t++)
    {
        objectFile << t << endl;
        // for each object
        for(Index_t i=0; i<Argument::numObjects * Argument::movePercent / 100; i++)
        {
            Object &object = objects.at(i);
            object.preEdgeID = object.curEdgeID;
            object.preDis = object.curDis;
            // if not static, update location
            if(object.speed != 0)
            {
                Distance_t speed = object.speed;
                Edge &edge = Data::edges.at(object.curEdgeID);
                Distance_t dis = object.curDis + speed;
                while(dis<0 || dis>edge.length)
                {
                    if(dis < 0)
                    {
                        EdgePV_t &epv = Data::adjMap[edge.start];
                        for(Index_t i=0; i<epv.size(); i++)
                        {
                            // decide if this edge will be the next edge
                            double chance = (i + 1) * 1.0 / epv.size();
                            if(getRandom() > chance)
                                continue;
                            // update object to the next edge
                            Edge &nextEdge = *epv.at(i);
                            if(nextEdge.start == edge.start)
                            {
                                dis = -dis;
                                speed = -speed;
                            }
                            else
                                dis = edge.length + dis;
                            edge = nextEdge;
                            // next edge found, break;
                            break;
                        }
                    }
                    else
                    {
                        EdgePV_t &epv = Data::adjMap[edge.start];
                        for(Index_t i=0; i<epv.size(); i++)
                        {
                            // decide if this edge will be the next edge
                            double chance = (i + 1) * 1.0 / epv.size();
                            if(getRandom() > chance)
                                continue;
                            // update object to the next edge
                            Edge &nextEdge = *epv.at(i);
                            if(nextEdge.start == edge.end)
                                dis -= edge.length;
                            else
                            {
                                dis -= edge.length;
                                dis = nextEdge.length - dis;
                                speed = -speed;
                            }
                            edge = nextEdge;
                            // next edge found, break;
                            break;
                        }
                    }
                }
                object.speed = speed;
                object.curEdgeID = edge.edgeID;
                object.curDis = dis;
            }
            objectFile << object;
        }
    }

    objectFile.close();
}
