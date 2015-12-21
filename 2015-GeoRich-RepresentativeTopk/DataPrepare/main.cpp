#include "color.h"
#include "house.h"
#include "point.h"
#include "preference.h"

using namespace std;

int main(int argc, char* argv[])
{
    assert(argc > 1);

    string argName = string(argv[1]);
    if(argName == "color")
    {
        assert(argc == 5);
        cout << "data set : " << argName << endl;
        double normMin, normMax;
        unsigned int total;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> normMin;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> normMax;
        istringstream arg4Stream(argv[4]);
        arg4Stream >> total;
        cout << "normalize : [" << normMin << ", " << normMax << "]" << endl;
        cout << "total : " << total << endl;
        Color color;
        color.prepareData(normMin, normMax, total);
    }
    else if(argName == "house")
    {
        assert(argc == 5);
        cout << "data set : " << argName << endl;
        double normMin, normMax;
        unsigned int total;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> normMin;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> normMax;
        istringstream arg4Stream(argv[4]);
        arg4Stream >> total;
        cout << "normalize : [" << normMin << ", " << normMax << "]" << endl;
        cout << "total : " << total << endl;
        House house;
        house.prepareData(normMin, normMax, total);
    }
    else if(argName == "preferenceuni")
    {
        assert(argc == 4);
        cout << "data set : uniform preference" << endl;
        unsigned int total, dim;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> total;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> dim;
        cout << "total :　" << total << endl;
        cout << "dimension : " << dim << endl;
        Preference preference;
        preference.prepareUniData(total, dim);
    }
    else if(argName == "preferenceclustered")
    {
        assert(argc == 4);
        cout << "data set : clustered preference" << endl;
        unsigned int total, dim;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> total;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> dim;
        cout << "total :　" << total << endl;
        cout << "dimension : " << dim << endl;
        Preference preference;
        preference.prepareClusteredData(total, dim);
    }
    else if(argName == "pointcorrelated")
    {
        assert(argc == 5);
        cout << "data set : correlated point" << endl;
        double maxValue;
        unsigned int total, dim;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> maxValue;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> total;
        istringstream arg4Stream(argv[4]);
        arg4Stream >> dim;
        cout << "maxValue : " << maxValue << endl;
        cout << "total : " << total << endl;
        cout << "dim : "  << dim << endl;
        Point point;
        point.prepareCorrelatedData(maxValue, total, dim);
    }
    else if(argName == "pointanticorrelated")
    {
        assert(argc == 5);
        cout << "data set : anticorrelated point" << endl;
        double maxValue;
        unsigned int total, dim;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> maxValue;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> total;
        istringstream arg4Stream(argv[4]);
        arg4Stream >> dim;
        cout << "maxValue : " << maxValue << endl;
        cout << "total : " << total << endl;
        cout << "dim : "  << dim << endl;
        Point point;
        point.prepareAntiCorrelatedData(maxValue, total, dim);
    }
    else if(argName == "pointclustered")
    {
        assert(argc == 5);
        cout << "data set : clustered point" << endl;
        double maxValue;
        unsigned int total, dim;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> maxValue;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> total;
        istringstream arg4Stream(argv[4]);
        arg4Stream >> dim;
        cout << "maxValue : " << maxValue << endl;
        cout << "total : " << total << endl;
        cout << "dim : "  << dim << endl;
        Point point;
        point.prepareClusteredData(maxValue, total, dim);
    }
    else if(argName == "pointuni")
    {
        assert(argc == 5);
        cout << "data set : uniform point" << endl;
        double maxValue;
        unsigned int total, dim;
        istringstream arg2Stream(argv[2]);
        arg2Stream >> maxValue;
        istringstream arg3Stream(argv[3]);
        arg3Stream >> total;
        istringstream arg4Stream(argv[4]);
        arg4Stream >> dim;
        cout << "maxValue : " << maxValue << endl;
        cout << "total : " << total << endl;
        cout << "dim : "  << dim << endl;
        Point point;
        point.prepareUniData(maxValue, total, dim);
    }



    return 0;
}
