#include <pthread.h>
#include <chrono>
#include <queue>
#include <random>
#include <thread>
#include "MyMonitor.h"

MyMonitor myMonitor;

void sleepRandom() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dist(0.1, 1);

  std::this_thread::sleep_for(std::chrono::duration<double>(dist(gen)));
}

void prodEven() {
  while (true) {
    myMonitor.prodEven();
    sleepRandom();
  }
}

void prodOdd() {
  while (true) {
    myMonitor.prodOdd();
    sleepRandom();
  }
}

void conEven() {
  while (true) {
    myMonitor.conEven();
    sleepRandom();
  }
}

void conOdd() {
  while (true) {
    myMonitor.conOdd();
    sleepRandom();
  }
}