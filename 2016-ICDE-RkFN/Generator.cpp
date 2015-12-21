#include "Generator.h"

/*******************************************************************************
 * FSUNode
 ******************************************************************************/

istream& operator>>(istream& in, FSUNode& n)
{
    double nodeID;
    in >> nodeID;
    in >> n.longitude >> n.latitude;
    return in;
}

bool FSUNode::operator< (const FSUNode& n) const
{
    if(longitude == n.longitude)
        return latitude < n.latitude;
    return longitude < n.longitude;
}

/*******************************************************************************
 * FSUPoint
 ******************************************************************************/

istream& operator>>(istream& in, FSUPoint& p)
{
    double categoryID;
    in >> p.longitude >> p.latitude;
    in >> categoryID;
    return in;
}

bool FSUPoint::operator< (const FSUPoint& p) const
{
    if(longitude == p.longitude)
        return latitude < p.latitude;
    return longitude < p.longitude;
}

/*******************************************************************************
 * Point2D
 ******************************************************************************/

Point2D::Point2D()
{
}

Point2D::Point2D(double x, double y)
    : x(x), y(y)
{
}

istream& operator>>(istream& in, Point2D& p)
{
    in >> p.x >> p.y;
    return in;
}

ostream& operator<<(ostream& out, const Point2D& p)
{
    out << p.x << " " << p.y << endl;
    return out;
}

bool Point2D::operator< (const Point2D& p) const
{
    if(x == p.x)
        return y < p.y;
    return x < p.x;
}

/*******************************************************************************
 * RealDataGenerator
 ******************************************************************************/

RealDataGenerator::RealDataGenerator()
{
    srand((unsigned)time(0));
}

void RealDataGenerator::genRealData()
{
    points.clear();

    readCalNode();
    duplicateRemove(calNodes);
    normalize(calNodes);
    duplicateDetect(calNodes);

    readCalPoint();
    duplicateRemove(calPoints);
    normalize(calPoints);
    duplicateDetect(calPoints);

    readNANode();
    duplicateRemove(naNodes);
    normalize(naNodes);
    duplicateDetect(naNodes);

    readSFNode();
    duplicateRemove(sfNodes);
    normalize(sfNodes);
    duplicateDetect(sfNodes);

    writeRealData();
    check();
}

void RealDataGenerator::pickFC(size_t nf, size_t nc, size_t nq)
{
    readRealData();

    vector<Point2D> facilityLeft;
    pick(points, facilities, facilityLeft, nf);
    string facilityFileName = Argument::getFacilityFileName();
    ofstream facilityFileOFS(facilityFileName.c_str());
    writePoints(facilityFileOFS, facilities, facilityFileName);

    vector<Point2D> clientLeft;
    pick(facilityLeft, clients, clientLeft, nc);
    string clientFileName = Argument::getClientFileName();
    ofstream clientFileOFS(clientFileName.c_str());
    writePoints(clientFileOFS, clients, clientFileName);

    vector<Point2D> queryLeft;
    pick(clientLeft, queries, queryLeft, nq);
    string queryFileName = Argument::getQueryFileName();
    ofstream queryFileOFS(queryFileName.c_str());
    writePoints(queryFileOFS, queries, queryFileName);
}

void RealDataGenerator::genUniData(size_t total)
{
    for(size_t iq = 0; iq < total; iq++)
    {
        Coord point[DIM];
        for(size_t dim = 0; dim < DIM; dim++)
            point[dim] = 1e-3 + getRandom() * (NORMALIZE_SCALE - 1);
        Point2D p(point[0], point[1]);
        points.push_back(p);
    }

    writeUniData();
    check();
}

