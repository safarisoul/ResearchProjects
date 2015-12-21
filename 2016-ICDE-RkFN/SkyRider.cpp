#include "SkyRider.h"

void SkyRider::kDepthContour(RStarTree& tree, KDepthContour& contour, size_t k)
{
    Line upperStart, lowerStart;
    upperEnvelopeStart(tree, k, upperStart);
    lowerEnvelopeStart(tree, k, lowerStart);

    Envelope kUpperEnvelope, kLowerEnvelope;
    envelope(tree, kUpperEnvelope, upperStart);
    envelope(tree, kLowerEnvelope, lowerStart);

/*
    ofstream envelopeOFS("data/debug/envelope.txt");
    assert(envelopeOFS.good());
    for(size_t iv = 1; iv < kUpperEnvelope.vertex.size(); iv++)
    {
        Vertex pre = kUpperEnvelope.vertex.at(iv - 1);
        Vertex cur = kUpperEnvelope.vertex.at(iv);
        envelopeOFS << pre.first << " " << pre.second << " " << cur.first << " " << cur.second << endl;
    }
    for(size_t iv = 1; iv < kLowerEnvelope.vertex.size(); iv++)
    {
        Vertex pre = kLowerEnvelope.vertex.at(iv - 1);
        Vertex cur = kLowerEnvelope.vertex.at(iv);
        envelopeOFS << pre.first << " " << pre.second << " " << cur.first << " " << cur.second << endl;
    }
    envelopeOFS.close();
*/

    //validateEnvelope(tree, kUpperEnvelope, kLowerEnvelope, k);

    Line_V upperHullLine, lowerHullLine;
    hull(upperHullLine, kUpperEnvelope, Util2D::LEFT_TURN); // upper hull of kdc == lower hull of upper envelope (left turn)
    hull(lowerHullLine, kLowerEnvelope, Util2D::RIGHT_TURN);  // lower hull of kdc == upper hull of lower envelope (right turn)

    //validateCandidateHull(*Data::fTree, upperHullLine, lowerHullLine, k);

    constructContour(contour, upperHullLine, lowerHullLine);

    //validateContour(contour);

    Util2D::lineCross(contour.upperHullLine.at(0), contour.lowerHullLine.at(contour.lowerHullLine.size() - 1), contour.left);
    Util2D::lineCross(contour.upperHullLine.at(contour.upperHullLine.size() - 1), contour.lowerHullLine.at(0), contour.right);

    //assert(contour.left.x < contour.right.x); // otherwise, k-depth contour does not exist

    Util2D::linePass(contour.left, contour.right, contour.axis);

    contour.upperHullVertex.push_back(contour.left);
    for(size_t il = 1; il < contour.upperHullLine.size(); il++)
    {
        Vertex v;
        Util2D::lineCross(contour.upperHullLine.at(il - 1), contour.upperHullLine.at(il), v);
        contour.upperHullVertex.push_back(v);
    }
    contour.upperHullVertex.push_back(contour.right);

    contour.lowerHullVertex.push_back(contour.right);
    for(size_t il = 1; il < contour.lowerHullLine.size(); il++)
    {
        Vertex v;
        Util2D::lineCross(contour.lowerHullLine.at(il - 1), contour.lowerHullLine.at(il), v);
        contour.lowerHullVertex.push_back(v);
    }
    contour.lowerHullVertex.push_back(contour.left);
}

/*******************************************************************************
 * envelope
 ******************************************************************************/

void SkyRider::upperEnvelopeStart(RStarTree& tree, size_t k, Line& start)
{
    size_t cnt = 0;
    MinHeap heap;
    heap.push(MinHeapEntry(tree.root->mbrn.coord[0][0], tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(childPtr->mbrn.coord[0][0], childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    heap.push(MinHeapEntry(entryPtr->mbre.coord[0][0], entryPtr));
                }
            }
        }
        else
        {
            cnt++;
            if(cnt == k)
            {
                start.a = e.entryPtr->mbre.coord[0][0];
                start.b = e.entryPtr->mbre.coord[1][0];
                return;
            }
        }
    }
}

