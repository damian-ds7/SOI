#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include <vector>
#include <functional>
#include "monitors.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <testNum>" << std::endl;
    return 1;
  }

  int testNum = std::stoi(argv[1]);
  std::vector<std::thread> threads;

  switch (testNum) {
    case 1:
      std::cout << "1 prod even" << std::endl;
      threads.emplace_back(prodEven);
      break;
    case 2:
      std::cout << "1 prod odd" << std::endl;
      threads.emplace_back(prodOdd);
      break;
    case 3:
      std::cout << "1 cons even" << std::endl;
      threads.emplace_back(conEven);
      break;
    case 4:
      std::cout << "1 cons odd" << std::endl;
      threads.emplace_back(conOdd);
      break;
    case 5:
      std::cout << "1 prod even + 1 prod odd" << std::endl;
      threads.emplace_back(prodEven);
      threads.emplace_back(prodOdd);
      break;
    case 6:
      std::cout << "1 cons even + 1 cons odd" << std::endl;
      threads.emplace_back(conEven);
      threads.emplace_back(conOdd);
      break;
    case 7:
      std::cout << "1 prod even + 1 cons even" << std::endl;
      threads.emplace_back(prodEven);
      threads.emplace_back(conEven);
      break;
    case 8:
      std::cout << "1 prod odd + 1 cons odd" << std::endl;
      threads.emplace_back(prodOdd);
      threads.emplace_back(conOdd);
      break;
    case 9:
      std::cout << "1 prod even + 1 prod odd + 1 cons even + 1 cons odd"
                << std::endl;
      threads.emplace_back(prodEven);
      threads.emplace_back(prodOdd);
      threads.emplace_back(conEven);
      threads.emplace_back(conOdd);
      break;
    case 10:
      std::cout << "2 prod even + 2 prod odd + 2 cons even + 2 cons odd"
                << std::endl;
      threads.emplace_back(prodEven);
      threads.emplace_back(prodEven);
      threads.emplace_back(prodOdd);
      threads.emplace_back(prodOdd);
      threads.emplace_back(conEven);
      threads.emplace_back(conEven);
      threads.emplace_back(conOdd);
      threads.emplace_back(conOdd);
      break;
    default:
      std::cout << "Invalid test number" << std::endl;
      return 1;
  }

  for (auto& thread : threads) {
    thread.join();
  }

  return 0;
}