void RealDataGenerator::pickUni(size_t nf, size_t nc, size_t nq)
{
    readUniData();

    vector<Point2D> facilityLeft;
    pick(points, facilities, facilityLeft, nf);
    string facilityFileName = Argument::getFacilityFileName();
    ofstream facilityFileOFS(facilityFileName.c_str());
    writePoints(facilityFileOFS, facilities, facilityFileName);

    vector<Point2D> clientLeft;
    pick(facilityLeft, clients, clientLeft, nc);
    string clientFileName = Argument::getClientFileName();
    ofstream clientFileOFS(clientFileName.c_str());
    writePoints(clientFileOFS, clients, clientFileName);

    vector<Point2D> queryLeft;
    pick(clientLeft, queries, queryLeft, nq);
    string queryFileName = Argument::getQueryFileName();
    ofstream queryFileOFS(queryFileName.c_str());
    writePoints(queryFileOFS, queries, queryFileName);
}

void RealDataGenerator::genUniCData(size_t total)
{
    Coord mid = NORMALIZE_SCALE / 2;
    for(size_t iq = 0; iq < total; iq++)
    {
        while(true)
        {
            Coord point[DIM];
            Coord diff = 0, dis = 0;
            for(size_t dim = 0; dim < DIM; dim++)
            {
                point[dim] = 1e-3 + getRandom() * (NORMALIZE_SCALE - 1);
                diff = point[dim] - mid;
                dis += diff * diff;
            }
            if(sqrt(dis) > mid)
                continue;
            Point2D p(point[0], point[1]);
            points.push_back(p);
            break;
        }
    }

    writeUniCData();
    check();
}

void RealDataGenerator::pickUniC(size_t nf, size_t nc, size_t nq)
{
    readUniCData();

    vector<Point2D> facilityLeft;
    pick(points, facilities, facilityLeft, nf);
    string facilityFileName = Argument::getFacilityFileName();
    ofstream facilityFileOFS(facilityFileName.c_str());
    writePoints(facilityFileOFS, facilities, facilityFileName);

    vector<Point2D> clientLeft;
    pick(facilityLeft, clients, clientLeft, nc);
    string clientFileName = Argument::getClientFileName();
    ofstream clientFileOFS(clientFileName.c_str());
    writePoints(clientFileOFS, clients, clientFileName);

    vector<Point2D> queryLeft;
    pick(clientLeft, queries, queryLeft, nq);
    string queryFileName = Argument::getQueryFileName();
    ofstream queryFileOFS(queryFileName.c_str());
    writePoints(queryFileOFS, queries, queryFileName);
}

void RealDataGenerator::vdca()
{
    Point query(99999.9, 99999.9);
    RStarTree band;
    BBS::circularkSkyband(band, *Data::fTree, NORMALIZE_SCALE, Argument::k, query);
    KDepthContour kdc;
    SkyRider::kDepthContour(band, kdc, Argument::k);

    Coord mid = NORMALIZE_SCALE / 2;

    // percentage of area outside dck : cntDC / cntRp
    for(size_t iq = 0; iq < 1000000; iq++)
    {
        while(true)
        {
            Coord point[DIM];
            Coord diff = 0, dis = 0;
            for(size_t dim = 0; dim < DIM; dim++)
            {
                point[dim] = 1e-3 + getRandom() * (NORMALIZE_SCALE - 1);
                diff = point[dim] - mid;
                dis += diff * diff;
            }
            if(sqrt(dis) > mid)
                continue;
            Argument::cntRp++;
            Mbr mbr(point);
            if(DCCR::contain(kdc, mbr) == 0)
                Argument::cntDC++;
            break;
        }
    }

    // percentage of area expected outside dck : expDCperR
    double a = 3.87031, b = -11.09062, c = 0.93407;
    double right = (2 * Argument::k * M_PI) / Argument::numFacilities;
    double exp = log(right);
    double theta = b / (exp - a) - c;
    double htheta = theta / 2;
    double cosh = cos(htheta);
    Argument::expDCperR = 1 - cosh * cosh;

    // percentage of area expected2 outside dck : expDCperR2
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
    theta = 0.5 * (theta1 + theta2);
    htheta = theta / 2;
    cosh = cos(htheta);
    Argument::expDCperR2 = 1 - cosh * cosh;
}