void SkyRider::lowerEnvelopeStart(RStarTree& tree, size_t k, Line& start)
{
    size_t cnt = 0;
    MaxHeap heap;
    heap.push(MaxHeapEntry(tree.root->mbrn.coord[0][1], tree.root));
    while(!heap.isEmpty())
    {
        MaxHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MaxHeapEntry(childPtr->mbrn.coord[0][1], childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    heap.push(MaxHeapEntry(entryPtr->mbre.coord[0][1], entryPtr));
                }
            }
        }
        else
        {
            cnt++;
            if(cnt == k)
            {
                start.a = e.entryPtr->mbre.coord[0][0];
                start.b = e.entryPtr->mbre.coord[1][0];
                return;
            }
        }
    }
}

void SkyRider::envelope(RStarTree& tree, Envelope& envelope, Line& start)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());
    //debug << "===== get envelope =====" << endl;

    envelope.line.push_back(start);

    Line lc = start;
    Coord loc = INF_N;
    Vertex candidate;
    Line next;


    while(getIntersection(tree, lc, loc, candidate, next))
    {
        envelope.line.push_back(next);
        envelope.vertex.push_back(candidate);
        lc = next;
    }

    //debug.close();
}

bool SkyRider::getIntersection(RStarTree& tree, Line& lc, Coord& loc, Vertex& candidate, Line& next)
{
    Coord best = INF_P;
    MinHeap heap;
    heap.push(MinHeapEntry(0, tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.entryPtr)
        {
            loc = e.key;
            candidate.x = loc;
            candidate.y = lc.a * loc + lc.b;
            next.a = e.entryPtr->mbre.coord[0][0];
            next.b = e.entryPtr->mbre.coord[1][0];
            return true;
        }
        else
        {
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    Junctions junctions = junction(lc, childPtr->mbrn);
                    if(junctions.second + EPS < loc)
                        continue;
                    if(junctions.first > best + EPS)
                        continue;
                    heap.push(MinHeapEntry(junctions.first, childPtr));
                    if(junctions.first > loc + EPS && junctions.second + EPS < best)
                        best = junctions.second;
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    Line l2;
                    l2.a = entryPtr->mbre.coord[0][0];
                    l2.b = entryPtr->mbre.coord[1][0];
                    if(l2.a == lc.a && l2.b == lc.b)
                        continue;
                    Coord junction = enteringJunction(lc, l2);
                    if(junction < loc + EPS)
                        continue;
                    if(junction > best + EPS)
                        continue;
                    heap.push(MinHeapEntry(junction, entryPtr));
                    best = junction;
                }
            }
        }
    }
    return false;
}

