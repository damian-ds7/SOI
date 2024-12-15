#include "MyMonitor.h"
#include <iostream>

void MyMonitor::prodEven() {
  enter();

  if (!canProdEven()) {
    ++prodEvenWaiting;
    wait(prodEvenCond);
    --prodEvenWaiting;
  }

  unsigned int element = nextEven;
  buffer.push_back(element);
  std::cout << "Produced even: " << element << "\n";
  nextEven = (nextEven + 2) % 50;
  ++evenCount;

  if (prodOddWaiting > 0u && canProdOdd()) {
    signal(prodOddCond);
  } else if (conEvenWaiting > 0u && canConEven()) {
    signal(conEvenCond);
  } else if (conOddWaiting > 0u && canConOdd()) {
    signal(conOddCond);
  }
  leave();
}

void MyMonitor::prodOdd() {
  enter();

  if (!canProdOdd()) {
    ++prodOddWaiting;
    wait(prodOddCond);
    --prodOddWaiting;
  }

  unsigned int element = nextOdd;
  buffer.push_back(element);
  std::cout << "Produced odd: " << element << "\n";
  nextOdd = (nextOdd + 2) % 50;
  ++oddCount;

  if (prodEvenWaiting > 0u && canProdEven()) {
    signal(prodOddCond);
  } else if (conEvenWaiting > 0u && canConEven()) {
    signal(conEvenCond);
  } else if (conOddWaiting > 0u && canConOdd()) {
    signal(conOddCond);
  }
  leave();
}

void MyMonitor::conEven() {
  enter();

  if (!canConEven()) {
    ++conEvenWaiting;
    wait(conEvenCond);
    --conEvenWaiting;
  }

  unsigned int element = buffer.front();
  buffer.pop_front();
  std::cout << "Consumed even: " << element << "\n";
  --evenCount;

  if (prodOddWaiting > 0u && canProdOdd()) {
    signal(prodOddCond);
  } else if (prodEvenWaiting > 0u && canProdEven()) {
    signal(prodEvenCond);
  } else if (conOddWaiting > 0u && canConOdd()) {
    signal(conOddCond);
  }
  leave();
}

void MyMonitor::conOdd() {
  enter();

  if (!canConOdd()) {
    ++conOddWaiting;
    wait(conOddCond);
    --conOddWaiting;
  }

  unsigned int element = buffer.front();
  buffer.pop_front();
  std::cout << "Consumed odd: " << element << "\n";
  --oddCount;

  if (prodOddWaiting > 0u && canProdOdd()) {
    signal(prodOddCond);
  } else if (prodEvenWaiting > 0u && canProdEven()) {
    signal(prodEvenCond);
  } else if (conEvenWaiting > 0u && canConEven()) {
    signal(conEvenCond);
  }
  leave();
}

bool MyMonitor::canProdEven() {
  return evenCount < 10u;
}

bool MyMonitor::canProdOdd() {
  return oddCount < evenCount;
}

// bool MyMonitor::canProdEven() {
//   int count_even = 0;
//   for(auto it = buffer.begin(); it != buffer.end(); ++it)  {
//     if(*it % 2 == 0) {
//       ++count_even;
//     }
//   }
//   return count_even < 10;
// }

// bool MyMonitor::canProdOdd() {
//   int count_even = 0;
//   int count_odd = 0;
//   for(auto it = buffer.begin(); it != buffer.end(); ++it)  {
//     if(*it % 2 == 0) {
//       ++count_even;
//     } else {
//       ++count_odd;
//     }
//   }
//   return count_even > count_odd;
// }

bool MyMonitor::canConEven() {
  return buffer.size() >= 3 && buffer.front() % 2 == 0;
}

bool MyMonitor::canConOdd() {
  return buffer.size() >= 7 && buffer.front() % 2 == 1;
}

void MyMonitor::fillEven() {
  for (unsigned int i = 0; i < 20; i += 2) {
    buffer.push_back(i);
  }
  evenCount = 10u;
  nextEven = 20u;
}

void MyMonitor::fillOdd() {
  for (unsigned int i = 1; i < 20; i += 2) {
    buffer.push_back(i);
  }
  oddCount = 10;
  nextOdd = 21;
}

void MyMonitor::fillBoth() {
  for (unsigned int i = 1; i < 20; ++i) {
    buffer.push_back(i);
  }
  evenCount = 10u;
  oddCount = 10u;
  nextEven = 20u;
  nextOdd = 21u;
}