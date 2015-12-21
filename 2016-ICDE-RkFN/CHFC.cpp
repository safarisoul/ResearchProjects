#include "CHFC.h"

void CHFC::chfc(RStarTree& fTree, RStarTree& cTree, Point& query, size_t k, Result& result, Result& notResult)
{
    struct timeval start, end;

    if(k == 1)
    {
        FVC1 fvc1;

        gettimeofday(&start, NULL);
        furVorCell(fTree, query, fvc1);
        gettimeofday(&end, NULL);

        long seconds = end.tv_sec  - start.tv_sec;
        long useconds = end.tv_usec - start.tv_usec;
        Argument::Tfvc += seconds * 1e6 + useconds;

        fvc1.print();

        gettimeofday(&start, NULL);
        rangeQuery(cTree, fvc1, result, notResult);
        gettimeofday(&end, NULL);

        seconds = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        Argument::Trq += seconds * 1e6 + useconds;
    }
    else
    {
        FVC fvc;

        gettimeofday(&start, NULL);
        furVorCell(fTree, query, k, fvc);
        gettimeofday(&end, NULL);

        long seconds = end.tv_sec  - start.tv_sec;
        long useconds = end.tv_usec - start.tv_usec;
        Argument::Tfvc += seconds * 1e6 + useconds;

        gettimeofday(&start, NULL);
        rangeQuery(cTree, fvc, k, result, notResult);
        gettimeofday(&end, NULL);

        seconds = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        Argument::Trq += seconds * 1e6 + useconds;
    }
}

/*******************************************************************************
 * FVC1
 ******************************************************************************/

void CHFC::furVorCell(RStarTree& fTree, Point& query, FVC1& fvc1)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    Vertex q;
    q.x = query.coord[0];
    q.y = query.coord[1];

    struct timeval start, end;

    gettimeofday(&start, NULL);
    size_t max = 0;
    ConvexHull ch;
    //chfc << "===== 1-st convex hull =====" << endl;
    if(ConHull::convexHull(fTree, ch))
    {
        Argument::NFchfc += ch.size();
        max = 1;
    }
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    Argument::Tch += seconds * 1e6 + useconds;

/*
    ofstream conhull;
    conhull.open ("data/debug/conhull.txt", ios::out | ios::trunc );
    assert(conhull.is_open());
    for(size_t ich = 1; ich <= max; ich++)
    {
        for(size_t iv = 1; iv < ch[ich].upperLeftHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].upperLeftHullVertex.at(iv - 1);
            Vertex cur = ch[ich].upperLeftHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        for(size_t iv = 1; iv < ch[ich].upperRightHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].upperRightHullVertex.at(iv - 1);
            Vertex cur = ch[ich].upperRightHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        for(size_t iv = 1; iv < ch[ich].lowerRightHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].lowerRightHullVertex.at(iv - 1);
            Vertex cur = ch[ich].lowerRightHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        for(size_t iv = 1; iv < ch[ich].lowerLeftHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].lowerLeftHullVertex.at(iv - 1);
            Vertex cur = ch[ich].lowerLeftHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        conhull << endl;
    }
    conhull.close();
*/

    if(max == 1 && contain(ch, q))
    {
        fvc1.phi = true;
        return;
    }

    fvc1.init();

    Vertex first;

    first = ch.upperLeftHullVertex.at(0);
    for(size_t iv = 1; iv < ch.upperLeftHullVertex.size(); iv++)
    {
        Vertex& cur = ch.upperLeftHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        HalfSpace hSpace;
        HalfSpace::halfSpace(cur, q, hSpace);
        fvc1.prune(hSpace);
    }

    first = ch.upperRightHullVertex.at(0);
    for(size_t iv = 1; iv < ch.upperRightHullVertex.size(); iv++)
    {
        Vertex& cur = ch.upperRightHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        HalfSpace hSpace;
        HalfSpace::halfSpace(cur, q, hSpace);
        fvc1.prune(hSpace);
    }

    first = ch.lowerLeftHullVertex.at(0);
    for(size_t iv = 1; iv < ch.lowerLeftHullVertex.size(); iv++)
    {
        Vertex& cur = ch.lowerLeftHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        HalfSpace hSpace;
        HalfSpace::halfSpace(cur, q, hSpace);
        fvc1.prune(hSpace);
    }

    first = ch.lowerRightHullVertex.at(0);
    for(size_t iv = 1; iv < ch.lowerRightHullVertex.size(); iv++)
    {
        Vertex& cur = ch.lowerRightHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        HalfSpace hSpace;
        HalfSpace::halfSpace(cur, q, hSpace);
        fvc1.prune(hSpace);
    }

    //cout << "CHFC::furVorCell() end" << endl;

    //chfc.close();
}