/*
void SkyRider::validateEnvelope(RTree& tree, Envelope& upperEnvelope, Envelope& lowerEnvelope, size_t k)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());

    vector<size_t> cntAbove, cntBelow;

    //debug << "===== upper envelope =====" << endl;
    for(size_t il = 0; il < upperEnvelope.line.size(); il++)
    {
        Coord x;
        if(il == 0)
            x = upperEnvelope.vertex.at(0).first - 1;
        else if(il + 1 == upperEnvelope.line.size())
            x = upperEnvelope.vertex.at(il - 1).first + 1;
        else
            x = (upperEnvelope.vertex.at(il - 1).first + upperEnvelope.vertex.at(il).first) * 0.5;
        Line line = upperEnvelope.line.at(il);
        //debug << "y = " << line.first << "x " << (line.second >= 0 ? "+ " : "- ") << abs(line.second);
        size_t cntA = 0, cntB = 0;
        cnt(tree, x, line, cntA, cntB);
        //debug << " || " << cntA << " @x(" << x << ")";
        cntAbove.push_back(cntA);
        cntBelow.push_back(cntB);
        if(il != 0)
        {
            //Vertex vertex = upperEnvelope.vertex.at(il - 1);
            //debug << " || (" << vertex.first << ", " << vertex.second << ")";
        }
        //debug << endl;
    }

    cntAbove.clear();
    cntBelow.clear();

    //debug << "===== lower envelope =====" << endl;
    for(size_t il = 0; il < lowerEnvelope.line.size(); il++)
    {
        Coord x;
        if(il == 0)
            x = lowerEnvelope.vertex.at(0).first - 1;
        else if(il + 1 == lowerEnvelope.line.size())
            x = lowerEnvelope.vertex.at(il - 1).first + 1;
        else
            x = (lowerEnvelope.vertex.at(il - 1).first + lowerEnvelope.vertex.at(il).first) * 0.5;
        Line line = lowerEnvelope.line.at(il);
        //debug << "y = " << line.first << "x " << (line.second >= 0 ? "+ " : "- ") << abs(line.second);
        size_t cntA = 0, cntB = 0;
        cnt(tree, x, line, cntA, cntB);
        //debug << " || " << cntB << " @x(" << x << ")";
        cntAbove.push_back(cntA);
        cntBelow.push_back(cntB);
        if(il != 0)
        {
            //Vertex vertex = lowerEnvelope.vertex.at(il - 1);
            //debug << " || (" << vertex.first << ", " << vertex.second << ")";
        }
        //debug << endl;
    }

    //debug.close();
}

void SkyRider::cnt(RTree& tree, Coord x, Line& cur, size_t& cntAbove, size_t& cntBelow)
{
    Coord y0 = cur.first * x + cur.second;
    MinHeap heap;
    heap.push(MinHeapEntry(1, tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
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
                    Line line = make_pair(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                    if(cur.first == line.first && cur.second == line.second)
                        continue;
                    Coord y = line.first * x + line.second;
                    if(y > y0)
                        cntAbove++;
                    else if(y < y0)
                        cntBelow++;
                    else
                        assert(false);
                }
            }
        }
    }
}

bool SkyRider::find(Node_P nodePtr, Coord x, Coord y)
{
    MinHeap heap;
    heap.push(MinHeapEntry(1, nodePtr));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
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
                    Line line = make_pair(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                    if(line.first == x && line.second == y)
                        return true;
                }
            }
        }
    }
    return false;
}
*/
/*******************************************************************************
 * hull line
 ******************************************************************************/

void SkyRider::hull(Line_V& hullLine, Envelope& envelope, int turn)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());
    //if(turn == Util2D::LEFT_TURN)
        //debug << "===== lower hull of upper envelope =====" << endl;
    //else
        //debug << "===== upper hull of lower envelope =====" << endl;

    Vertex_V& vertex = envelope.vertex;
    Line_V& line = envelope.line;

    Coord xFrom = vertex.at(0).x - 1;
    Vertex begin;
    begin.x = xFrom;
    begin.y = Util2D::y(line.at(0), xFrom);
    Coord xTo = vertex.at(vertex.size() - 1).x + 1;
    Vertex end;
    end.x = xTo;
    end.y = Util2D::y(line.at(line.size() - 1), xTo);
    vertex.insert(vertex.begin(), begin);
    vertex.push_back(end);

    //for(size_t iv = 0; iv < vertex.size(); iv++)
        //debug << iv << " (" << vertex.at(iv).first << ", " << vertex.at(iv).second << ")" << endl;

    vector<int> pre;
    for(size_t iv = 0; iv < vertex.size(); iv++)
        pre.push_back(iv - 1);

    for(size_t iv = 2; iv < vertex.size(); iv++)
        while(!checkTurn(vertex, pre, iv, turn));

    size_t last = vertex.size() - 1;
    //debug << "pick:" << endl;
    while(pre.at(last) != 0)
    {
        Vertex& v = vertex.at(pre.at(last));
        Line primalL;
        primalL.a = -v.x;
        primalL.b = v.y;
        //debug << " " << pre.at(last) << "(" << v.first << ", " << v.second << ") ==> y = " << primalL.first << "x " << (primalL.second > 0 ? "+ " : "- ") << abs(primalL.second) << endl;
        hullLine.insert(hullLine.begin(), primalL);
        last = pre.at(last);
    }

    //debug.close();
}

