#include "ConHull.h"

bool ConHull::convexHull(RStarTree& tree, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    if(!init(tree, hull))
        return false;

    MaxHeap heap;
    heap.push(MaxHeapEntry(0, tree.root));
    while(!heap.isEmpty())
    {
        MaxHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    Coord dis = maxDis(childPtr->mbrn, hull);
                    //chfc << "[" << childPtr->mbrn.coord[0][0] << ", " << childPtr->mbrn.coord[0][1] << "] [" << childPtr->mbrn.coord[1][0] << ", " << childPtr->mbrn.coord[1][1] << "] " << dis << endl;
                    if(dis > EPS)
                        heap.push(MaxHeapEntry(dis, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    Vertex vertex;
                    vertex.x = entryPtr->mbre.coord[0][0];
                    vertex.y = entryPtr->mbre.coord[1][0];
                    Coord dis = maxDis(vertex, hull);
                    //chfc << "(" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ") " << dis << endl;
                    if(dis > EPS)
                        heap.push(MaxHeapEntry(dis, entryPtr));
                }
            }
        }
        else
        {
            Vertex vertex;
            vertex.x = e.entryPtr->mbre.coord[0][0];
            vertex.y = e.entryPtr->mbre.coord[1][0];
            unionV(hull, vertex);
        }
    }

    //chfc.close();
    return true;
}

bool ConHull::convexHull(RStarTree& tree, ConvexHull& hull, ConvexHull& outer)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    if(!init(tree, hull, outer))
        return false;

    MaxHeap heap;
    heap.push(MaxHeapEntry(0, tree.root));
    while(!heap.isEmpty())
    {
        MaxHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    Coord dis = maxDis(childPtr->mbrn, hull);
                    if(dis > EPS)
                        heap.push(MaxHeapEntry(dis, childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    Vertex vertex;
                    vertex.x = entryPtr->mbre.coord[0][0];
                    vertex.y = entryPtr->mbre.coord[1][0];
                    if(exclude(outer, vertex))
                        continue;
                    Coord dis = maxDis(vertex, hull);
                    if(dis > EPS)
                        heap.push(MaxHeapEntry(dis, entryPtr));
                }
            }
        }
        else
        {
            Vertex vertex;
            vertex.x = e.entryPtr->mbre.coord[0][0];
            vertex.y = e.entryPtr->mbre.coord[1][0];
            unionV(hull, vertex);
        }
    }

    //chfc.close();
    return true;
}

/*******************************************************************************
 * convex hull
 ******************************************************************************/

bool ConHull::init(RStarTree& tree, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    if(!minX(tree, hull))
        return false;//chfc << "minX (" << hull.minX.first << ", " << hull.minX.second << ")" << endl;
    if(!minY(tree, hull))
        return false;//chfc << "minY (" << hull.minY.first << ", " << hull.minY.second << ")" << endl;
    if(!maxX(tree, hull))
        return false;//chfc << "maxX (" << hull.maxX.first << ", " << hull.maxX.second << ")" << endl;
    if(!maxY(tree, hull))
        return false;//chfc << "maxY (" << hull.maxY.first << ", " << hull.maxY.second << ")" << endl;

    Line line;

    hull.upperLeftHullVertex.push_back(hull.minX);
    hull.upperLeftHullVertex.push_back(hull.maxY);
    Util2D::linePass(hull.minX, hull.maxY, line);
    hull.upperLeftHullLine.push_back(line);

    hull.upperRightHullVertex.push_back(hull.maxY);
    hull.upperRightHullVertex.push_back(hull.maxX);
    Util2D::linePass(hull.maxY, hull.maxX, line);
    hull.upperRightHullLine.push_back(line);

    hull.lowerRightHullVertex.push_back(hull.maxX);
    hull.lowerRightHullVertex.push_back(hull.minY);
    Util2D::linePass(hull.maxX, hull.minY, line);
    hull.lowerRightHullLine.push_back(line);

    hull.lowerLeftHullVertex.push_back(hull.minY);
    hull.lowerLeftHullVertex.push_back(hull.minX);
    Util2D::linePass(hull.minY, hull.minX, line);
    hull.lowerLeftHullLine.push_back(line);

    //chfc.close();
    return true;
}

