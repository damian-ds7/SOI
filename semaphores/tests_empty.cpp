#include <pthread.h>
#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include "semaphores.h"

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
      std::cout << "1 prod even + 1 prod odd + 1 cons even + 1 cons odd"
                << std::endl;
      pthread_create(&threads[0], NULL, &prodEven, NULL);
      pthread_create(&threads[1], NULL, &prodOdd, NULL);
      pthread_create(&threads[2], NULL, &conEven, NULL);
      pthread_create(&threads[3], NULL, &conOdd, NULL);
      threadNum = 4;
      break;
    case 10:
      std::cout << "2 prod even + 2 prod odd + 2 cons even + 2 cons odd"
                << std::endl;
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
