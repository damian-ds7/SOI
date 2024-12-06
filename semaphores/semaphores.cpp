#include <pthread.h>
#include <iostream>
#include <queue>
#include "monitor.h"

std::queue<int> buffer;

Semaphore mutex(1);
Semaphore prodEvenMutex(0);
Semaphore prodOddMutex(0);
Semaphore conEvenMutex(0);
Semaphore conOddMutex(0);

unsigned int prodEvenWaiting = 0, prodOddWaiting = 0, conEvenWaiting = 0,
             conOddWaiting = 0, nextEven = 0, nextOdd = 1, evenCount = 0,
             oddCount = 0;

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

    int element = nextEven;
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

    int element = nextOdd;
    nextEven = (nextOdd + 2) % 50;
    buffer.push(element);
    ++oddCount;

    std::cout << "Produced odd: " << element << "\n";

    if (prodEvenWaiting > 0u && canProdEven()) {
      prodOddMutex.v();
    } else if (conEvenWaiting > 0u && canConEven()) {
      conEvenMutex.v();
    } else if (conOddWaiting > 0u && canConOdd()) {
      conOddMutex.v();
    } else {
      mutex.v();
    }
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

    int element = buffer.front();
    buffer.pop();
    --evenCount;

    std::cout << "Consumed even: " << element << "\n";

    if (prodOddWaiting > 0u && canProdOdd()) {
      prodOddMutex.v();
    } else if (prodEvenWaiting > 0u && canProdEven()) {
      conEvenMutex.v();
    } else if (conOddWaiting > 0u && canConOdd()) {
      conOddMutex.v();
    } else {
      mutex.v();
    }
  }
}

void* conOdd(void*) {
  while (true) {
    mutex.p();

    if (!canConOdd()) {
      ++conOddWaiting;
      mutex.v();
      conEvenMutex.p();
      --conOddWaiting;
    }

    int element = buffer.front();
    buffer.pop();
    --oddCount;

    std::cout << "Consumed odd: " << element << "\n";

    if (prodOddWaiting > 0u && canProdOdd()) {
      prodOddMutex.v();
    } else if (prodEvenWaiting > 0u && canProdEven()) {
      conEvenMutex.v();
    } else if (conEvenWaiting > 0u && canConEven()) {
      conEvenMutex.v();
    } else {
      mutex.v();
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    printf("4 arguments required: prod_even prod_odd cons_even cons_odd\n");
    return 1;
  }

  int thread_count = 0;

  for (int i = 1; i < argc; ++i) {
    thread_count += atoi(argv[i]);
  }

  pthread_t threads[thread_count];

  int i = 0;

  for (; i < atoi(argv[1]); ++i) {
    pthread_create(threads + i, NULL, &prodEven, NULL);
  }

  for (; i < i + atoi(argv[2]); ++i) {
    pthread_create(threads + i, NULL, &prodOdd, NULL);
  }

  for (; i < i + atoi(argv[3]); ++i) {
    pthread_create(threads + i, NULL, &conEven, NULL);
  }

  for (; i < i + atoi(argv[4]); ++i) {
    pthread_create(threads + i, NULL, &conOdd, NULL);
  }

  for (i = 0; i < thread_count; ++i) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}