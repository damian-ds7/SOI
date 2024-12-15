#pragma once

#include <deque>
#include "monitor.h"

class MyMonitor : Monitor {
 public:
  void prodEven();
  void prodOdd();
  void conEven();
  void conOdd();

  void fillEven();
  void fillOdd();
  void fillBoth();

 private:
  std::deque<unsigned int> buffer;
  Condition prodEvenCond, prodOddCond, conEvenCond, conOddCond;
  unsigned int nextEven = 0u;
  unsigned int nextOdd = 1u;

  unsigned int evenCount = 0u;
  unsigned int oddCount = 0u;

  unsigned int prodEvenWaiting = 0u;
  unsigned int prodOddWaiting = 0u;
  unsigned int conEvenWaiting = 0u;
  unsigned int conOddWaiting = 0u;

  bool canProdEven();
  bool canProdOdd();
  bool canConEven();
  bool canConOdd();
};
