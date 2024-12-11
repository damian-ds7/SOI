#pragma once

#include "MyMonitor.h"

extern MyMonitor myMonitor;

void* prodEven(void* arg);
void* prodOdd(void* arg);
void* conEven(void* arg);
void* conOdd(void* arg);