#include "house.h"

istream& operator >> (istream& in, HouseEntry& h)
{
    int id;
    in >> id;
    char ch;
    for(int j=0; j<HOUSE_ENTRY_DIM; j++)
        in >> ch >> h.value[j];
    return in;
}

ostream& operator << (ostream& out, HouseEntry& h)
{
    out << h.value[0];
    for(int j=1; j<HOUSE_ENTRY_DIM; j++)
        out << " " << h.value[j];
    out << endl;
    return out;
}

void House::prepareData(double minValue, double maxValue, unsigned int total)
{
    readRawData();
    disturb();
    normalize(minValue, maxValue);
    writeData(minValue, maxValue, total);
    cout << "House data file is ready" << endl;
}

void House::readRawData()
{
    entries.clear();
    ifstream houseEntryIFS(HOUSE_ENTRY_RAW_FILE);
    assert(houseEntryIFS.is_open());
    string line;
    getline(houseEntryIFS, line);
    while(true)
    {
        HouseEntry entry;
        houseEntryIFS >> entry;
        if(houseEntryIFS) // check that the input succeeded
            entries.push_back(entry);
        else
            break;
    }
    houseEntryIFS.close();
    cout << entries.size() << " entries read from [House] raw data file" << endl;
}

void House::disturb()
{
    if(entries.size() >= 2)
        ;
    else
        assert(false);

    // find out the minimum and maximum value of each dimension
    for(int j=0; j<HOUSE_ENTRY_DIM; j++)
    {
        minValue[j] = entries[0].value[j];
        maxValue[j] = entries[0].value[j];
    }
    for(unsigned int i=1; i<entries.size(); i++)
    {
        for(int j=0; j<HOUSE_ENTRY_DIM; j++)
        {
            minValue[j] = min(minValue[j], entries[i].value[j]);
            maxValue[j] = max(maxValue[j], entries[i].value[j]);
        }
    }

    // disturb
    for(int j=0; j<HOUSE_ENTRY_DIM; j++)
    {
        double range = maxValue[j] - minValue[j];
        assert(range > 0);
        for(unsigned int i=0; i<entries.size(); i++)
        {
            double orig = entries[i].value[j];
            double shift = range / entries.size() * getRandom();
            entries[i].value[j] = orig + shift;
        }
    }
}

void House::normalize(double normMin, double normMax)
{
    if(entries.size() >= 2)
        ;
    else
        assert(false);

    // find out the minimum and maximum value of each dimension
    for(int j=0; j<HOUSE_ENTRY_DIM; j++)
    {
        minValue[j] = entries[0].value[j];
        maxValue[j] = entries[0].value[j];
    }
    for(unsigned int i=1; i<entries.size(); i++)
    {
        for(int j=0; j<HOUSE_ENTRY_DIM; j++)
        {
            minValue[j] = min(minValue[j], entries[i].value[j]);
            maxValue[j] = max(maxValue[j], entries[i].value[j]);
        }
    }

    // scale to the right position
    double normRange = normMax - normMin;
    assert(normRange > 0);
    for(int j=0; j<HOUSE_ENTRY_DIM; j++)
    {
        double originalRange = maxValue[j] - minValue[j];
        assert(originalRange > 0);
        for(unsigned int i=0; i<entries.size(); i++)
        {
            double originalValue = entries[i].value[j];
            double normValue = normMin + (originalValue - minValue[j]) / originalRange * normRange;
            entries[i].value[j] = normValue;
        }
    }

    // check normalized value
    for(int j=0; j<HOUSE_ENTRY_DIM; j++)
    {
        minValue[j] = entries[0].value[j];
        maxValue[j] = entries[0].value[j];
    }
    for(unsigned int i=1; i<entries.size(); i++)
    {
        for(int j=0; j<HOUSE_ENTRY_DIM; j++)
        {
            minValue[j] = min(minValue[j], entries[i].value[j]);
            maxValue[j] = max(maxValue[j], entries[i].value[j]);
        }
    }
    for(int j=0; j<HOUSE_ENTRY_DIM; j++)
    {
        assert(minValue[j] >= normMin);
        assert(maxValue[j] <= normMax);
        cout << "Dim " << (j+1) << " [" << minValue[j] << ", " << maxValue[j] << "]" << endl;
    }
}

void House::writeData(double normMin, double normMax, unsigned int total)
{
    stringstream sstream;
    sstream << "data/house_" << normMin << "_" << normMax << "_" << min(total, static_cast<unsigned int>(entries.size())) << ".txt";
    ofstream houseOFS(sstream.str().c_str());
    assert(houseOFS.good());
    unsigned int picked = 0;
    for(unsigned int i=0; i<entries.size(); i++)
    {
        unsigned int remain = entries.size() - i;
        if(picked == total)
            break;
        else
        {
            if(total - picked >= remain)
            {
                houseOFS << entries[i];
                picked++;
            }
            else
            {
                double passmark = (total - picked)/(double)remain;
                if(getRandom() <= passmark)
                {
                    houseOFS << entries[i];
                    picked++;
                }
            }
        }
    }
    houseOFS.close();
}