bool ConHull::minX(RStarTree& tree, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(tree.root->mbrn.coord[0][0], tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
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
            hull.minX.x = e.entryPtr->mbre.coord[0][0];
            hull.minX.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

bool ConHull::minY(RStarTree& tree, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(tree.root->mbrn.coord[1][0], tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(childPtr->mbrn.coord[1][0], childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    heap.push(MinHeapEntry(entryPtr->mbre.coord[1][0], entryPtr));
                }
            }
        }
        else
        {
            hull.minY.x = e.entryPtr->mbre.coord[0][0];
            hull.minY.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

bool ConHull::maxX(RStarTree& tree, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MaxHeap heap;
    heap.push(MaxHeapEntry(tree.root->mbrn.coord[0][1], tree.root));
    while(!heap.isEmpty())
    {
        MaxHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
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
            hull.maxX.x = e.entryPtr->mbre.coord[0][0];
            hull.maxX.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

bool ConHull::maxY(RStarTree& tree, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MaxHeap heap;
    heap.push(MaxHeapEntry(tree.root->mbrn.coord[1][1], tree.root));
    while(!heap.isEmpty())
    {
        MaxHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MaxHeapEntry(childPtr->mbrn.coord[1][1], childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    heap.push(MaxHeapEntry(entryPtr->mbre.coord[1][1], entryPtr));
                }
            }
        }
        else
        {
            hull.maxY.x = e.entryPtr->mbre.coord[0][0];
            hull.maxY.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

Coord ConHull::maxDis(Mbr& mbr, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    Coord maxDis = -1;

    if(mbr.coord[0][0] - EPS < hull.maxY.x && mbr.coord[1][1] + EPS > hull.minX.y)
    {
        Vertex vertex;
        vertex.x = mbr.coord[0][0];
        vertex.y = mbr.coord[1][1];
        for(size_t il = 0; il < hull.upperLeftHullLine.size(); il++)
        {
            Line& line = hull.upperLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_ABOVE)
                maxDis = max(maxDis, Util2D::distance2(line, hull.upperLeftHullVertex.at(il), hull.upperLeftHullVertex.at(il + 1), vertex));
        }
    }

    if(mbr.coord[0][1] + EPS > hull.maxY.x && mbr.coord[1][1] + EPS > hull.maxX.y)
    {
        Vertex vertex;
        vertex.x = mbr.coord[0][1];
        vertex.y = mbr.coord[1][1];
        for(size_t il = 0; il < hull.upperRightHullLine.size(); il++)
        {
            Line& line = hull.upperRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_ABOVE)
                maxDis = max(maxDis, Util2D::distance2(line, hull.upperRightHullVertex.at(il), hull.upperRightHullVertex.at(il + 1), vertex));
        }
    }

    if(mbr.coord[0][1] + EPS > hull.minY.x && mbr.coord[1][0] - EPS < hull.maxX.y)
    {
        Vertex vertex;
        vertex.x = mbr.coord[0][1];
        vertex.y = mbr.coord[1][0];
        for(size_t il = 0; il < hull.lowerRightHullLine.size(); il++)
        {
            Line& line = hull.lowerRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_BELOW)
                maxDis = max(maxDis, Util2D::distance2(line, hull.lowerRightHullVertex.at(il), hull.lowerRightHullVertex.at(il + 1), vertex));
        }
    }

    if(mbr.coord[0][0] - EPS < hull.minY.x && mbr.coord[1][0] - EPS < hull.minX.y)
    {
        Vertex vertex;
        vertex.x = mbr.coord[0][0];
        vertex.y = mbr.coord[1][0];
        for(size_t il = 0; il < hull.lowerLeftHullLine.size(); il++)
        {
            Line& line = hull.lowerLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_BELOW)
                maxDis = max(maxDis, Util2D::distance2(line, hull.lowerLeftHullVertex.at(il), hull.lowerLeftHullVertex.at(il + 1), vertex));
        }
    }

    //chfc.close();

    return maxDis;
}

Coord ConHull::maxDis(Vertex& vertex, ConvexHull& hull)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    Coord maxDis = -1;

    if(vertex.x - EPS < hull.maxY.x && vertex.y + EPS > hull.minX.y)
        for(size_t il = 0; il < hull.upperLeftHullLine.size(); il++)
        {
            Line& line = hull.upperLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_ABOVE)
                maxDis = max(maxDis, Util2D::distance2(line, hull.upperLeftHullVertex.at(il), hull.upperLeftHullVertex.at(il + 1), vertex));
        }

    if(vertex.x + EPS > hull.maxY.x && vertex.y + EPS > hull.maxX.y)
        for(size_t il = 0; il < hull.upperRightHullLine.size(); il++)
        {
            Line& line = hull.upperRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_ABOVE)
                maxDis = max(maxDis, Util2D::distance2(line, hull.upperRightHullVertex.at(il), hull.upperRightHullVertex.at(il + 1), vertex));
        }

    if(vertex.x + EPS > hull.minY.x && vertex.y - EPS < hull.maxX.y)
        for(size_t il = 0; il < hull.lowerRightHullLine.size(); il++)
        {
            Line& line = hull.lowerRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_BELOW)
                maxDis = max(maxDis, Util2D::distance2(line, hull.lowerRightHullVertex.at(il), hull.lowerRightHullVertex.at(il + 1), vertex));
        }

    if(vertex.x - EPS < hull.minY.x && vertex.y - EPS < hull.minX.y)
        for(size_t il = 0; il < hull.lowerLeftHullLine.size(); il++)
        {
            Line& line = hull.lowerLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND)
                maxDis = 0;
            else if(status == Util2D::STRICT_BELOW)
                maxDis = max(maxDis, Util2D::distance2(line, hull.lowerLeftHullVertex.at(il), hull.lowerLeftHullVertex.at(il + 1), vertex));
        }

    //chfc.close();

    return maxDis;
}

