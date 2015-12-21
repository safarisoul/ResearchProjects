#include "DCCR.h"

void DCCR::rkfn(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult)
{
    struct timeval start, end;
    Vertex q;
    q.x = query.coord[0];
    q.y = query.coord[1];

    RStarTree band;

    gettimeofday(&start, NULL);
    bool good = BBS::circularkSkyband(band, fTree, NORMALIZE_SCALE, k, query);
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    Argument::Tbbs += seconds * 1e6 + useconds;

    if(!good)
    {
        Argument::Sdccr++;
        return;
    }

    KDepthContour kdc;

    gettimeofday(&start, NULL);
    SkyRider::kDepthContour(band, kdc, k);
    gettimeofday(&end, NULL);

    seconds = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    Argument::Tskyrider += seconds * 1e6 + useconds;

    //kdc.print();

    CandidateRegion cr;

    gettimeofday(&start, NULL);
    candidateRegion(fTree, band, q, k, kdc, cr);
    gettimeofday(&end, NULL);

    seconds = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    Argument::Tcr += seconds * 1e6 + useconds;

    //cr.print();

    if(cr.phi)
    {
        Argument::Sdccr++;
        return;
    }

    if(k == 1)
    {
        gettimeofday(&start, NULL);
        filterVerify(cTree, cr, result, notResult);
        gettimeofday(&end, NULL);

        seconds = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        Argument::Tfv += seconds * 1e6 + useconds;
    }
    else
    {
        RStarTree skirt;
        RadiansPoint_V skirt_v;

        gettimeofday(&start, NULL);
        outerSkirt(band, skirt, skirt_v, kdc, q);
        gettimeofday(&end, NULL);

        seconds = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        Argument::Tsk += seconds * 1e6 + useconds;

        Argument::NFband += band.root->aggregate;
        Argument::NFskirt += skirt.root->aggregate;

        RadiansRange_V crr;

        gettimeofday(&start, NULL);
        candidateRange(crr, skirt_v, k, q);
        gettimeofday(&end, NULL);

        seconds = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        Argument::Tcrr += seconds * 1e6 + useconds;

        gettimeofday(&start, NULL);
        filterVerify(skirt, cTree, query, q, k, cr, result, notResult, crr);
        gettimeofday(&end, NULL);

        seconds = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        Argument::Tfv += seconds * 1e6 + useconds;
    }
}

void DCCR::vara(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult)
{
    Vertex q;
    q.x = query.coord[0];
    q.y = query.coord[1];

    RStarTree band;
    bool good = BBS::circularkSkyband(band, fTree, NORMALIZE_SCALE, k, query);
    if(!good)
        return;

    KDepthContour kdc;
    SkyRider::kDepthContour(band, kdc, k);

    RStarTree skirt;
    RadiansPoint_V skirt_v;
    outerSkirt(band, skirt, skirt_v, kdc, q);

    RadiansRange_V crr;
    candidateRange(crr, skirt_v, k, q);

    Coord mid = NORMALIZE_SCALE / 2;
    size_t cntAR = 0, total = 1000000;
    // percentage of area inside candidate range : cntAR / total
    for(size_t iq = 0; iq < total; iq++)
    {
        while(true)
        {
            Coord point[DIM];
            Coord diff = 0, dis = 0;
            for(size_t dim = 0; dim < DIM; dim++)
            {
                point[dim] = 1e-3 + Util2D::getRandom() * (NORMALIZE_SCALE - 1);
                diff = point[dim] - mid;
                dis += diff * diff;
            }
            if(sqrt(dis) > mid)
                continue;
            Vertex p;
            p.x = point[0];
            p.y = point[1];
            if(contain(crr, p, q) == CONTAIN_COMPLETE)
                cntAR++;
            break;
        }
    }

    double v = (2 * Argument::k * M_PI) / Argument::numFacilities;
    double theta1 = 0, theta2 = M_PI;
    while(theta2 - theta1 > 1e-6)
    {
        double mid = 0.5 * (theta1 + theta2);
        double value = mid - sin(mid);
        if(value < v)
            theta1 = mid;
        else
            theta2 = mid;
    }
    double theta = 0.5 * (theta1 + theta2);
    double htheta = theta / 2;
    double rprime = cos(htheta);
    Vertex center;
    center.x = 1;
    center.y = 1;
    Vertex p;
    p.x = q.x / NORMALIZE_SCALE * 2;
    p.y = q.y / NORMALIZE_SCALE * 2;
    double d = sqrt(Util2D::distance2(center, p));

    double alpha = 2 * acos(rprime/d);
    double beta = alpha;
    double gamma = acos(d*sin(beta/2));
    double mu = Util2D::H_PI - beta/2;
    double eta = gamma + mu;
    double delta = Util2D::T_PI - 2 * eta;

    double base = 2 * sin(delta/2);
    double h = d + cos(delta/2);
    double areaseg = 0.5 * (delta - sin(delta));
    double areatri = 0.5 * base * h;

    double area = areaseg + areatri;
    double totalarea = M_PI;

    if(!isnan(area/totalarea))
    {
        Argument::cntCM++;
        Argument::cumuAR += ((double)cntAR/total);
        Argument::expARperR += area/totalarea;
    }
}

void DCCR::tpru(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult)
{
    Vertex q;
    q.x = query.coord[0];
    q.y = query.coord[1];

    RStarTree band;
    bool good = BBS::circularkSkyband(band, fTree, NORMALIZE_SCALE, k, query);
    if(!good)
        return;

    KDepthContour kdc;
    SkyRider::kDepthContour(band, kdc, k);

    CandidateRegion cr;
    candidateRegion(fTree, band, q, k, kdc, cr);

    if(cr.phi)
        return;

    RStarTree skirt;
    RadiansPoint_V skirt_v;
    outerSkirt(band, skirt, skirt_v, kdc, q);

    RadiansRange_V crr;
    candidateRange(crr, skirt_v, k, q);

    Argument::cntP++;
    tpruFVCR(skirt, cTree, query, q, k, cr, result, notResult, crr);
    tpruFVCRR(skirt, cTree, query, q, k, cr, result, notResult, crr);
    tpruFVB(skirt, cTree, query, q, k, cr, result, notResult, crr);
    tpruFVS(skirt, cTree, query, q, k, cr, result, notResult, crr);
}

// *****************************************************************************
//  candidate region
// *****************************************************************************