void RealDataGenerator::rstQ()
{
    Point query(99999.9, 99999.9);
    Vertex q;
    q.x = 99999.9;
    q.y = 99999.9;
    RStarTree band;
    BBS::circularkSkyband(band, *Data::fTree, NORMALIZE_SCALE, Argument::k, query);
    KDepthContour kdc;
    SkyRider::kDepthContour(band, kdc, Argument::k);
    RStarTree skirt;
    RadiansPoint_V skirt_v;
    DCCR::outerSkirt(band, skirt, skirt_v, kdc, q);

    vector<Point2D> rstq;
    if(Argument::dataset == "unic")
    {
        Coord mid = NORMALIZE_SCALE / 2;
        for(size_t iq = 0; iq < Argument::numQueries; iq++)
        {
            Coord point[DIM];
            bool get = false;
            while(!get)
            {
                Coord diff = 0, dis = 0;
                for(size_t dim = 0; dim < DIM; dim++)
                {
                    point[dim] = getRandom() * NORMALIZE_SCALE;
                    diff = point[dim] - mid;
                    dis += diff * diff;
                }
                if(sqrt(dis) > mid)
                    continue;
                Mbr mbr(point);
                if(DCCR::contain(kdc, mbr) == 0 && !RStarTreeUtil::find(skirt, point))
                    get = true;
            }
            Point2D p(point[0], point[1]);
            rstq.push_back(p);
        }
    }
    else
    {
        for(size_t iq = 0; iq < Argument::numQueries; iq++)
        {
            Coord point[DIM];
            bool get = false;
            while(!get)
            {
                for(size_t dim = 0; dim < DIM; dim++)
                    point[dim] = getRandom() * NORMALIZE_SCALE;
                Mbr mbr(point);
                if(DCCR::contain(kdc, mbr) == 0 && !RStarTreeUtil::find(skirt, point))
                    get = true;
            }
            Point2D p(point[0], point[1]);
            rstq.push_back(p);
        }
    }

    string queryFileName = Argument::getQueryFileName();
    ofstream queryFileOFS(queryFileName.c_str());
    writePoints(queryFileOFS, rstq, queryFileName);
}

void RealDataGenerator::rstQ1()
{
    size_t holdk = Argument::k;
    Argument::k = 1;

    Point query(99999.9, 99999.9);
    Vertex q;
    q.x = 99999.9;
    q.y = 99999.9;
    RStarTree band;
    BBS::circularkSkyband(band, *Data::fTree, NORMALIZE_SCALE, Argument::k, query);
    KDepthContour kdc;
    SkyRider::kDepthContour(band, kdc, Argument::k);
    RStarTree skirt;
    RadiansPoint_V skirt_v;
    DCCR::outerSkirt(band, skirt, skirt_v, kdc, q);

    vector<Point2D> rstq;
    if(Argument::dataset == "unic")
    {
        Coord mid = NORMALIZE_SCALE / 2;
        for(size_t iq = 0; iq < Argument::numQueries; iq++)
        {
            Coord point[DIM];
            bool get = false;
            while(!get)
            {
                Coord diff = 0, dis = 0;
                for(size_t dim = 0; dim < DIM; dim++)
                {
                    point[dim] = getRandom() * NORMALIZE_SCALE;
                    diff = point[dim] - mid;
                    dis += diff * diff;
                }
                if(sqrt(dis) > mid)
                    continue;
                Mbr mbr(point);
                if(DCCR::contain(kdc, mbr) == 0 && !RStarTreeUtil::find(skirt, point))
                    get = true;
            }
            Point2D p(point[0], point[1]);
            rstq.push_back(p);
        }
    }
    else
    {
        for(size_t iq = 0; iq < Argument::numQueries; iq++)
        {
            Coord point[DIM];
            bool get = false;
            while(!get)
            {
                for(size_t dim = 0; dim < DIM; dim++)
                    point[dim] = getRandom() * NORMALIZE_SCALE;
                Mbr mbr(point);
                if(DCCR::contain(kdc, mbr) == 0 && !RStarTreeUtil::find(skirt, point))
                    get = true;
            }
            Point2D p(point[0], point[1]);
            rstq.push_back(p);
        }
    }

    Argument::k = holdk;

    string queryFileName = Argument::getQueryFileName();
    ofstream queryFileOFS(queryFileName.c_str());
    writePoints(queryFileOFS, rstq, queryFileName);
}

