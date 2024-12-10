#include <pthread.h>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include <chrono>
#include "monitor.h"


std::queue<unsigned int> buffer;

Semaphore mutex(1);
Semaphore prodEvenMutex(0);
Semaphore prodOddMutex(0);
Semaphore conEvenMutex(0);
Semaphore conOddMutex(0);

unsigned int prodEvenWaiting = 0u, prodOddWaiting = 0u, conEvenWaiting = 0u,
             conOddWaiting = 0u, nextEven = 0u, nextOdd = 1u, evenCount = 0u,
             oddCount = 0u;

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

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("1 argument required: test_num\n");
    return 1;
  }

  int testNum = atoi(argv[1]);
  pthread_t threads[8];
  int threadNum;

  switch (testNum) {
    case 1:
      std::cout << "1 prod even" << std::endl;
      pthread_create(&threads[0], NULL, &prodEven, NULL);
      threadNum = 1;
      break;
    case 2:
      std::cout << "1 prod odd" << std::endl;
      pthread_create(&threads[0], NULL, &prodOdd, NULL);
      threadNum = 1;
      break;
    case 3:
      std::cout << "1 cons even" << std::endl;
      pthread_create(&threads[0], NULL, &conEven, NULL);
      threadNum = 1;
      break;
    case 4:
      std::cout << "1 cons odd" << std::endl;
      pthread_create(&threads[0], NULL, &conOdd, NULL);
      threadNum = 1;
      break;
    case 5:
      std::cout << "1 prod even + 1 prod odd" << std::endl;
      pthread_create(&threads[0], NULL, &prodEven, NULL);
      pthread_create(&threads[1], NULL, &prodOdd, NULL);
      threadNum = 2;
      break;
    case 6:
      std::cout << "1 cons even + 1 cons odd" << std::endl;
      pthread_create(&threads[0], NULL, &conEven, NULL);
      pthread_create(&threads[1], NULL, &conOdd, NULL);
      threadNum = 2;
      break;
    case 7:
      std::cout << "1 prod even + 1 cons even" << std::endl;
      pthread_create(&threads[0], NULL, &prodEven, NULL);
      pthread_create(&threads[1], NULL, &conEven, NULL);
      threadNum = 2;
      break;
    case 8:
      std::cout << "1 prod odd + 1 cons odd" << std::endl;
      pthread_create(&threads[0], NULL, &prodOdd, NULL);
      pthread_create(&threads[1], NULL, &conOdd, NULL);
      threadNum = 2;
      break;
    case 9:
      std::cout << "1 prod even + 1 prod odd + 1 cons even + 1 cons odd" << std::endl;
      pthread_create(&threads[0], NULL, &prodEven, NULL);
      pthread_create(&threads[1], NULL, &prodOdd, NULL);
      pthread_create(&threads[2], NULL, &conEven, NULL);
      pthread_create(&threads[3], NULL, &conOdd, NULL);
      threadNum = 4;
      break;
    case 10:
      std::cout << "2 prod even + 2 prod odd + 2 cons even + 2 cons odd" << std::endl;
      pthread_create(&threads[0], NULL, &prodEven, NULL);
      pthread_create(&threads[1], NULL, &prodEven, NULL);
      pthread_create(&threads[2], NULL, &prodOdd, NULL);
      pthread_create(&threads[3], NULL, &prodOdd, NULL);
      pthread_create(&threads[4], NULL, &conEven, NULL);
      pthread_create(&threads[5], NULL, &conEven, NULL);
      pthread_create(&threads[6], NULL, &conOdd, NULL);
      pthread_create(&threads[7], NULL, &conOdd, NULL);
      threadNum = 8;
      break;
    default:
      std::cout << "Invalid test number" << std::endl;
      return 1;
      break;
    }

  for (int i = 0; i < threadNum; ++i) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