void DCCR::candidateRegion(RStarTree& fTree, RStarTree& band, Vertex& q, size_t k, KDepthContour& kdc, CandidateRegion& cr)
{
    if(kdc.upperHullVertex.size() == 0)
        return;
    else if(enclose(kdc, q))
    {
        cr.phi = true;
        return;
    }

    Tangent tangents;
    tangent(kdc, q, tangents);

    HalfSpace hSpace1, hSpace2;
    HalfSpace::halfSpace(tangents.tPoint1, q, hSpace1);
    HalfSpace::halfSpace(tangents.tPoint2, q, hSpace2);

    cr.init();
    cr.prune(hSpace1);
    cr.prune(hSpace2);
    if(cr.phi)
        return;

    Vertex_V pruners;
    initPruners(kdc, tangents, q, pruners);
    updatePruners(cr, pruners, q);

    while(pruners.size() > 0)
    {
        HalfSpace hSpace;
        HalfSpace::halfSpace(pruners.at(0), q, hSpace);
        pruners.erase(pruners.begin());

        if(cr.prune(hSpace))
            updatePruners(cr, pruners, q);
    }
}

bool DCCR::enclose(KDepthContour& kdc, Vertex& q)
{
    for(size_t il = 0; il < kdc.upperHullLine.size(); il++)
        if(Util2D::isAboveLine(kdc.upperHullLine.at(il), q) == Util2D::STRICT_ABOVE) // q is above upper hull
            return false;
    for(size_t il = 0; il < kdc.lowerHullLine.size(); il++)
        if(Util2D::isAboveLine(kdc.lowerHullLine.at(il), q) == Util2D::STRICT_BELOW) // q is below lower hull
            return false;
    return true;
}

void DCCR::tangent(KDepthContour& kdc, Vertex& q, Tangent& tangent)
{
    Line line;
    size_t n = 0;int cnt = 0;

    Util2D::linePass(kdc.left, q, line);
    if(verifyTangent(line, kdc.lowerHullVertex.at(kdc.lowerHullVertex.size() - 2), kdc.upperHullVertex.at(1)))
    {
        cnt++;
        addTangent(tangent, kdc.left, line, n);
    }

    Util2D::linePass(kdc.right, q, line);
    if(verifyTangent(line, kdc.upperHullVertex.at(kdc.upperHullVertex.size() - 2), kdc.lowerHullVertex.at(1)))
    {
        cnt++;
        addTangent(tangent, kdc.right, line, n);
    }

    for(size_t iv = 1; iv < kdc.upperHullVertex.size() - 1; iv++)
    {
        Util2D::linePass(kdc.upperHullVertex.at(iv), q, line);
        if(verifyTangent(line, kdc.upperHullVertex.at(iv - 1), kdc.upperHullVertex.at(iv + 1)))
        {
            cnt++;
            addTangent(tangent, kdc.upperHullVertex.at(iv), line, n);
        }
    }

    for(size_t iv = 1; iv < kdc.lowerHullVertex.size() - 1; iv++)
    {
        Util2D::linePass(kdc.lowerHullVertex.at(iv), q, line);
        if(verifyTangent(line, kdc.lowerHullVertex.at(iv - 1), kdc.lowerHullVertex.at(iv + 1)))
        {
            cnt++;
            addTangent(tangent, kdc.lowerHullVertex.at(iv), line, n);
        }
    }

    if(n < 2)
    {
        addTangent(tangent, kdc.upperHullVertex.at(0), line, n);
        addTangent(tangent, kdc.lowerHullVertex.at(0), line, n);
    }
    Util2D::linePass(tangent.tPoint1, tangent.tPoint2, tangent.connection);
}

void DCCR::initPruners(KDepthContour& kdc, Tangent& tangent, Vertex& q, Vertex_V& pruners)
{
    for(size_t iv = 1; iv < kdc.upperHullVertex.size(); iv++)
        pruners.push_back(kdc.upperHullVertex.at(iv));
    for(size_t iv = 1; iv < kdc.lowerHullVertex.size(); iv++)
        pruners.push_back(kdc.lowerHullVertex.at(iv));
}

void DCCR::updatePruners(CandidateRegion& cr, Vertex_V& pruners, Vertex& q)
{
    Vertex_S vs;
    cr.getVertex(vs);
    Vertex_V newPruners;
    for(size_t ip = 0; ip < pruners.size(); ip++)
    {
        bool good = false;
        for(Vertex_S::iterator itvs=vs.begin(); itvs!=vs.end(); itvs++)
        {
            Vertex v = *itvs;
            Coord dis2vq = Util2D::distance2(v, q);
            Coord dis2vp = Util2D::distance2(pruners.at(ip), v);
            if(dis2vp > dis2vq + EPS)
            {
                good = true;
                break;
            }
        }
        if(good)
            newPruners.push_back(pruners.at(ip));
    }
    pruners.clear();
    pruners.swap(newPruners);
}

void DCCR::candidateRange(RadiansRange_V& crr, RadiansPoint_V& skirt_v, size_t k, Vertex& q)
{
    //ofstream error;
    //error.open ("data/debug/error.txt", ios::out | ios::app );
    //assert(error.is_open());

    sort(skirt_v.begin(), skirt_v.end());
    for(size_t isv1=0; isv1<skirt_v.size(); isv1++)
    {
        RadiansPoint& rp1 = skirt_v.at(isv1);
        Radians ccwBound = rp1.angle + M_PI;// + EPS;
        Radians cwBound = rp1.angle - M_PI;// - EPS;
        for(size_t isv2=0; isv2<skirt_v.size(); isv2++)
        {
            if(isv2 == isv1)
            {
                rp1.cw++;
                rp1.ccw++;
                continue;
            }
            RadiansPoint& rp2 = skirt_v.at(isv2);
            if(rp2.angle > rp1.angle)
            {
                if(rp2.angle < ccwBound)
                    rp1.ccw++;
                if(rp2.angle - Util2D::T_PI > cwBound)
                    rp1.cw++;
            }
            else if(rp2.angle < rp1.angle)
            {
                if(rp2.angle + Util2D::T_PI < ccwBound)
                    rp1.ccw++;
                if(rp2.angle > cwBound)
                    rp1.cw++;
            }
            else
            {
                rp1.cw++;
                rp1.ccw++;
            }
        }
    }

    for(size_t isv1=0; isv1<skirt_v.size(); isv1++)
    {
        RadiansPoint& rp1 = skirt_v.at(isv1);
        if(rp1.ccw == k)
        {
            Radians ccwBound = rp1.angle + M_PI;
            size_t isv2 = isv1 + 1;
            if(isv2 == skirt_v.size())
                isv2 = 0;
            while(isv2 != isv1)
            {
                RadiansPoint& rp2 = skirt_v.at(isv2);
                if(rp2.angle > ccwBound || (rp2.angle < rp1.angle && rp2.angle + Util2D::T_PI > ccwBound))
                {
                    if(rp2.cw == k)
                    {
                        RadiansRange rr;
                        rr.from = rp1.angle - Util2D::H_PI;
                        if(rr.from < 0)
                            rr.from += Util2D::T_PI;
                        rr.to = rp2.angle + Util2D::H_PI;
                        if(rr.to >= Util2D::T_PI)
                            rr.to -= Util2D::T_PI;

                        bool further = true;
                        size_t isvCheck = isv1;
                        while(true)
                        {
                            isvCheck++;
                            if(isvCheck == skirt_v.size())
                                isvCheck = 0;
                            if(isvCheck == isv2)
                                break;
                            RadiansPoint& rpCheck = skirt_v.at(isvCheck);
                            if(rpCheck.ccw < k || rpCheck.cw < k)
                            {
                                further = false;
                                break;
                            }
                        }

                        // not just the angle, but also the halfSpace
                        if(further)
                        {
                            HalfSpace::halfSpace(rp1.point, q, rr.start);
                            HalfSpace::halfSpace(rp2.point, q, rr.end);
                            rr.ok = true;
                        }
                        crr.push_back(rr);

                        break;
                    }
                }
                isv2++;
                if(isv2 == skirt_v.size())
                    isv2 = 0;
            }
        }
    }

    //error.close();
}

