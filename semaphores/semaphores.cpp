#include <pthread.h>
#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include "monitor.h"
#include "semaphores.h"


std::queue<unsigned int> buffer;

Semaphore mutex(1);
Semaphore prodEvenMutex(0);
Semaphore prodOddMutex(0);
Semaphore conEvenMutex(0);
Semaphore conOddMutex(0);

unsigned int prodEvenWaiting = 0u;
unsigned int prodOddWaiting = 0u;
unsigned int conEvenWaiting = 0u;
unsigned int conOddWaiting = 0u;
unsigned int nextEven = 0u;
unsigned int nextOdd = 1u;
unsigned int evenCount = 0u;
unsigned int oddCount = 0u;

bool canProdEven() {
  return evenCount < 10;
}

bool canProdOdd() {
  return oddCount < evenCount;
}
bool canConEven() {
  return buffer.size() >= 3 && buffer.front() % 2 == 0;
}

bool canConOdd() {
  return buffer.size() >= 7 && buffer.front() % 2 == 1;
}

void* prodEven(void*) {
  while (true) {
    mutex.p();

    if (!canProdEven()) {
      ++prodEvenWaiting;
      mutex.v();
      prodEvenMutex.p();
      --prodEvenWaiting;
    }

    unsigned int element = nextEven;
    nextEven = (nextEven + 2) % 50;
    buffer.push(element);

    std::cout << "Produced even: " << element << "\n";

    ++evenCount;

    if (prodOddWaiting > 0u && canProdOdd()) {
      prodOddMutex.v();
    } else if (conEvenWaiting > 0u && canConEven()) {
      conEvenMutex.v();
    } else if (conOddWaiting > 0u && canConOdd()) {
      conOddMutex.v();
    } else {
      mutex.v();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}

void* prodOdd(void*) {
  while (true) {
    mutex.p();

    if (!canProdOdd()) {
      ++prodOddWaiting;
      mutex.v();
      prodOddMutex.p();
      --prodOddWaiting;
    }

    unsigned int element = nextOdd;
    nextOdd = (nextOdd + 2) % 50;
    buffer.push(element);
    ++oddCount;

    std::cout << "Produced odd: " << element << "\n";

    if (prodEvenWaiting > 0u && canProdEven()) {
      prodEvenMutex.v();
    } else if (conEvenWaiting > 0u && canConEven()) {
      conEvenMutex.v();
    } else if (conOddWaiting > 0u && canConOdd()) {
      conOddMutex.v();
    } else {
      mutex.v();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}

void* conEven(void*) {
  while (true) {
    mutex.p();

    if (!canConEven()) {
      ++conEvenWaiting;
      mutex.v();
      conEvenMutex.p();
      --conEvenWaiting;
    }

    unsigned int element = buffer.front();
    buffer.pop();
    --evenCount;

    std::cout << "Consumed even: " << element << "\n";

    if (prodOddWaiting > 0u && canProdOdd()) {
      prodOddMutex.v();
    } else if (prodEvenWaiting > 0u && canProdEven()) {
      prodEvenMutex.v();
    } else if (conOddWaiting > 0u && canConOdd()) {
      conOddMutex.v();
    } else {
      mutex.v();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}

void* conOdd(void*) {
  while (true) {
    mutex.p();

    if (!canConOdd()) {
      ++conOddWaiting;
      mutex.v();
      conOddMutex.p();
      --conOddWaiting;
    }

    unsigned int element = buffer.front();
    buffer.pop();
    --oddCount;

    std::cout << "Consumed odd: " << element << "\n";

    if (prodOddWaiting > 0u && canProdOdd()) {
      prodOddMutex.v();
    } else if (prodEvenWaiting > 0u && canProdEven()) {
      prodEvenMutex.v();
    } else if (conEvenWaiting > 0u && canConEven()) {
      conEvenMutex.v();
    } else {
      mutex.v();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(random() % 1000));
  }
}