void CHFC::rangeQuery(RStarTree& cTree, FVC1& fvc1, Result& result, Result& notResult)
{
    if(fvc1.phi)
    {
        Argument::Schfc++;
        return;
    }

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
            Argument::CIOchfc++;
            if(e.nodePtr->level)
            {
                Vertex corner[4];
                corner[0].x = e.nodePtr->mbrn.coord[0][0];
                corner[0].y = e.nodePtr->mbrn.coord[1][0];
                corner[1].x = e.nodePtr->mbrn.coord[0][0];
                corner[1].y = e.nodePtr->mbrn.coord[1][1];
                corner[2].x = e.nodePtr->mbrn.coord[0][1];
                corner[2].y = e.nodePtr->mbrn.coord[1][1];
                corner[3].x = e.nodePtr->mbrn.coord[0][1];
                corner[3].y = e.nodePtr->mbrn.coord[1][0];
                if(contain(fvc1, corner[0]) && contain(fvc1, corner[1]) && contain(fvc1, corner[2]) && contain(fvc1, corner[3]))
                    addResult(result, e.nodePtr);
                else
                {
                    Node_P_V& children = *e.nodePtr->children;
                    for(size_t ic = 0; ic < children.size(); ic++)
                    {
                        Node_P childPtr = children.at(ic);
                        heap.push(MinHeapEntry(0, childPtr));
                    }
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    Vertex data;
                    data.x = entryPtr->mbre.coord[0][0];
                    data.y = entryPtr->mbre.coord[1][0];
                    if(contain(fvc1, data))
                        result.insert(entryPtr);
                    else
                        notResult.insert(entryPtr);
                }
            }
    }
}

bool CHFC::contain(FVC1& fvc1, Vertex& data)
{
    for(size_t ihs = 0; ihs < fvc1.hSpaces.size(); ihs++)
    {
        HalfSpace& hs = fvc1.hSpaces.at(ihs);
        if(Util2D::isAboveLine(hs.line, data) == hs.isAbove)
            return false;
    }
    return true;
}

void CHFC::addResult(Result& result, Node_P nodePtr)
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
                Argument::CIOchfc++;
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

/*******************************************************************************
 * FVC
 ******************************************************************************/

void CHFC::furVorCell(RStarTree& fTree, Point& query, size_t k, FVC& fvc)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    Vertex q;
    q.x = query.coord[0];
    q.y = query.coord[1];

    struct timeval start, end;

    gettimeofday(&start, NULL);
    size_t max = 0;
    ConvexHull ch[k + 1];
    //chfc << "===== 1-st convex hull =====" << endl;
    if(ConHull::convexHull(fTree, ch[1]))
    {
        Argument::NFchfc += ch[1].size();
        max = 1;
    }
    for(size_t ich = 2; max + 1 == ich && ich <= k; ich++)
    {
        //chfc << "===== " << ich << "-th convex hull =====" << endl;
        if(ConHull::convexHull(fTree, ch[ich], ch[ich - 1]))
        {
            Argument::NFchfc += ch[ich].size();
            max = ich;
        }
    }
    gettimeofday(&end, NULL);

    long seconds = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    Argument::Tch += seconds * 1e6 + useconds;

/*
    ofstream conhull;
    conhull.open ("data/debug/conhull.txt", ios::out | ios::trunc );
    assert(conhull.is_open());
    for(size_t ich = 1; ich <= max; ich++)
    {
        for(size_t iv = 1; iv < ch[ich].upperLeftHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].upperLeftHullVertex.at(iv - 1);
            Vertex cur = ch[ich].upperLeftHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        for(size_t iv = 1; iv < ch[ich].upperRightHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].upperRightHullVertex.at(iv - 1);
            Vertex cur = ch[ich].upperRightHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        for(size_t iv = 1; iv < ch[ich].lowerRightHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].lowerRightHullVertex.at(iv - 1);
            Vertex cur = ch[ich].lowerRightHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        for(size_t iv = 1; iv < ch[ich].lowerLeftHullVertex.size(); iv++)
        {
            Vertex pre = ch[ich].lowerLeftHullVertex.at(iv - 1);
            Vertex cur = ch[ich].lowerLeftHullVertex.at(iv);
            conhull << pre.x << " " << pre.y << " " << cur.x << " " << cur.y << endl;
        }
        conhull << endl;
    }
    conhull.close();
*/
    if(max == k && contain(ch[k], q))
    {
        fvc.phi = true;
        return;
    }

    for(size_t ich = 1; ich <= max; ich++)
        updateFVC(ch[ich], fvc, q);

    //chfc.close();
}