// *****************************************************************************
//  outer skirt
// *****************************************************************************

void DCCR::outerSkirt(RStarTree& band, RStarTree& skirt, RadiansPoint_V& skirt_v, KDepthContour& kdc, Vertex& q)
{
    MinHeap heap;
    heap.push(MinHeapEntry(1, band.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.entryPtr)
        {
            Vertex data;
            data.x = e.entryPtr->mbre.coord[0][0];
            data.y = e.entryPtr->mbre.coord[1][0];
            if(contain(kdc, data) == CONTAIN_NOT)
            {
                skirt.insertData(e.entryPtr);
                RadiansPoint rp;
                rp.angle = Util2D::angle(q.x, q.y, data.x, data.y);
                rp.point = data;
                skirt_v.push_back(rp);
            }
        }
        else
        {
            int containment = contain(kdc, e.nodePtr->mbrn);
            if(containment == CONTAIN_PARTIAL)
            {
                if(e.nodePtr->level)
                {
                    Node_P_V& children = *e.nodePtr->children;
                    for(size_t ic = 0; ic < children.size(); ic++)
                    {
                        Node_P childPtr = children.at(ic);
                        heap.push(MinHeapEntry(1, childPtr));
                    }
                }
                else
                {
                    Entry_P_V& entries = *e.nodePtr->entries;
                    for(size_t ie = 0; ie < entries.size(); ie++)
                    {
                        Entry_P entryPtr = entries.at(ie);
                        heap.push(MinHeapEntry(1, entryPtr));
                    }
                }
            }
            else if(containment == CONTAIN_NOT)
                insert(skirt, skirt_v, e.nodePtr, q);
        }
    }
}

void DCCR::insert(RStarTree& skirt, RadiansPoint_V& skirt_v, Node_P nodePtr, Vertex& q)
{
    MinHeap heap;
    heap.push(MinHeapEntry(1, nodePtr));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.entryPtr)
        {
            skirt.insertData(e.entryPtr);
            Vertex data;
            data.x = e.entryPtr->mbre.coord[0][0];
            data.y = e.entryPtr->mbre.coord[1][0];
            RadiansPoint rp;
            rp.angle = Util2D::angle(q.x, q.y, data.x, data.y);
            rp.point = data;
            skirt_v.push_back(rp);
        }
        else
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(1, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    skirt.insertData(entryPtr);
                    Vertex data;
                    data.x = entryPtr->mbre.coord[0][0];
                    data.y = entryPtr->mbre.coord[1][0];
                    RadiansPoint rp;
                    rp.angle = Util2D::angle(q.x, q.y, data.x, data.y);
                    rp.point = data;
                    skirt_v.push_back(rp);
                }
            }
        }
    }
}

int DCCR::contain(KDepthContour& kdc, Mbr& mbr)
{
    Vertex corner[4];
    corner[0].x = mbr.coord[0][0];
    corner[0].y = mbr.coord[1][0];
    corner[1].x = mbr.coord[0][0];
    corner[1].y = mbr.coord[1][1];
    corner[2].x = mbr.coord[0][1];
    corner[2].y = mbr.coord[1][1];
    corner[3].x = mbr.coord[0][1];
    corner[3].y = mbr.coord[1][0];

    bool out = false;
    for(size_t il = 0; il < kdc.upperHullLine.size(); il++)
    {
        size_t cnt = 0;
        for(size_t ic = 0; ic < 4; ic++)
            if(Util2D::isAboveLine(kdc.upperHullLine.at(il), corner[ic]) == Util2D::STRICT_ABOVE)
            {
                out = true;
                cnt++;
            }
        if(cnt == 4)
            return CONTAIN_NOT;
    }
    for(size_t il = 0; il < kdc.lowerHullLine.size(); il++)
    {
        size_t cnt = 0;
        for(size_t ic = 0; ic < 4; ic++)
            if(Util2D::isAboveLine(kdc.lowerHullLine.at(il), corner[ic]) == Util2D::STRICT_BELOW)
            {
                out = true;
                cnt++;
            }
        if(cnt == 4)
            return CONTAIN_NOT;
    }
    if(out)
        return CONTAIN_PARTIAL;
    return CONTAIN_COMPLETE;
}

int DCCR::contain(KDepthContour& kdc, Vertex& p)
{
    for(size_t il = 0; il < kdc.upperHullLine.size(); il++)
        if(Util2D::isAboveLine(kdc.upperHullLine.at(il), p) != Util2D::STRICT_BELOW) // q is above upper hull
            return CONTAIN_NOT;
    for(size_t il = 0; il < kdc.lowerHullLine.size(); il++)
        if(Util2D::isAboveLine(kdc.lowerHullLine.at(il), p) != Util2D::STRICT_ABOVE) // q is below lower hull
            return CONTAIN_NOT;
    return CONTAIN_COMPLETE;
}

// *****************************************************************************
//  filtering and verification
// *****************************************************************************