bool SkyRider::checkTurn(Vertex_V& vertex, vector<int>& pre, size_t iv, int turn)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());

    if(pre.at(iv) == 0)
        return true;
    size_t i2 = pre.at(iv);
    size_t i1 = pre.at(i2);
    Vertex& v3 = vertex.at(iv);
    Vertex& v2 = vertex.at(i2);
    Vertex& v1 = vertex.at(i1);
    //debug << "check turn @ " << i1 << "(" << v1.first << ", " << v1.second << ") " << i2 << "(" << v2.first << ", " << v2.second << ") " << iv << "(" << v3.first << ", " << v3.second << ") " << flush;
    int getTurn = Util2D::turn(v1, v2, v3);
    //debug << getTurn = Util2D::turn(v1, v2, v3);
    if(getTurn != turn)
    {
        pre.at(iv) = i1;
        //debug << " ==> pre.at(" << iv << ") = " << i1 << endl;
        return false;
    }
    else
    {
        //debug << endl;
        return true;
    }

    //debug.close();
}
/*
void SkyRider::validateCandidateHull(RTree& tree, Line_V& upperHullLine, Line_V& lowerHullLine, size_t k)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());

    //debug << "===== upper hull line =====" << endl;
    for(size_t il = 0; il < upperHullLine.size(); il++)
    {
        Line line = upperHullLine.at(il);
        size_t cntA = 0, cntB = 0, cntOn = 0;
        cntP(tree, line, cntA, cntB, cntOn);
        //debug << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << " || "<< cntA << " || " << cntOn << " || " << cntB << endl;
    }

    //debug << "===== lower hull line =====" << endl;
    for(size_t il = 0; il < lowerHullLine.size(); il++)
    {
        Line line = lowerHullLine.at(il);
        size_t cntA = 0, cntB = 0, cntOn = 0;
        cntP(tree, line, cntA, cntB, cntOn);
        //debug << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << " || "<< cntA << " || " << cntOn << " || " << cntB << endl;
    }

    //debug.close();
}

void SkyRider::cntP(RTree& tree, Line& line, size_t& cntAbove, size_t& cntBelow, size_t& cntOn)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(1, tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
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
                    Vertex data = make_pair(entryPtr->mbre.coord[0][0], entryPtr->mbre.coord[1][0]);
                    Coord y = line.first * data.first + line.second;
                    //if(abs(y - data.second) < 1)
                        //debug << "diff(y) " << abs(y - data.second) << endl;
                    if(abs(y - data.second) < 1)
                        cntOn++;
                    else if(y > data.second)
                        cntAbove++;
                    else if(y < data.second)
                        cntBelow++;
                }
            }
        }
    }

    //debug.close();
}
*/
/*******************************************************************************
 * contour
 ******************************************************************************/

