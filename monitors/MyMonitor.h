#pragma once

#include <queue>
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
  std::queue<unsigned int> buffer;
  Condition prodEvenCond, prodOddCond, conEvenCond, conOddCond;
  unsigned int nextEven = 0;
  unsigned int nextOdd = 1;

  unsigned int evenCount = 0;
  unsigned int oddCount = 0;

  bool canProdEven();
  bool canProdOdd();
  bool canConEven();
  bool canConOdd();
};