/*******************************************************************************
 * k-th convex hull, k != 1
 ******************************************************************************/

bool ConHull::init(RStarTree& tree, ConvexHull& hull, ConvexHull& outer)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    if(!minX(tree, hull, outer))
        return false;//chfc << "minX (" << hull.minX.first << ", " << hull.minX.second << ")" << endl;
    if(!minY(tree, hull, outer))
        return false;//chfc << "minY (" << hull.minY.first << ", " << hull.minY.second << ")" << endl;
    if(!maxX(tree, hull, outer))
        return false;//chfc << "maxX (" << hull.maxX.first << ", " << hull.maxX.second << ")" << endl;
    if(!maxY(tree, hull, outer))
        return false;//chfc << "maxY (" << hull.maxY.first << ", " << hull.maxY.second << ")" << endl;

    Line line;

    hull.upperLeftHullVertex.push_back(hull.minX);
    hull.upperLeftHullVertex.push_back(hull.maxY);
    Util2D::linePass(hull.minX, hull.maxY, line);
    hull.upperLeftHullLine.push_back(line);

    hull.upperRightHullVertex.push_back(hull.maxY);
    hull.upperRightHullVertex.push_back(hull.maxX);
    Util2D::linePass(hull.maxY, hull.maxX, line);
    hull.upperRightHullLine.push_back(line);

    hull.lowerRightHullVertex.push_back(hull.maxX);
    hull.lowerRightHullVertex.push_back(hull.minY);
    Util2D::linePass(hull.maxX, hull.minY, line);
    hull.lowerRightHullLine.push_back(line);

    hull.lowerLeftHullVertex.push_back(hull.minY);
    hull.lowerLeftHullVertex.push_back(hull.minX);
    Util2D::linePass(hull.minY, hull.minX, line);
    hull.lowerLeftHullLine.push_back(line);

    //chfc.close();
    return true;
}