bool CHFC::contain(ConvexHull& ch, Vertex& q)
{
    if(q.x - EPS < ch.maxY.x && q.y + EPS > ch.minX.y)
        for(size_t il = 0; il < ch.upperLeftHullLine.size(); il++)
        {
            Line& line = ch.upperLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, q);
            if(status == Util2D::STRICT_ABOVE || status == Util2D::STRICT_LEFT)
                return false;
        }

    if(q.x + EPS > ch.maxY.x && q.y + EPS > ch.maxX.y)
        for(size_t il = 0; il < ch.upperRightHullLine.size(); il++)
        {
            Line& line = ch.upperRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, q);
            if(status == Util2D::STRICT_ABOVE || status == Util2D::STRICT_RIGHT)
                return false;
        }

    if(q.x + EPS > ch.minY.x && q.y - EPS < ch.maxX.y)
        for(size_t il = 0; il < ch.lowerRightHullLine.size(); il++)
        {
            Line& line = ch.lowerRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, q);
            if(status == Util2D::STRICT_BELOW || status == Util2D::STRICT_RIGHT)
                return false;
        }

    if(q.x - EPS < ch.minY.x && q.y - EPS < ch.minX.y)
        for(size_t il = 0; il < ch.lowerLeftHullLine.size(); il++)
        {
            Line& line = ch.lowerLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, q);
            if(status == Util2D::STRICT_BELOW || status == Util2D::STRICT_LEFT)
                return false;
        }

    return true;
}

void CHFC::updateFVC(ConvexHull& ch, FVC& fvc, Vertex& q)
{
    Vertex first;

    first = ch.upperLeftHullVertex.at(0);
    for(size_t iv = 1; iv < ch.upperLeftHullVertex.size(); iv++)
    {
        Vertex& cur = ch.upperLeftHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        updateFVC(cur, fvc, q);
    }

    first = ch.upperRightHullVertex.at(0);
    for(size_t iv = 1; iv < ch.upperRightHullVertex.size(); iv++)
    {
        Vertex& cur = ch.upperRightHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        updateFVC(cur, fvc, q);
    }

    first = ch.lowerLeftHullVertex.at(0);
    for(size_t iv = 1; iv < ch.lowerLeftHullVertex.size(); iv++)
    {
        Vertex& cur = ch.lowerLeftHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        updateFVC(cur, fvc, q);
    }

    first = ch.lowerRightHullVertex.at(0);
    for(size_t iv = 1; iv < ch.lowerRightHullVertex.size(); iv++)
    {
        Vertex& cur = ch.lowerRightHullVertex.at(iv);
        if(cur.x == first.x && cur.y == first.y)
            continue;
        updateFVC(cur, fvc, q);
    }
}

void CHFC::updateFVC(Vertex& p, FVC& fvc, Vertex& q)
{
    HalfSpace hSpace;
    HalfSpace::halfSpace(p, q, hSpace);
    fvc.prune(hSpace);
}

/*******************************************************************************
 * range query
 ******************************************************************************/

void CHFC::rangeQuery(RStarTree& cTree, FVC& fvc, size_t k, Result& result, Result& notResult)
{
    if(fvc.phi)
    {
        Argument::Schfc++;
        return;
    }

    MinHeap heap;
    heap.push(MinHeapEntry(0, cTree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::CIOchfc++;
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
                    Vertex data;
                    data.x = entryPtr->mbre.coord[0][0];
                    data.y = entryPtr->mbre.coord[1][0];
                    if(contain(fvc, data, k))
                        result.insert(entryPtr);
                    else
                        notResult.insert(entryPtr);
                }
            }
        }
        else
            assert(false);
    }
}

bool CHFC::contain(FVC& fvc, Vertex& data, size_t k)
{
    size_t cnt = 0;
    for(size_t ihs = 0; ihs < fvc.hSpaces.size(); ihs++)
    {
        HalfSpace& hs = fvc.hSpaces.at(ihs);
        if(Util2D::isAboveLine(hs.line, data) == hs.isAbove)
            cnt++;
        if(cnt >= k)
            return false;
    }
    return true;
}