void RealDataGenerator::readCalNode()
{
    calNodes.clear();
    ifstream calNodeIFS(CAL_NODE_FILE);
    assert(calNodeIFS.is_open());
    while(true)
    {
        FSUNode node;
        calNodeIFS >> node;
        if(calNodeIFS) // check that the inputs succeeded
            calNodes.push_back(node);
        else
            break;
    }
    calNodeIFS.close();
    cout << calNodes.size() << "/" << NUM_CAL_NODES << " nodes from " << CAL_NODE_FILE << endl;
}

void RealDataGenerator::readCalPoint()
{
    calPoints.clear();
    ifstream calPointIFS(CAL_POINT_FILE);
    assert(calPointIFS.is_open());
    while(true)
    {
        FSUPoint point;
        calPointIFS >> point;
        if(calPointIFS) // check that the inputs succeeded
            calPoints.push_back(point);
        else
            break;
    }
    calPointIFS.close();
    cout << calPoints.size() << "/" << NUM_CAL_POINTS << " points from " << CAL_POINT_FILE << endl;
}

void RealDataGenerator::readNANode()
{
    naNodes.clear();
    ifstream naNodeIFS(NA_NODE_FILE);
    assert(naNodeIFS.is_open());
    while(true)
    {
        FSUNode node;
        naNodeIFS >> node;
        if(naNodeIFS) // check that the inputs succeeded
            naNodes.push_back(node);
        else
            break;
    }
    naNodeIFS.close();
    cout << naNodes.size() << "/" << NUM_NA_NODES << " nodes from " << NA_NODE_FILE << endl;
}

void RealDataGenerator::readSFNode()
{
    sfNodes.clear();
    ifstream sfNodeIFS(SF_NODE_FILE);
    assert(sfNodeIFS.is_open());
    while(true)
    {
        FSUNode node;
        sfNodeIFS >> node;
        if(sfNodeIFS) // check that the inputs succeeded
            sfNodes.push_back(node);
        else
            break;
    }
    sfNodeIFS.close();
    cout << sfNodes.size() << "/" << NUM_SF_NODES << " nodes from " << SF_NODE_FILE << endl;
}

template <class T>
void RealDataGenerator::normalize(vector<T> &pointV)
{
    double xmin = INF_P, xmax = INF_N, ymin = INF_P, ymax = INF_N;
    double increase;
    for(size_t in = 0; in < pointV.size(); in++)
    {
        xmin = min(xmin, pointV.at(in).longitude);
        xmax = max(xmax, pointV.at(in).longitude);
        ymin = min(ymin, pointV.at(in).latitude);
        ymax = max(ymax, pointV.at(in).latitude);
    }
    if(xmin <= 0)
    {
        increase = 0 - xmin;
        xmin = INF_P;
        for(size_t in = 0; in < pointV.size(); in++)
        {
            pointV.at(in).longitude += increase;
            xmin = min(xmin, pointV.at(in).longitude);
            xmax = max(xmax, pointV.at(in).longitude);
        }
    }
    if(ymin <= 0)
    {
        increase = 0 - ymin;
        ymin = INF_P;
        for(size_t in = 0; in < pointV.size(); in++)
        {
            pointV.at(in).latitude += increase;
            ymin = min(ymin, pointV.at(in).latitude);
            ymax = max(ymax, pointV.at(in).latitude);
        }
    }
    //cout << "<" << xmin << "," << xmax << "> <" << ymin << "," << ymax << ">" << endl;
    double xRange = xmax - xmin, xScaleLow = getRandom(), xScaleRange = NORMALIZE_SCALE - xScaleLow - getRandom();
    double yRange = ymax - ymin, yScaleLow = getRandom(), yScalRange = NORMALIZE_SCALE - yScaleLow - getRandom();
    for(size_t in = 0; in < pointV.size(); in++)
    {
        double x = xScaleLow + (pointV.at(in).longitude - xmin) / xRange * xScaleRange;
        double y = yScaleLow + (pointV.at(in).latitude - ymin) / yRange * yScalRange;
        points.push_back(Point2D(x, y));
    }
}