void SkyRider::constructContour(KDepthContour& contour, Line_V& upperHullLine, Line_V& lowerHullLine)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());

    size_t iUpperStart = 0, iUpperEnd = upperHullLine.size() - 1;
    size_t iLowerStart = 0, iLowerEnd = lowerHullLine.size() - 1;

    Vertex upper, lower, cross;
    bool found;
    do
    {
        found = false;
        Util2D::lineCross(upperHullLine.at(iUpperStart), upperHullLine.at(iUpperStart + 1), upper);
        Util2D::lineCross(lowerHullLine.at(iLowerEnd), lowerHullLine.at(iLowerEnd - 1), lower);
        Util2D::lineCross(upperHullLine.at(iUpperStart), lowerHullLine.at(iLowerEnd), cross);
        if(cross.x > upper.x - EPS)
        {
            iUpperStart++;
            found = true;
        }
        if(cross.x > lower.x - EPS)
        {
            iLowerEnd--;
            found = true;
        }
    }while(found);
    do
    {
        found = false;
        Util2D::lineCross(upperHullLine.at(iUpperEnd), upperHullLine.at(iUpperEnd - 1), upper);
        Util2D::lineCross(lowerHullLine.at(iLowerStart), lowerHullLine.at(iLowerStart + 1), lower);
        Util2D::lineCross(upperHullLine.at(iUpperEnd), lowerHullLine.at(iLowerStart), cross);
        if(cross.x < upper.x + EPS)
        {
            iUpperEnd--;
            found = true;
        }
        if(cross.x < lower.x + EPS)
        {
            iLowerStart++;
            found = true;
        }
    }while(found);

    //debug << "===== prune candidate hull line =====" << endl;
    //debug << "iUpperStart " << iUpperStart << " iUpperEnd " << iUpperEnd << "(" << upperHullLine.size() - 1 << ")" << endl;
    //debug << "iLowerStart " << iLowerStart << " iLowerEnd " << iLowerEnd << "(" << lowerHullLine.size() - 1 << ")" << endl;

    for(size_t il = iUpperStart; il <= iUpperEnd; il++)
        contour.upperHullLine.push_back(upperHullLine.at(il));
    for(size_t il = iLowerStart; il <= iLowerEnd; il++)
        contour.lowerHullLine.push_back(lowerHullLine.at(il));

    //debug.close();
}
/*
void SkyRider::validateContour(KDepthContour& contour)
{
    //ofstream debug;
    //debug.open ("data/debug/debug.txt", ios::out | ios::app );
    //assert(debug.is_open());

    //debug << "===== upper hull =====" << endl;
    Line cur, next;
    Vertex from, to;
    from = make_pair(0, Util2D::y(contour.upperHullLine.at(0), 0));
    for(size_t il = 0; il < contour.upperHullLine.size() - 1; il++)
    {
        cur = contour.upperHullLine.at(il);
        next = contour.upperHullLine.at(il + 1);
        Util2D::lineCross(cur, next, to);
        //debug << "y = " << cur.first << "x " << (cur.second > 0 ? "+ " : "- ") << abs(cur.second) << " from(" << from.first << ", " << from.second << ") to(" << to.first << ", " << to.second << ")" << endl;
        assert(to.first + EPS > from.first);
        from = to;
    }
    cur = contour.upperHullLine.at(contour.upperHullLine.size() - 1);
    next = contour.lowerHullLine.at(0);
    Util2D::lineCross(cur, next, to);
    //debug << "y = " << cur.first << "x " << (cur.second > 0 ? "+ " : "- ") << abs(cur.second) << " from(" << from.first << ", " << from.second << ") to(" << to.first << ", " << to.second << ")" << endl;
    assert(to.first + EPS > from.first);
    from = to;

    //debug << "===== lower hull =====" << endl;
    for(size_t il = 0; il < contour.lowerHullLine.size() - 1; il++)
    {
        cur = contour.lowerHullLine.at(il);
        next = contour.lowerHullLine.at(il + 1);
        Util2D::lineCross(cur, next, to);
        //debug << "y = " << cur.first << "x " << (cur.second > 0 ? "+ " : "- ") << abs(cur.second) << " from(" << from.first << ", " << from.second << ") to(" << to.first << ", " << to.second << ")" << endl;
        assert(to.first < from.first + EPS);
        from = to;
    }
    cur = contour.lowerHullLine.at(contour.lowerHullLine.size() - 1);
    next = contour.upperHullLine.at(0);
    Util2D::lineCross(cur, next, to);
    //debug << "y = " << cur.first << "x " << (cur.second > 0 ? "+ " : "- ") << abs(cur.second) << " from(" << from.first << ", " << from.second << ") to(" << to.first << ", " << to.second << ")" << endl;
    assert(to.first < from.first + EPS);
    from = to;

    //debug.close();
}
*/