bool ConHull::minX(RStarTree& tree, ConvexHull& hull, ConvexHull& outer)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(tree.root->mbrn.coord[0][0], tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(childPtr->mbrn.coord[0][0], childPtr));
                    //chfc << "push [" << childPtr->mbrn.coord[0][0] << ", " << childPtr->mbrn.coord[0][1] << "] [" << childPtr->mbrn.coord[1][0] << ", " << childPtr->mbrn.coord[1][1] << "]" << endl;
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    Vertex vertex;
                    vertex.x = entryPtr->mbre.coord[0][0];
                    vertex.y = entryPtr->mbre.coord[1][0];
                    if(exclude(outer, vertex))
                    {
                        //chfc << "skip (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                        continue;
                    }
                    heap.push(MinHeapEntry(entryPtr->mbre.coord[0][0], entryPtr));
                    //chfc << "push (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                }
            }
        }
        else
        {
            hull.minX.x = e.entryPtr->mbre.coord[0][0];
            hull.minX.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

bool ConHull::minY(RStarTree& tree, ConvexHull& hull, ConvexHull& outer)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MinHeap heap;
    heap.push(MinHeapEntry(tree.root->mbrn.coord[1][0], tree.root));
    while(!heap.isEmpty())
    {
        MinHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MinHeapEntry(childPtr->mbrn.coord[1][0], childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    Vertex vertex;
                    vertex.x = entryPtr->mbre.coord[0][0];
                    vertex.y = entryPtr->mbre.coord[1][0];
                    if(exclude(outer, vertex))
                    {
                        //chfc << "skip (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                        continue;
                    }
                    heap.push(MinHeapEntry(entryPtr->mbre.coord[1][0], entryPtr));
                    //chfc << "push (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                }
            }
        }
        else
        {
            hull.minY.x = e.entryPtr->mbre.coord[0][0];
            hull.minY.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

bool ConHull::maxX(RStarTree& tree, ConvexHull& hull, ConvexHull& outer)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MaxHeap heap;
    heap.push(MaxHeapEntry(tree.root->mbrn.coord[0][1], tree.root));
    while(!heap.isEmpty())
    {
        MaxHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
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
                    Vertex vertex;
                    vertex.x = entryPtr->mbre.coord[0][0];
                    vertex.y = entryPtr->mbre.coord[1][0];
                    if(exclude(outer, vertex))
                    {
                        //chfc << "skip (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                        continue;
                    }
                    heap.push(MaxHeapEntry(entryPtr->mbre.coord[0][1], entryPtr));
                    //chfc << "push (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                }
            }
        }
        else
        {
            hull.maxX.x = e.entryPtr->mbre.coord[0][0];
            hull.maxX.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

bool ConHull::maxY(RStarTree& tree, ConvexHull& hull, ConvexHull& outer)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    MaxHeap heap;
    heap.push(MaxHeapEntry(tree.root->mbrn.coord[1][1], tree.root));
    while(!heap.isEmpty())
    {
        MaxHeapEntry e = heap.pop();
        if(e.nodePtr)
        {
            Argument::FIOchfc++;
            if(e.nodePtr->level)
            {
                Node_P_V& children = *e.nodePtr->children;
                for(size_t ic = 0; ic < children.size(); ic++)
                {
                    Node_P childPtr = children.at(ic);
                    heap.push(MaxHeapEntry(childPtr->mbrn.coord[1][1], childPtr));
                }
            }
            else
            {
                Entry_P_V& entries = *e.nodePtr->entries;
                for(size_t ie = 0; ie < entries.size(); ie++)
                {
                    Entry_P entryPtr = entries.at(ie);
                    Vertex vertex;
                    vertex.x = entryPtr->mbre.coord[0][0];
                    vertex.y = entryPtr->mbre.coord[1][0];
                    if(exclude(outer, vertex))
                    {
                        //chfc << "skip (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                        continue;
                    }
                    heap.push(MaxHeapEntry(entryPtr->mbre.coord[1][1], entryPtr));
                    //chfc << "push (" << entryPtr->mbre.coord[0][0] << ", " << entryPtr->mbre.coord[1][0] << ")" << endl;
                }
            }
        }
        else
        {
            hull.maxY.x = e.entryPtr->mbre.coord[0][0];
            hull.maxY.y = e.entryPtr->mbre.coord[1][0];
            //chfc.close();
            return true;
        }
    }

    //chfc.close();
    return false;
}

bool ConHull::exclude(ConvexHull& outer, Vertex& vertex)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());

    if(vertex.x - EPS < outer.maxY.x && vertex.y + EPS > outer.minX.y)
        for(size_t il = 0; il < outer.upperLeftHullLine.size(); il++)
        {
            Line& line = outer.upperLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND || status == Util2D::STRICT_ABOVE)
            {
                //Vertex v1 = outer.upperLeftHullVertex.at(il);
                //Vertex v2 = outer.upperLeftHullVertex.at(il + 1);
                //chfc << (status == Util2D::AROUND ? "around " : "") << (status == Util2D::STRICT_ABOVE ? "above " : "") << (status == Util2D::STRICT_BELOW ? "below " : "") << "line " << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << " (" << v1.first << ", " << v1.second << ") (" << v2.first << ", " << v2.second << ")" << endl;
                //chfc.close();
                return true;
            }
        }

    if(vertex.x + EPS > outer.maxY.x && vertex.y + EPS > outer.maxX.y)
        for(size_t il = 0; il < outer.upperRightHullLine.size(); il++)
        {
            Line& line = outer.upperRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND || status == Util2D::STRICT_ABOVE)
            {
                //Vertex v1 = outer.upperRightHullVertex.at(il);
                //Vertex v2 = outer.upperRightHullVertex.at(il + 1);
                //chfc << (status == Util2D::AROUND ? "around " : "") << (status == Util2D::STRICT_ABOVE ? "above " : "") << (status == Util2D::STRICT_BELOW ? "below " : "") << "line " << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << " (" << v1.first << ", " << v1.second << ") (" << v2.first << ", " << v2.second << ")" << endl;
                //chfc.close();
                return true;
            }
        }

    if(vertex.x + EPS > outer.minY.x && vertex.y - EPS < outer.maxX.y)
        for(size_t il = 0; il < outer.lowerRightHullLine.size(); il++)
        {
            Line& line = outer.lowerRightHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND || status == Util2D::STRICT_BELOW)
            {
                //Vertex v1 = outer.lowerRightHullVertex.at(il);
                //Vertex v2 = outer.lowerRightHullVertex.at(il + 1);
                //chfc << (status == Util2D::AROUND ? "around " : "") << (status == Util2D::STRICT_ABOVE ? "above " : "") << (status == Util2D::STRICT_BELOW ? "below " : "") << "line " << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << " (" << v1.first << ", " << v1.second << ") (" << v2.first << ", " << v2.second << ")" << endl;
                //chfc.close();
                return true;
            }
        }

    if(vertex.x - EPS < outer.minY.x && vertex.y - EPS < outer.minX.y)
        for(size_t il = 0; il < outer.lowerLeftHullLine.size(); il++)
        {
            Line& line = outer.lowerLeftHullLine.at(il);
            int status = Util2D::isAboveLine(line, vertex);
            if(status == Util2D::AROUND || status == Util2D::STRICT_BELOW)
            {
                //Vertex v1 = outer.lowerLeftHullVertex.at(il);
                //Vertex v2 = outer.lowerLeftHullVertex.at(il + 1);
                //chfc << (status == Util2D::AROUND ? "around " : "") << (status == Util2D::STRICT_ABOVE ? "above " : "") << (status == Util2D::STRICT_BELOW ? "below " : "") << "line " << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << " (" << v1.first << ", " << v1.second << ") (" << v2.first << ", " << v2.second << ")" << endl;
                //chfc.close();
                return true;
            }
        }

    //chfc.close();

    return false;
}