void DCCR::filterVerify(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr)
{
    //ofstream log;
    //log.open ("data/debug/log.txt", ios::out | ios::app );
    //assert(log.is_open());

    Sector_L secs;

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Argument::CIOdccr++;

        int incr = contain(cr, e.nodePtr);
        if(incr == CONTAIN_NOT)
            notResult.insert(e.nodePtr);
        else
        {
            int incrr = contain(crr, e.nodePtr, q);
            if(incrr == CONTAIN_NOT)
                notResult.insert(e.nodePtr);
            else
            {
                if(incr == CONTAIN_PARTIAL || incrr == CONTAIN_PARTIAL)
                {
                    if(e.nodePtr->level)
                    {
                        Node_P_V& children = *e.nodePtr->children;
                        for(size_t ic = 0; ic < children.size(); ic++)
                        {
                            Node_P childPtr = children.at(ic);
                            heap.push(MinHeapEntry(0, childPtr));
                        }
                    }
                    else
                    {
                        Entry_P_V& entries = *e.nodePtr->entries;
                        for(size_t ie = 0; ie < entries.size(); ie++)
                        {
                            Entry_P entryPtr = entries.at(ie);
                            // data inside candidate region & candidate range
                            if(contain(cr, entryPtr) == CONTAIN_COMPLETE && contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                            {
                                if(contain(secs, entryPtr, q) == CONTAIN_COMPLETE)
                                    result.insert(entryPtr);
                                // brute force verification
                                else
                                {
                                    Point data(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                                    double r = data.distance2(query);// - EPS;
                                    size_t cnt = RStarTreeUtil::rangeQuery2SmallTree(skirt, data, r, k);
                                    if(cnt < k)
                                        result.insert(entryPtr);
                                    else
                                        notResult.insert(entryPtr);
                                }
                            }
                            // data not inside candidate region & candidate range
                            else
                                notResult.insert(entryPtr);
                        }
                    }
                }
                else
                {
                    if(contain(secs, e.nodePtr, q) == CONTAIN_COMPLETE)
                        addResult(result, e.nodePtr);
                    else
                    {
                        Mbr& mbr = e.nodePtr->mbrn;
                        size_t min = 0, max = 0;
                        minmax(skirt, query, q, mbr, min, max);
                        // if maximum possible fn farther than q is less than k
                        if(max < k)
                        {
                            addResult(result, e.nodePtr);
                            Sector sec;
                            getSector(e.nodePtr, q, sec);
                            addSector(secs, sec);
                        }
                        // if minimum possible fn farther than q is more than k
                        else if(min >= k)
                            notResult.insert(e.nodePtr);
                        // not sure about it, break down into smaller ones
                        else
                        {
                            if(e.nodePtr->level)
                            {
                                Node_P_V& children = *e.nodePtr->children;
                                for(size_t ic = 0; ic < children.size(); ic++)
                                {
                                    Node_P childPtr = children.at(ic);
                                    heap.push(MinHeapEntry(0, childPtr));
                                }
                            }
                            else
                            {
                                Entry_P_V& entries = *e.nodePtr->entries;
                                for(size_t ie = 0; ie < entries.size(); ie++)
                                {
                                    Entry_P entryPtr = entries.at(ie);
                                    // data inside candidate region & candidate range
                                    if(contain(cr, entryPtr) == CONTAIN_COMPLETE && contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                                    {
                                        if(contain(secs, entryPtr, q) == CONTAIN_COMPLETE)
                                            result.insert(entryPtr);
                                        // brute force verification
                                        else
                                        {
                                            Point data(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                                            double r = data.distance2(query);// - EPS;
                                            size_t cnt = RStarTreeUtil::rangeQuery2SmallTree(skirt, data, r, k);
                                            if(cnt < k)
                                                result.insert(entryPtr);
                                            else
                                                notResult.insert(entryPtr);
                                        }
                                    }
                                    // data not inside candidate region & candidate range
                                    else
                                        notResult.insert(entryPtr);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //cout << "(" << secs.size() << ")" << flush;
    //log.close();
}

void DCCR::filterVerifyS(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr)
{
    //ofstream log;
    //log.open ("data/debug/log.txt", ios::out | ios::app );
    //assert(log.is_open());

    Sector_L secs;

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Argument::CIOdccr++;

        int incr = contain(cr, e.nodePtr);
        if(incr == CONTAIN_NOT)
            notResult.insert(e.nodePtr);
        else
        {
            int incrr = contain(crr, e.nodePtr, q);
            if(incrr == CONTAIN_NOT)
                notResult.insert(e.nodePtr);
            else
            {
                if(incr == CONTAIN_PARTIAL || incrr == CONTAIN_PARTIAL)
                {
                    if(e.nodePtr->level)
                    {
                        Node_P_V& children = *e.nodePtr->children;
                        for(size_t ic = 0; ic < children.size(); ic++)
                        {
                            Node_P childPtr = children.at(ic);
                            heap.push(MinHeapEntry(0, childPtr));
                        }
                    }
                    else
                    {
                        Entry_P_V& entries = *e.nodePtr->entries;
                        for(size_t ie = 0; ie < entries.size(); ie++)
                        {
                            Entry_P entryPtr = entries.at(ie);
                            // data inside candidate region & candidate range
                            if(contain(cr, entryPtr) == CONTAIN_COMPLETE && contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                            {
                                if(contain(secs, entryPtr, q) == CONTAIN_COMPLETE)
                                    result.insert(entryPtr);
                                // brute force verification
                                else
                                {
                                    Point data(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                                    double r = data.distance2(query);// - EPS;
                                    size_t cnt = RStarTreeUtil::rangeQuery2SmallTree(skirt, data, r, k);
                                    if(cnt < k)
                                        result.insert(entryPtr);
                                    else
                                        notResult.insert(entryPtr);
                                }
                            }
                            // data not inside candidate region & candidate range
                            else
                                notResult.insert(entryPtr);
                        }
                    }
                }
                else
                {
                    if(contain(secs, e.nodePtr, q) == CONTAIN_COMPLETE)
                        addResult(result, e.nodePtr);
                    else
                    {
                        Mbr& mbr = e.nodePtr->mbrn;
                        size_t min = 0, max = 0;
                        minmax(skirt, query, q, mbr, min, max);
                        // if maximum possible fn farther than q is less than k
                        if(max < k)
                        {
                            addResult(result, e.nodePtr);
                            Sector sec;
                            getSector(e.nodePtr, q, sec);
                            addSector(secs, sec);
                        }
                        // if minimum possible fn farther than q is more than k
                        else if(min >= k)
                            notResult.insert(e.nodePtr);
                        // not sure about it, break down into smaller ones
                        else
                        {
                            if(e.nodePtr->level)
                            {
                                Node_P_V& children = *e.nodePtr->children;
                                for(size_t ic = 0; ic < children.size(); ic++)
                                {
                                    Node_P childPtr = children.at(ic);
                                    heap.push(MinHeapEntry(0, childPtr));
                                }
                            }
                            else
                            {
                                Entry_P_V& entries = *e.nodePtr->entries;
                                for(size_t ie = 0; ie < entries.size(); ie++)
                                {
                                    Entry_P entryPtr = entries.at(ie);
                                    // data inside candidate region & candidate range
                                    if(contain(cr, entryPtr) == CONTAIN_COMPLETE && contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                                    {
                                        if(contain(secs, entryPtr, q) == CONTAIN_COMPLETE)
                                            result.insert(entryPtr);
                                        // brute force verification
                                        else
                                        {
                                            Point data(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                                            double r = data.distance2(query);// - EPS;
                                            size_t cnt = RStarTreeUtil::rangeQuery2SmallTree(skirt, data, r, k);
                                            if(cnt < k)
                                                result.insert(entryPtr);
                                            else
                                                notResult.insert(entryPtr);
                                        }
                                    }
                                    // data not inside candidate region & candidate range
                                    else
                                        notResult.insert(entryPtr);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //cout << "(" << secs.size() << ")" << flush;
    //log.close();
}

void DCCR::tpruFVCR(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr)
{
    //ofstream log;
    //log.open ("data/debug/log.txt", ios::out | ios::app );
    //assert(log.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Argument::CIOdccr++;

        int containment = contain(cr, e.nodePtr);
        // node inside candidate region
        if(containment == CONTAIN_COMPLETE)
            ;
        // node intersect with candidate region & candidate range
        else if(containment == CONTAIN_PARTIAL)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(0, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    // data inside candidate region & candidate range
                    if(contain(cr, entryPtr) == CONTAIN_COMPLETE)
                        ;
                    // data not inside candidate region & candidate range
                    else
                        Argument::cntPCR++;
                }
            }
        }
        // node outside of candidate region
        else
            Argument::cntPCR += e.nodePtr->aggregate;
    }

    //log.close();
}

void DCCR::tpruFVCRR(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr)
{
    //ofstream log;
    //log.open ("data/debug/log.txt", ios::out | ios::app );
    //assert(log.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Argument::CIOdccr++;

        int containment = contain(crr, e.nodePtr, q);
        if(containment == CONTAIN_COMPLETE)
            ;
        // node intersect with candidate region & candidate range
        else if(containment == CONTAIN_PARTIAL)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(0, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    // data inside candidate region & candidate range
                    if(contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                        ;
                    // data not inside candidate region & candidate range
                    else
                        Argument::cntPCRR++;
                }
            }
        }
        // node outside of candidate region
        else
            Argument::cntPCRR += e.nodePtr->aggregate;
    }

    //log.close();
}

void DCCR::tpruFVB(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr)
{
    //ofstream log;
    //log.open ("data/debug/log.txt", ios::out | ios::app );
    //assert(log.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Argument::CIOdccr++;

        int containment = contain(cr, e.nodePtr);
        // node inside candidate region
        if(containment == CONTAIN_COMPLETE)
            containment = contain(crr, e.nodePtr, q);
        if(containment == CONTAIN_COMPLETE)
            ;
        // node intersect with candidate region & candidate range
        else if(containment == CONTAIN_PARTIAL)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(0, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    // data inside candidate region & candidate range
                    if(contain(cr, entryPtr) == CONTAIN_COMPLETE && contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                        ;
                    // data not inside candidate region & candidate range
                    else
                        Argument::cntPB++;
                }
            }
        }
        // node outside of candidate region
        else
            Argument::cntPB += e.nodePtr->aggregate;
    }

    //log.close();
}

void DCCR::tpruFVS(RStarTree& skirt, RStarTree& cTree, Point& query, Vertex& q, size_t k, CandidateRegion& cr, Result& result, Result& notResult, RadiansRange_V& crr)
{
    Sector_L secs;

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Argument::CIOdccr++;

        int incr = contain(cr, e.nodePtr);
        if(incr == CONTAIN_NOT)
            ;
        else
        {
            int incrr = contain(crr, e.nodePtr, q);
            if(incrr == CONTAIN_NOT)
                ;
            else
            {
                if(incr == CONTAIN_PARTIAL || incrr == CONTAIN_PARTIAL)
                {
                    if(e.nodePtr->level)
                    {
                        Node_P_V& children = *e.nodePtr->children;
                        for(size_t ic = 0; ic < children.size(); ic++)
                        {
                            Node_P childPtr = children.at(ic);
                            heap.push(MinHeapEntry(0, childPtr));
                        }
                    }
                    else
                    {
                        Entry_P_V& entries = *e.nodePtr->entries;
                        for(size_t ie = 0; ie < entries.size(); ie++)
                        {
                            Entry_P entryPtr = entries.at(ie);
                            // data inside candidate region & candidate range
                            if(contain(cr, entryPtr) == CONTAIN_COMPLETE && contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                            {
                                if(contain(secs, entryPtr, q) == CONTAIN_COMPLETE)
                                {
                                    result.insert(entryPtr);
                                    Argument::cntIS++;
                                }
                                // brute force verification
                                else
                                {
                                    Point data(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                                    double r = data.distance2(query);// - EPS;
                                    size_t cnt = RStarTreeUtil::rangeQuery2SmallTree(skirt, data, r, k);
                                    if(cnt < k)
                                        result.insert(entryPtr);
                                    else
                                        ;
                                }
                            }
                            // data not inside candidate region & candidate range
                            else
                                ;
                        }
                    }
                }
                else
                {
                    if(contain(secs, e.nodePtr, q) == CONTAIN_COMPLETE)
                    {
                        addResult(result, e.nodePtr);
                        Argument::cntIS += e.nodePtr->aggregate;
                    }
                    else
                    {
                        Mbr& mbr = e.nodePtr->mbrn;
                        size_t min = 0, max = 0;
                        minmax(skirt, query, q, mbr, min, max);
                        // if maximum possible fn farther than q is less than k
                        if(max < k)
                        {
                            addResult(result, e.nodePtr);
                            Sector sec;
                            getSector(e.nodePtr, q, sec);
                            addSector(secs, sec);
                        }
                        // if minimum possible fn farther than q is more than k
                        else if(min >= k)
                            ;
                        // not sure about it, break down into smaller ones
                        else
                        {
                            if(e.nodePtr->level)
                            {
                                Node_P_V& children = *e.nodePtr->children;
                                for(size_t ic = 0; ic < children.size(); ic++)
                                {
                                    Node_P childPtr = children.at(ic);
                                    heap.push(MinHeapEntry(0, childPtr));
                                }
                            }
                            else
                            {
                                Entry_P_V& entries = *e.nodePtr->entries;
                                for(size_t ie = 0; ie < entries.size(); ie++)
                                {
                                    Entry_P entryPtr = entries.at(ie);
                                    // data inside candidate region & candidate range
                                    if(contain(cr, entryPtr) == CONTAIN_COMPLETE && contain(crr, entryPtr, q) == CONTAIN_COMPLETE)
                                    {
                                        if(contain(secs, entryPtr, q) == CONTAIN_COMPLETE)
                                        {
                                            result.insert(entryPtr);
                                            Argument::cntIS++;
                                        }
                                        // brute force verification
                                        else
                                        {
                                            Point data(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                                            double r = data.distance2(query);// - EPS;
                                            size_t cnt = RStarTreeUtil::rangeQuery2SmallTree(skirt, data, r, k);
                                            if(cnt < k)
                                                result.insert(entryPtr);
                                            else
                                                ;
                                        }
                                    }
                                    // data not inside candidate region & candidate range
                                    else
                                        ;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Argument::cntRST += result.aggregate;
}

void DCCR::filterVerify(RStarTree& cTree, CandidateRegion& cr, Result& result, Result& notResult)
{
    //ofstream log;
    //log.open ("data/debug/log.txt", ios::out | ios::app );
    //assert(log.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        Argument::CIOdccr++;

        int containment = contain(cr, e.nodePtr);
        // node inside candidate region
        if(containment == CONTAIN_COMPLETE)
            addResult(result, e.nodePtr);
        // node intersect with candidate region
        else if(containment == CONTAIN_PARTIAL)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(0, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    // data inside candidate region
                    if(contain(cr, entryPtr) == CONTAIN_COMPLETE)
                        result.insert(entryPtr);
                    // data not inside candidate region
                    else
                        notResult.insert(entryPtr);
                }
            }
        }
        // node outside of candidate region
        else
            notResult.insert(e.nodePtr);
    }

    //log.close();
}

void DCCR::addResult(Result& result, Node_P nodePtr)
{
    MinHeap heap;
    heap.push(MinHeapEntry(1, nodePtr));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr->level)
        {
            Node_P_V& children = *e.nodePtr->children;
            for(size_t ic = 0; ic < children.size(); ic++)
            {
                Node_P childPtr = children.at(ic);
                heap.push(MinHeapEntry(1, childPtr));
                Argument::CIOdccr++;
            }
        }
        else
        {
            Entry_P_V& entries = *e.nodePtr->entries;
            for(size_t ie = 0; ie < entries.size(); ie++)
            {
                Entry_P entryPtr = entries.at(ie);
                result.insert(entryPtr);
            }
        }
    }
}

int DCCR::contain(CandidateRegion& cr, Entry_P entryPtr)
{
    Vertex client;
    client.x = entryPtr->mbre.coord[0][0];
    client.y = entryPtr->mbre.coord[1][0];
    for(size_t ihs = 0; ihs < cr.hSpaces.size(); ihs++)
    {
        HalfSpace& hSpace = cr.hSpaces.at(ihs);
        if(Util2D::isAboveLine(hSpace.line, client) == hSpace.isAbove)
            return CONTAIN_NOT;
    }
    return CONTAIN_COMPLETE;
}

int DCCR::contain(CandidateRegion& cr, Node_P nodePtr)
{
    Vertex clients[4];
    clients[0].x = nodePtr->mbrn.coord[0][0];
    clients[0].y = nodePtr->mbrn.coord[1][0];
    clients[1].x = nodePtr->mbrn.coord[0][0];
    clients[1].y = nodePtr->mbrn.coord[1][1];
    clients[2].x = nodePtr->mbrn.coord[0][1];
    clients[2].y = nodePtr->mbrn.coord[1][0];
    clients[3].x = nodePtr->mbrn.coord[0][1];
    clients[3].y = nodePtr->mbrn.coord[1][1];
    bool inside = false, outside = false;
    for(size_t ihs = 0; ihs < cr.hSpaces.size(); ihs++)
    {
        HalfSpace& hSpace = cr.hSpaces.at(ihs);
        for(size_t ic = 0; ic < 4; ic++)
        {
            int status = Util2D::isAboveLine(hSpace.line, clients[ic]);
            if(status == hSpace.isAbove)
                outside = true;
            else if(status == Util2D::AROUND)
                ; // ignore
            else
                inside = true;
        }
        if(outside && !inside)
            return CONTAIN_NOT;
    }
    if(inside && !outside)
        return CONTAIN_COMPLETE;
    return CONTAIN_PARTIAL;
}

int DCCR::contain(RadiansRange_V& crr, Vertex& p, Vertex& q)
{
    for(size_t icrr=0; icrr<crr.size(); icrr++)
    {
        RadiansRange& rs = crr.at(icrr);
        if(rs.ok)
        {
            if(Util2D::isAboveLine(rs.start.line, p) != rs.start.isAbove && Util2D::isAboveLine(rs.end.line, p) != rs.end.isAbove)
                return CONTAIN_COMPLETE;
        }
        else
        {
            Radians r = Util2D::angle(q.x, q.y, p.x, p.y);
            if(rs.from < rs.to)
            {
                if(rs.from < r && r < rs.to)
                    return CONTAIN_COMPLETE;
            }
            else
            {
                if(rs.from < r || r < rs.to)
                    return CONTAIN_COMPLETE;
            }
        }
    }
    return CONTAIN_NOT;
}

int DCCR::contain(RadiansRange_V& crr, Entry_P entryPtr, Vertex& q)
{
    for(size_t icrr=0; icrr<crr.size(); icrr++)
    {
        RadiansRange& rs = crr.at(icrr);
        Vertex p;
        p.x = entryPtr->mbre.coord[0][0];
        p.y = entryPtr->mbre.coord[1][0];
        if(rs.ok)
        {
            if(Util2D::isAboveLine(rs.start.line, p) != rs.start.isAbove && Util2D::isAboveLine(rs.end.line, p) != rs.end.isAbove)
                return CONTAIN_COMPLETE;
        }
        else
        {
            Radians r = Util2D::angle(q.x, q.y, p.x, p.y);
            if(rs.from < rs.to)
            {
                if(rs.from < r && r < rs.to)
                    return CONTAIN_COMPLETE;
            }
            else
            {
                if(rs.from < r || r < rs.to)
                    return CONTAIN_COMPLETE;
            }
        }
    }
    return CONTAIN_NOT;
}

int DCCR::contain(RadiansRange_V& crr, Node_P nodePtr, Vertex& q)
{
    Vertex p[4];
    p[0].x = nodePtr->mbrn.coord[0][0];
    p[0].y = nodePtr->mbrn.coord[1][0];
    p[1].x = nodePtr->mbrn.coord[0][0];
    p[1].y = nodePtr->mbrn.coord[1][1];
    p[2].x = nodePtr->mbrn.coord[0][1];
    p[2].y = nodePtr->mbrn.coord[1][0];
    p[3].x = nodePtr->mbrn.coord[0][1];
    p[3].y = nodePtr->mbrn.coord[1][1];

    for(size_t icrr=0; icrr<crr.size(); icrr++)
    {
        RadiansRange& rs = crr.at(icrr);
        if(rs.ok)
        {
            bool s[4][2];
            for(size_t ip = 0; ip < 4; ip++)
            {
                s[ip][0] = (Util2D::isAboveLine(rs.start.line, p[ip]) != rs.start.isAbove);
                s[ip][1] = (Util2D::isAboveLine(rs.end.line, p[ip]) != rs.end.isAbove);
            }
            if(s[0][0] && s[0][1] && s[1][0] && s[1][1] && s[2][0] && s[2][1] && s[3][0] && s[3][1])
                return CONTAIN_COMPLETE;
            if( (s[0][0]^s[1][0]) || (s[0][0]^s[2][0]) || (s[0][0]^s[3][0]) || (s[1][0]^s[2][0]) || (s[1][0]^s[3][0]) || (s[2][0]^s[3][0]) )
                return CONTAIN_PARTIAL;
            if( (s[0][1]^s[1][1]) || (s[0][1]^s[2][1]) || (s[0][1]^s[3][1]) || (s[1][1]^s[2][1]) || (s[1][1]^s[3][1]) || (s[2][1]^s[3][1]) )
                return CONTAIN_PARTIAL;
        }
    }

    if( (nodePtr->mbrn.coord[0][0] - EPS < q.x && q.x < nodePtr->mbrn.coord[0][1] + EPS)
        && (nodePtr->mbrn.coord[1][0] - EPS < q.y && q.y < nodePtr->mbrn.coord[1][1]) + EPS )
        return CONTAIN_PARTIAL;

    // calculate node range
    Radians r[4];
    bool p1 = false, p4 = false;
    for(size_t ip = 0; ip < 4; ip++)
    {
        r[ip] = Util2D::angle(q.x, q.y, p[ip].x, p[ip].y);
        if(p[ip].x > q.x)
        {
            if(p[ip].y > q.y)
                p1 = true;
            else
                p4 = true;
        }
    }
    Radians from, to; // node range
    if(p1 && p4)
    {
        from = Util2D::T_PI;
        to = 0;
        for(size_t ip = 0; ip < 4; ip++)
            if(r[ip] < M_PI)
                to = max(to, r[ip]);
            else
                from = min(from, r[ip]);
    }
    else
    {
        from = min(min(r[0], r[1]), min(r[2], r[3]));
        to = max(max(r[0], r[1]), max(r[2], r[3]));
    }

    // check node range against candidate range
    for(size_t icrr=0; icrr<crr.size(); icrr++)
    {
        RadiansRange& rs = crr.at(icrr);
        if(!rs.ok)
        {
            // if node range is completely inside candidate range
            if(rs.from < rs.to)
            {
                if( (rs.from < from && from < rs.to) && (rs.from < to && to < rs.to) )
                    return CONTAIN_COMPLETE;
            }
            else
            {
                if( (rs.from < from || from < rs.to) && (rs.from < to || to < rs.to) )
                    return CONTAIN_COMPLETE;
            }

            // if any one of the boundary of candidate range is inside node range => overlap
            if(from < to)
            {
                if( (from < rs.from && rs.from < to) || (from < rs.to && rs.to < to) )
                    return CONTAIN_PARTIAL;
            }
            else
            {
                if( (from < rs.from || rs.from < to) || (from < rs.to || rs.to < to) )
                    return CONTAIN_PARTIAL;
            }
        }
    }

    return CONTAIN_NOT;
}

void DCCR::minmax(RStarTree& skirt, Point& query, Vertex& q, Mbr& mbr, size_t& min, size_t& max)
{
    Coord minQ = RStarTreeUtil::minDis2(query, mbr);
    Coord maxQ = RStarTreeUtil::maxDis2(query, mbr);

    Vertex corner[4];
    corner[0].x = mbr.coord[0][0];
    corner[0].y = mbr.coord[1][0];
    corner[1].x = mbr.coord[0][0];
    corner[1].y = mbr.coord[1][1];
    corner[2].x = mbr.coord[0][1];
    corner[2].y = mbr.coord[1][0];
    corner[3].x = mbr.coord[0][1];
    corner[3].y = mbr.coord[1][1];

    MinHeap heap;
    heap.push(MinHeapEntry(1, skirt.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr->level)
        {
            Node_P_V& children = *e.nodePtr->children;
            for(size_t ic = 0; ic < children.size(); ic++)
            {
                Node_P childPtr = children.at(ic);
                heap.push(MinHeapEntry(1, childPtr));
            }
        }
        else
        {
            Entry_P_V& entries = *e.nodePtr->entries;
            for(size_t ie = 0; ie < entries.size(); ie++)
            {
                Entry_P entryPtr = entries.at(ie);

                Vertex p;
                p.x = entryPtr->mbre.coord[0][0];
                p.y = entryPtr->mbre.coord[1][0];
                HalfSpace hSpace;
                HalfSpace::halfSpace(p, q, hSpace);
                if(Util2D::isAboveLine(hSpace.line, corner[0]) != hSpace.isAbove
                   && Util2D::isAboveLine(hSpace.line, corner[1]) != hSpace.isAbove
                   && Util2D::isAboveLine(hSpace.line, corner[2]) != hSpace.isAbove
                   && Util2D::isAboveLine(hSpace.line, corner[3]) != hSpace.isAbove )
                   continue;

                Point point;
                point.coord[0] = entryPtr->mbre.coord[0][0];
                point.coord[1] = entryPtr->mbre.coord[1][0];
                Coord minM = RStarTreeUtil::minDis2(point, mbr);
                Coord maxM = RStarTreeUtil::maxDis2(point, mbr);
                if(maxM > minQ + EPS)
                {
                    max++;
                    if(minM > maxQ + EPS)
                        min++;
                }
            }
        }
    }

    //if(max < 10)
        //cout << "*" << flush;
    //cout << min << " " << max << endl;
}

void DCCR::getSector(Node_P nodePtr, Vertex& q, Sector& sec)
{
    Vertex p[4];
    p[0].x = nodePtr->mbrn.coord[0][0];
    p[0].y = nodePtr->mbrn.coord[1][0];
    p[1].x = nodePtr->mbrn.coord[0][0];
    p[1].y = nodePtr->mbrn.coord[1][1];
    p[2].x = nodePtr->mbrn.coord[0][1];
    p[2].y = nodePtr->mbrn.coord[1][0];
    p[3].x = nodePtr->mbrn.coord[0][1];
    p[3].y = nodePtr->mbrn.coord[1][1];

    Radians r[4];
    bool p1 = false, p4 = false;
    for(size_t ip = 0; ip < 4; ip++)
    {
        r[ip] = Util2D::angle(q.x, q.y, p[ip].x, p[ip].y);
        if(p[ip].x > q.x)
        {
            if(p[ip].y >= q.y)
                p1 = true;
            else
                p4 = true;
        }
    }

    Coord disfrom, disto;
    if(p1 && p4)
    {
        sec.from = Util2D::T_PI + 1;
        sec.to = -1;
        for(size_t ip = 0; ip < 4; ip++)
            if(r[ip] < M_PI && r[ip] > sec.to)
            {
                sec.to = r[ip];
                disto = Util2D::distance2(p[ip], q);
            }
            else if(r[ip] >= M_PI && r[ip] < sec.from)
            {
                sec.from = r[ip];
                disfrom = Util2D::distance2(p[ip], q);
            }
    }
    else
    {
        Radians min = Util2D::T_PI + 1, max = -1;
        size_t mini = 0, maxi = 0;
        for(size_t ip = 0; ip < 4; ip++)
        {
            if(r[ip] < min)
            {
                min = r[ip];
                mini = ip;
            }
            if(r[ip] > max)
            {
                max = r[ip];
                maxi = ip;
            }
        }
        sec.from = min;
        disfrom = Util2D::distance2(p[mini], q);
        sec.to = max;
        disto = Util2D::distance2(p[maxi], q);
    }
    sec.dis = max(disfrom, disto);
}

void DCCR::addSector(Sector_L& secs, Sector& sec)
{
    if(sec.from > sec.to)
    {
        Sector first;
        first.from = sec.from;
        first.to = Util2D::T_PI;
        first.dis = sec.dis;
        addSector(secs, first);

        Sector second;
        second.from = 0;
        second.to = sec.to;
        second.dis = sec.dis;
        addSector(secs, second);
    }
    else
    {
        if(secs.size() == 0)
        {
            secs.push_front(sec);
        }
        else
        {
            for(Sector_L::iterator iters = secs.begin(); iters != secs.end(); iters++)
            {
                Sector& cur = *iters;
                if(sec.from >= cur.to)
                {
                    if(iters == secs.end()--)
                    {
                        secs.push_back(sec);
                    }
                }
                else if(sec.to <= cur.from)
                {
                    secs.insert(iters, sec);
                    break;
                }
                // sec.to > cur.from
                else
                {
                    if(sec.from < cur.from)
                    {
                        Sector add;
                        add.from = sec.from;
                        add.to = cur.from;
                        add.dis = sec.dis;
                        secs.insert(iters, add);
                        if(sec.to <= cur.to)
                            break;
                        else
                            sec.from = cur.to;
                    }
                    // sec.from >= cur.from
                    else
                    {
                        if(sec.to <= cur.to)
                        {
                            break;
                        }
                        // sec.to > cur.to
                        else if(sec.from < cur.to)
                        {
                            sec.from = cur.to;
                            if(iters == --secs.end())
                            {
                                secs.push_back(sec);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

int DCCR::contain(Sector_L& secs, Node_P nodePtr, Vertex& q)
{
    Vertex p[4];
    p[0].x = nodePtr->mbrn.coord[0][0];
    p[0].y = nodePtr->mbrn.coord[1][0];
    p[1].x = nodePtr->mbrn.coord[0][0];
    p[1].y = nodePtr->mbrn.coord[1][1];
    p[2].x = nodePtr->mbrn.coord[0][1];
    p[2].y = nodePtr->mbrn.coord[1][0];
    p[3].x = nodePtr->mbrn.coord[0][1];
    p[3].y = nodePtr->mbrn.coord[1][1];

    Radians r[4];
    Coord dis;
    bool p1 = false, p4 = false;
    for(size_t ip = 0; ip < 4; ip++)
    {
        r[ip] = Util2D::angle(q.x, q.y, p[ip].x, p[ip].y);
        if(ip == 0)
            dis = Util2D::distance2(p[ip], q);
        else
            dis = min(dis, Util2D::distance2(p[ip], q));
        if(p[ip].x > q.x)
        {
            if(p[ip].y > q.y)
                p1 = true;
            else
                p4 = true;
        }
    }

    Radians from, to; // node range
    if(p1 && p4)
    {
        from = Util2D::T_PI;
        to = 0;
        for(size_t ip = 0; ip < 4; ip++)
            if(r[ip] < M_PI)
                to = max(to, r[ip]);
            else
                from = min(from, r[ip]);
        if(contain(secs, from, Util2D::T_PI, dis) == CONTAIN_COMPLETE && contain(secs, 0, to, dis) == CONTAIN_COMPLETE)
            return CONTAIN_COMPLETE;
        else
            return CONTAIN_NOT;
    }
    else
    {
        from = min(min(r[0], r[1]), min(r[2], r[3]));
        to = max(max(r[0], r[1]), max(r[2], r[3]));
        return contain(secs, from, to, dis);
    }
}

int DCCR::contain(Sector_L& secs, Entry_P entryPtr, Vertex& q)
{
    Vertex p;
    p.x = entryPtr->mbre.coord[0][0];
    p.y = entryPtr->mbre.coord[1][0];
    Radians angle = Util2D::angle(q.x, q.y, p.x, p.y);
    Coord dis = Util2D::distance2(p, q);
    for(Sector_L::iterator iters = secs.begin(); iters != secs.end(); iters++)
    {
        Sector& cur = *iters;
        if(cur.from > angle)
            break;
        if(cur.from <= angle && angle <= cur.to)
        {
            if(dis >= cur.dis)
                return CONTAIN_COMPLETE;
            else
                return CONTAIN_NOT;
        }
    }
    return CONTAIN_NOT;
}

int DCCR::contain(Sector_L& secs, Radians from, Radians to, Coord dis)
{
    for(Sector_L::iterator iters = secs.begin(); iters != secs.end(); iters++)
    {
        Sector& cur = *iters;
        if(to <= cur.from)
            return CONTAIN_NOT;
        else
        {
            if(from < cur.from)
                return CONTAIN_NOT;
            else
            {
                if(to <= cur.to)
                {
                    if(dis > cur.dis + EPS)
                        return CONTAIN_COMPLETE;
                    return CONTAIN_NOT;
                }
                else if(from < cur.to)
                {
                    if(dis > cur.dis + EPS)
                        from = cur.to;
                    else
                        return CONTAIN_NOT;
                }
            }
        }
    }
    return CONTAIN_NOT;
}