template <class T>
void RealDataGenerator::duplicateDetect(vector<T> &pointV)
{
    sort(pointV.begin(), pointV.end());
    size_t cnt = 0;
    for(size_t ip = 0; ip < pointV.size() - 1; ip++)
    {
        bool xEqual = (pointV.at(ip).longitude == pointV.at(ip + 1).longitude);
        bool yEqual = (pointV.at(ip).latitude == pointV.at(ip + 1).latitude);
        if(xEqual & yEqual)
            cnt++;
    }
    cout << cnt << " pairs being equal" << endl;
}

template <class T>
void RealDataGenerator::duplicateRemove(vector<T> &pointV)
{
    sort(pointV.begin(), pointV.end());
    vector<T> copy;

    for(size_t ip = 0; ip < pointV.size() - 1; ip++)
    {
        bool xEqual = (pointV.at(ip).longitude == pointV.at(ip + 1).longitude);
        bool yEqual = (pointV.at(ip).latitude == pointV.at(ip + 1).latitude);
        if(!xEqual || !yEqual)
            copy.push_back(pointV.at(ip));
    }
    copy.push_back(pointV.at(pointV.size() - 1));

    size_t size = pointV.size();
    pointV.clear();
    for(size_t ip = 0; ip < copy.size(); ip++)
        pointV.push_back(copy.at(ip));
    cout << (size - pointV.size()) << " points removed" << endl;
}

void RealDataGenerator::check()
{
    sort(points.begin(), points.end());
    double xmin = points.at(0).x, xmax = points.at(points.size() - 1).x, ymin = INF_P, ymax = INF_N;
    for(size_t ip = 0; ip < points.size(); ip++)
    {
        ymin = min(ymin, points.at(ip).y);
        ymax = max(ymax, points.at(ip).y);
    }
    cout << "<" << xmin << "," << xmax << "> <" << ymin << "," << ymax << ">" << endl;
    size_t cnt = 0;
    for(size_t ip = 0; ip < points.size() - 1; ip++)
    {
        bool xEqual = ( abs(points.at(ip).x - points.at(ip + 1).x) < EPS );
        bool yEqual = ( abs(points.at(ip).y - points.at(ip + 1).y) < EPS );
        if(xEqual & yEqual)
            cnt++;
    }
    cout << cnt << " pairs being equal" << endl;
}

void RealDataGenerator::writePoints(ofstream& out, vector<Point2D>& source, string fileName)
{
    assert(out.good());
    for(size_t ip = 0; ip < source.size(); ip++)
        out << source.at(ip);
    //cout << source.size() << " points to " << fileName << endl;
}

void RealDataGenerator::writeRealData()
{
    vector<Point2D> copy;
    copy.swap(points);
    sort(copy.begin(), copy.end());
    points.clear();
    for(size_t ip = 0; ip < copy.size() - 1; ip++)
    {
        bool xEqual = (abs(copy.at(ip).x - copy.at(ip + 1).x) < EPS);
        bool yEqual = (abs(copy.at(ip).y - copy.at(ip + 1).y) < EPS);
        if(!xEqual || !yEqual)
            points.push_back(copy.at(ip));
    }
    points.push_back(copy.at(copy.size() - 1));
    cout << (copy.size() - points.size()) << " points removed" << endl;
    ofstream pointOFS(REAL_DATA_FILE);
    writePoints(pointOFS, points, REAL_DATA_FILE);
}

