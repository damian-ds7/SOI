#include "MyMonitor.h"
#include <iostream>

void MyMonitor::prodEven() {
  enter();

  if (!canProdEven()) {
    wait(prodEvenCond);
  }

  unsigned int element = nextEven;
  buffer.push(element);
  std::cout << "Produced even: " << element << "\n";
  nextEven = (nextEven + 2) % 50;
  ++evenCount;

  if (prodOddCond.getWaitingCount() > 0 && canProdOdd()) {
    signal(prodOddCond);
  } else if (conEvenCond.getWaitingCount() > 0 && canConEven()) {
    signal(conEvenCond);
  } else if (conOddCond.getWaitingCount() > 0 && canConOdd()) {
    signal(conOddCond);
  }
  leave();
}

void MyMonitor::prodOdd() {
  enter();

  if (!canProdOdd()) {
    wait(prodOddCond);
  }

  unsigned int element = nextOdd;
  buffer.push(element);
  std::cout << "Produced odd: " << element << "\n";
  nextOdd = (nextOdd + 2) % 50;
  ++oddCount;

  if (prodEvenCond.getWaitingCount() > 0 && canProdEven()) {
    signal(prodOddCond);
  } else if (conEvenCond.getWaitingCount() > 0 && canConEven()) {
    signal(conEvenCond);
  } else if (conOddCond.getWaitingCount() > 0 && canConOdd()) {
    signal(conOddCond);
  }
  leave();
}

void MyMonitor::conEven() {
  enter();

  if (!canConEven()) {
    wait(conEvenCond);
  }

  unsigned int element = buffer.front();
  buffer.pop();
  std::cout << "Consumed even: " << element << "\n";
  --evenCount;

  if (prodOddCond.getWaitingCount() > 0 && canProdOdd()) {
    signal(prodOddCond);
  } else if (prodEvenCond.getWaitingCount() > 0 && canProdEven()) {
    signal(conEvenCond);
  } else if (conOddCond.getWaitingCount() > 0 && canConOdd()) {
    signal(conOddCond);
  }
  leave();
}

void MyMonitor::conOdd() {
  enter();

  if (!canConOdd()) {
    wait(conOddCond);
  }

  unsigned int element = buffer.front();
  buffer.pop();
  std::cout << "Consumed odd: " << element << "\n";
  --oddCount;

  if (prodOddCond.getWaitingCount() > 0 && canProdOdd()) {
    signal(prodOddCond);
  } else if (prodEvenCond.getWaitingCount() > 0 && canProdEven()) {
    signal(prodEvenCond);
  } else if (conEvenCond.getWaitingCount() > 0 && canConEven()) {
    signal(conEvenCond);
  }
  leave();
}

bool MyMonitor::canProdEven() {
  return evenCount < 10;
}

bool MyMonitor::canProdOdd() {
  return oddCount < evenCount;
}
bool MyMonitor::canConEven() {
  return buffer.size() >= 3 && buffer.front() % 2 == 0;
}

bool MyMonitor::canConOdd() {
  return buffer.size() >= 7 && buffer.front() % 2 == 1;
}

void MyMonitor::fillEven() {
  for (unsigned int i = 0; i < 20; i += 2) {
    buffer.push(i);
  }
  evenCount = 10;
  nextEven = 20;
}

void MyMonitor::fillOdd() {
  for (unsigned int i = 1; i < 20; i += 2) {
    buffer.push(i);
  }
  oddCount = 10;
  nextOdd = 21;
}

void MyMonitor::fillBoth() {
  for (unsigned int i = 1; i < 20; ++i) {
    buffer.push(i);
  }
  evenCount = 10;
  oddCount = 10;
  nextEven = 20;
  nextOdd = 21;
}