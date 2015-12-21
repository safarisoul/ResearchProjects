#include "color.h"

istream& operator >> (istream& in, ColorEntry& c)
{
    int id;
    in >> id;
    for(int j=0; j<COLOR_ENTRY_DIM; j++)
        in >> c.value[j];
    return in;
}

ostream& operator << (ostream& out, ColorEntry& c)
{
    out << c.value[0];
    for(int j=1; j<COLOR_ENTRY_DIM; j++)
        out << " " << c.value[j];
    out << endl;
    return out;
}

void Color::prepareData(double minValue, double maxValue, unsigned int total)
{
    readRawData();
    disturb();
    normalize(minValue, maxValue);
    writeData(minValue, maxValue, total);
    cout << "Color data file is ready" << endl;
}

void Color::readRawData()
{
    entries.clear();
    ifstream colorEntryIFS(COLOR_ENTRY_RAW_FILE);
    assert(colorEntryIFS.is_open());
    while(true)
    {
        ColorEntry entry;
        colorEntryIFS >> entry;
        if(colorEntryIFS) // check that the input succeeded
            entries.push_back(entry);
        else
            break;
    }
    colorEntryIFS.close();
    cout << entries.size() << " entries read from [Color] raw data file" << endl;
}

void Color::disturb()
{
    if(entries.size() >= 2)
        ;
    else
        assert(false);

    // find out the minimum and maximum value of each dimension
    for(int j=0; j<COLOR_ENTRY_DIM; j++)
    {
        minValue[j] = entries[0].value[j];
        maxValue[j] = entries[0].value[j];
    }
    for(unsigned int i=1; i<entries.size(); i++)
    {
        for(int j=0; j<COLOR_ENTRY_DIM; j++)
        {
            minValue[j] = min(minValue[j], entries[i].value[j]);
            maxValue[j] = max(maxValue[j], entries[i].value[j]);
        }
    }

    // disturb
    for(int j=0; j<COLOR_ENTRY_DIM; j++)
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

void Color::normalize(double normMin, double normMax)
{
    if(entries.size() >= 2)
        ;
    else
        assert(false);

    // find out the minimum and maximum value of each dimension
    for(int j=0; j<COLOR_ENTRY_DIM; j++)
    {
        minValue[j] = entries[0].value[j];
        maxValue[j] = entries[0].value[j];
    }
    for(unsigned int i=1; i<entries.size(); i++)
    {
        for(int j=0; j<COLOR_ENTRY_DIM; j++)
        {
            minValue[j] = min(minValue[j], entries[i].value[j]);
            maxValue[j] = max(maxValue[j], entries[i].value[j]);
        }
    }

    // scale to the right position
    double normRange = normMax - normMin;
    assert(normRange > 0);
    for(int j=0; j<COLOR_ENTRY_DIM; j++)
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
    for(int j=0; j<COLOR_ENTRY_DIM; j++)
    {
        minValue[j] = entries[0].value[j];
        maxValue[j] = entries[0].value[j];
    }
    for(unsigned int i=1; i<entries.size(); i++)
    {
        for(int j=0; j<COLOR_ENTRY_DIM; j++)
        {
            minValue[j] = min(minValue[j], entries[i].value[j]);
            maxValue[j] = max(maxValue[j], entries[i].value[j]);
        }
    }
    for(int j=0; j<COLOR_ENTRY_DIM; j++)
    {
        assert(minValue[j] >= normMin);
        assert(maxValue[j] <= normMax);
        cout << "Dim " << (j+1) << " [" << minValue[j] << ", " << maxValue[j] << "]" << endl;
    }
}

void Color::writeData(double normMin, double normMax, unsigned int total)
{
    stringstream sstream;
    sstream << "data/color_" << normMin << "_" << normMax << "_" << min(total, static_cast<unsigned int>(entries.size())) << ".txt";
    ofstream colorOFS(sstream.str().c_str());
    assert(colorOFS.good());
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
                colorOFS << entries[i];
                picked++;
            }
            else
            {
                double passmark = (total - picked)/(double)remain;
                if(getRandom() <= passmark)
                {
                    colorOFS << entries[i];
                    picked++;
                }
            }
        }
    }
    colorOFS.close();
}
