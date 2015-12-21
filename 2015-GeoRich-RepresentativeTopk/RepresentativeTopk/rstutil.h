#ifndef RSTUTIL_H
#define RSTUTIL_H

#include "rstree.h"

typedef vector<LeafNodeEntry> EntryV;
typedef RSTree* RSTreePtr;

typedef struct MinHeapEntry
{
    double key;
    NodePtr nPtr;
    EntryPtr ePtr;

    MinHeapEntry(double key, NodePtr nPtr)
        : key(key), nPtr(nPtr), ePtr(NULL)
        {

        }
    MinHeapEntry(double key, EntryPtr ePtr)
        : key(key), nPtr(NULL), ePtr(ePtr)
        {

        }

    bool operator < (const MinHeapEntry& e) const;
}MinHeapEntry;

inline bool MinHeapEntry::operator < (const MinHeapEntry& e) const
{
    return key > e.key;
}

typedef vector<MinHeapEntry> MinHEntryV;

typedef struct MinHeap
{
    MinHEntryV hev;

    void push(MinHeapEntry e);
    MinHeapEntry pop();
    bool isEmpty();
    void clear();
}MinHeap;

inline void MinHeap::push(MinHeapEntry e)
{
    hev.push_back(e);
    push_heap(hev.begin(), hev.end());
}

inline MinHeapEntry MinHeap::pop()
{
    MinHeapEntry e = hev.front();
    pop_heap(hev.begin(), hev.end());
    hev.pop_back();
    return e;
}

inline bool MinHeap::isEmpty()
{
    return hev.empty();
}

inline void MinHeap::clear()
{
    hev.clear();
}

typedef struct MaxHeapEntry
{
    double key;
    NodePtr nPtr;
    EntryPtr ePtr;

    MaxHeapEntry(double key, NodePtr nPtr)
        : key(key), nPtr(nPtr), ePtr(NULL)
        {

        }
    MaxHeapEntry(double key, EntryPtr ePtr)
        : key(key), nPtr(NULL), ePtr(ePtr)
        {

        }

    bool operator < (const MaxHeapEntry& e) const;
}MaxHeapEntry;

inline bool MaxHeapEntry::operator < (const MaxHeapEntry& e) const
{
    return key < e.key;
}

typedef vector<MaxHeapEntry> MaxHEntryV;

typedef struct MaxHeap
{
    MaxHEntryV hev;

    void push(MaxHeapEntry e);
    MaxHeapEntry pop();
    bool isEmpty();
    void clear();
}MaxHeap;

inline void MaxHeap::push(MaxHeapEntry e)
{
    hev.push_back(e);
    push_heap(hev.begin(), hev.end());
}

inline MaxHeapEntry MaxHeap::pop()
{
    MaxHeapEntry e = hev.front();
    pop_heap(hev.begin(), hev.end());
    hev.pop_back();
    return e;
}





#endif // RSTUTIL_H