void RealDataGenerator::readRealData()
{
    points.clear();
    ifstream pointsIFS(REAL_DATA_FILE);
    assert(pointsIFS.is_open());
    while(true)
    {
        Point2D point;
        pointsIFS >> point;
        if(pointsIFS) // check that the inputs succeeded
            points.push_back(point);
        else
            break;
    }
    pointsIFS.close();
    //cout << points.size() << "/" << NUM_POINTS << " nodes from " << REAL_DATA_FILE << endl;
}

void RealDataGenerator::writeUniData()
{
    vector<Point2D> copy;
    copy.swap(points);
    sort(copy.begin(), copy.end());
    points.clear();
    for(size_t ip = 0; ip < copy.size() - 1; ip++)
    {
        bool xEqual = (abs(copy.at(ip).x - copy.at(ip + 1).x) < EPS);
        bool yEqual = (abs(copy.at(ip).y - copy.at(ip + 1).y) < EPS);
        if(!xEqual || !yEqual)
            points.push_back(copy.at(ip));
    }
    points.push_back(copy.at(copy.size() - 1));
    cout << (copy.size() - points.size()) << " points removed" << endl;
    ofstream pointOFS(UNI_DATA_FILE);
    writePoints(pointOFS, points, UNI_DATA_FILE);
}

void RealDataGenerator::readUniData()
{
    points.clear();
    ifstream pointsIFS(UNI_DATA_FILE);
    assert(pointsIFS.is_open());
    while(true)
    {
        Point2D point;
        pointsIFS >> point;
        if(pointsIFS) // check that the inputs succeeded
            points.push_back(point);
        else
            break;
    }
    pointsIFS.close();
}

void RealDataGenerator::writeUniCData()
{
    vector<Point2D> copy;
    copy.swap(points);
    sort(copy.begin(), copy.end());
    points.clear();
    for(size_t ip = 0; ip < copy.size() - 1; ip++)
    {
        bool xEqual = (abs(copy.at(ip).x - copy.at(ip + 1).x) < EPS);
        bool yEqual = (abs(copy.at(ip).y - copy.at(ip + 1).y) < EPS);
        if(!xEqual || !yEqual)
            points.push_back(copy.at(ip));
    }
    points.push_back(copy.at(copy.size() - 1));
    cout << (copy.size() - points.size()) << " points removed" << endl;
    ofstream pointOFS(UNIC_DATA_FILE);
    writePoints(pointOFS, points, UNI_DATA_FILE);
}

void RealDataGenerator::readUniCData()
{
    points.clear();
    ifstream pointsIFS(UNIC_DATA_FILE);
    assert(pointsIFS.is_open());
    while(true)
    {
        Point2D point;
        pointsIFS >> point;
        if(pointsIFS) // check that the inputs succeeded
            points.push_back(point);
        else
            break;
    }
    pointsIFS.close();
}

void RealDataGenerator::pick(vector<Point2D>& from, vector<Point2D>& to, vector<Point2D>& left, size_t num)
{
    assert(num <= from.size());

    to.clear();
    left.clear();

    for(size_t ip = 0; ip < from.size(); ip++)
    {
        size_t remain = from.size() - ip;
        size_t require = num - to.size();
        if(require == 0)
            left.push_back(from.at(ip));
        else
        {
            if(require == remain)
                to.push_back(from.at(ip));
            else
            {
                double passmark = require / (double) remain;
                if(getRandom() <= passmark)
                    to.push_back(from.at(ip));
                else
                    left.push_back(from.at(ip));
            }
        }
    }
    //cout << "points picked " << to.size() << "/"<< from.size() << endl;
}
