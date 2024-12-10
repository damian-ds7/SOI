#pragma once

#include <queue>
#include "monitor.h"

extern std::queue<unsigned int> buffer;

extern Semaphore mutex;
extern Semaphore prodEvenMutex;
extern Semaphore prodOddMutex;
extern Semaphore conEvenMutex;
extern Semaphore conOddMutex;

extern unsigned int prodEvenWaiting;
extern unsigned int prodOddWaiting;
extern unsigned int conEvenWaiting;
extern unsigned int conOddWaiting;
extern unsigned int nextEven;
extern unsigned int nextOdd;
extern unsigned int evenCount;
extern unsigned int oddCount;

bool canProdEven();
bool canProdOdd();
bool canConEven();
bool canConOdd();
void* prodEven(void* arg);
void* prodOdd(void* arg);
void* conEven(void* arg);
void* conOdd(void* arg);