/*******************************************************************************
 * union
 ******************************************************************************/

void ConHull::unionV(ConvexHull& hull, Vertex& vertex)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());
    //chfc << "union vertex (" << vertex.first << ", " << vertex.second << ")" << endl;

    if(vertex.x - EPS < hull.maxY.x && vertex.y + EPS > hull.minX.y)
        unionVUpper(hull.upperLeftHullVertex, hull.upperLeftHullLine, vertex);

    if(vertex.x + EPS > hull.maxY.x && vertex.y + EPS > hull.maxX.y)
        unionVUpper(hull.upperRightHullVertex, hull.upperRightHullLine, vertex);

    if(vertex.x + EPS > hull.minY.x && vertex.y - EPS < hull.maxX.y)
        unionVLower(hull.lowerRightHullVertex, hull.lowerRightHullLine, vertex);

    if(vertex.x - EPS < hull.minY.x && vertex.y - EPS < hull.minX.y)
        unionVLower(hull.lowerLeftHullVertex, hull.lowerLeftHullLine, vertex);

    //chfc.close();
}

void ConHull::unionVUpper(Vertex_V& upperHullVertex, Line_V& upperHullLine, Vertex& vertex)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());
    //chfc << "upper hull vertex" << endl;
    //for(size_t iv = 0; iv < upperHullVertex.size(); iv++)
        //chfc << "(" << upperHullVertex.at(iv).first << ", " << upperHullVertex.at(iv).second << ")" << endl;

    Vertex_V hullVertex;
    bool pre = false;
    bool insert = false;
    for(size_t il = 0; il < upperHullLine.size(); il++)
    {
        bool cur = (Util2D::isAboveLine(upperHullLine.at(il), vertex) == Util2D::STRICT_ABOVE);
        if(pre && cur)
        {
            // do nothing
        }
        else
            hullVertex.push_back(upperHullVertex.at(il));
        if(cur && !insert)
        {
            hullVertex.push_back(vertex);
            insert = true;
        }
        pre = cur;
    }
    hullVertex.push_back(upperHullVertex.at(upperHullVertex.size() - 1));

    Line_V hullLine;
    for(size_t iv = 1; iv < hullVertex.size(); iv++)
    {
        Vertex pre = hullVertex.at(iv - 1);
        Vertex next = hullVertex.at(iv);
        Line line;
        Util2D::linePass(pre, next, line);
        hullLine.push_back(line);
    }

    upperHullLine.swap(hullLine);
    upperHullVertex.swap(hullVertex);

    //chfc << "upper hull vertex" << endl;
    //for(size_t iv = 0; iv < upperHullVertex.size(); iv++)
        //chfc << "(" << upperHullVertex.at(iv).first << ", " << upperHullVertex.at(iv).second << ")" << endl;
    //chfc << "upper hull line" << endl;
    //for(size_t il = 0; il < upperHullLine.size(); il++)
    //{
        //Line line = upperHullLine.at(il);
        //chfc << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << endl;
    //}


    //chfc.close();
}

