#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <algorithm>

using namespace std;

// max heap

typedef struct HeapEntry
{
    double key;
    int id;

    HeapEntry(double key, int id)
        : key(key), id(id)
        {}

    inline bool operator < (const HeapEntry& e) const { return key < e.key; }
}HeapEntry;

typedef vector<HeapEntry> HEntryV;

typedef struct Heap
{
    HEntryV hev;

    inline void push(HeapEntry e) { hev.push_back(e); push_heap(hev.begin(), hev.end()); };
    inline HeapEntry pop() { HeapEntry e = hev.front(); pop_heap(hev.begin(), hev.end()); hev.pop_back(); return e; };
    inline HEntryV getElements() { HEntryV nv(hev); sort_heap(nv.begin(), nv.end()); return nv; };
    inline bool haskey(double key) { for(unsigned int i=0; i<hev.size(); i++) if(hev[i].key == key) return true; return false; };
    inline double top() { return hev.front().key; };
    inline int topid() { return hev.front().id; };
    inline bool isEmpty() { return hev.empty(); };
    inline int size() { return hev.size(); };
    inline void clear() { hev.clear(); };
}Heap;

#endif // HEAP_H
