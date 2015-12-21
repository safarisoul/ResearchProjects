#ifndef MAIN_H
#define MAIN_H

#include "Generator.h"
#include "BruteForce.h"
#include "DCCR.h"
#include "CHFC.h"

void run();
void init();
void genRealData();
void genUniData();
void genUniCData();
void pickFCFromRealData();
void pickFCFromUniData();
void pickFCFromUniCData();
void loadData();
size_t runBruteForce();
size_t runCHFC();
size_t runRkFN();
void genrstq();
void genrstq1();
void vdca();
void vara();
void tpru();

#endif // MAIN_H