void ConHull::unionVLower(Vertex_V& lowerHullVertex, Line_V& lowerHullLine, Vertex& vertex)
{
    //ofstream chfc;
    //chfc.open ("data/debug/chfc.txt", ios::out | ios::app );
    //assert(chfc.is_open());
    //chfc << "lower hull vertex" << endl;
    //for(size_t iv = 0; iv < lowerHullVertex.size(); iv++)
        //chfc << "(" << lowerHullVertex.at(iv).first << ", " << lowerHullVertex.at(iv).second << ")" << endl;

    Vertex_V hullVertex;
    bool pre = false;
    bool insert = false;
    for(size_t il = 0; il < lowerHullLine.size(); il++)
    {
        bool cur = (Util2D::isAboveLine(lowerHullLine.at(il), vertex) == Util2D::STRICT_BELOW);
        if(pre && cur)
        {
            // do nothing
        }
        else
            hullVertex.push_back(lowerHullVertex.at(il));
        if(cur && !insert)
        {
            hullVertex.push_back(vertex);
            insert = true;
        }
        pre = cur;
    }
    hullVertex.push_back(lowerHullVertex.at(lowerHullVertex.size() - 1));

    Line_V hullLine;
    for(size_t iv = 1; iv < hullVertex.size(); iv++)
    {
        Vertex pre = hullVertex.at(iv - 1);
        Vertex next = hullVertex.at(iv);
        Line line;
        Util2D::linePass(pre, next, line);
        hullLine.push_back(line);
    }

    lowerHullLine.swap(hullLine);
    lowerHullVertex.swap(hullVertex);

    //chfc << "lower hull vertex" << endl;
    //for(size_t iv = 0; iv < lowerHullVertex.size(); iv++)
        //chfc << "(" << lowerHullVertex.at(iv).first << ", " << lowerHullVertex.at(iv).second << ")" << endl;
    //chfc << "lower hull line" << endl;
    //for(size_t il = 0; il < lowerHullLine.size(); il++)
    //{
        //Line line = lowerHullLine.at(il);
        //chfc << "y = " << line.first << "x " << (line.second > 0 ? "+ " : "- ") << abs(line.second) << endl;
    //}

    //chfc.close();
}
