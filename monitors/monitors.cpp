#include <pthread.h>
#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include "MyMonitor.h"

MyMonitor myMonitor;

void* prodEven(void*) {
  while (true) {
    myMonitor.prodEven();
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}

void* prodOdd(void*) {
  while (true) {
    myMonitor.prodOdd();
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}

void* conEven(void*) {
  while (true) {
    myMonitor.conEven();
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}

void* conOdd(void*) {
  while (true) {
    myMonitor.conOdd();
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}
