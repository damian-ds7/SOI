#include <iostream>
#include <deque>
#include <thread>
#include <algorithm>
#include "monitor.h"


std::deque<int> buffer;
int even_count = 0;
int odd_count = 0;

Semaphore buf_sem(1);
Semaphore even_available_sem(0);
Semaphore odd_available_sem(0);
Semaphore produce_sem(1);
Semaphore consume_sem(1);

void producer_even() {
  int number = 0;
  while (true) {
    produce_sem.p();
    buf_sem.p();

    if (even_count < 10) {
      buffer.push_back(number);
      even_count++;
      std::cout << "Produced even: " << number << "\n";
      even_available_sem.v();
      number = (number + 2) % 50;
    }

    buf_sem.v();
    produce_sem.v();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void producer_odd() {
  int number = 1;
  while (true) {
    produce_sem.p();
    buf_sem.p();

    if (even_count > odd_count) {
      buffer.push_back(number);
      odd_count++;
      std::cout << "Produced odd: " << number << "\n";
      odd_available_sem.v();
      number = (number + 2) % 50;
    }

    buf_sem.v();
    produce_sem.v();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void consumer_even() {
  while (true) {
    even_available_sem.p();
    consume_sem.p();
    buf_sem.p();

    if (buffer.size() >= 3) {
      auto it = std::find_if(buffer.begin(), buffer.end(), [](int x) { return x % 2 == 0; });

      int number = *it;
      buffer.erase(it);
      even_count--;
      std::cout << "Consumed even: " << number << "\n";
    }

    buf_sem.v();
    consume_sem.v();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
  }
}

void consumer_odd() {
  while (true) {
    odd_available_sem.p();
    consume_sem.p();
    buf_sem.p();

    if (buffer.size() >= 7) {
      auto it = std::find_if(buffer.begin(), buffer.end(), [](int x) { return x % 2 != 0; });

      int number = *it;
      buffer.erase(it);
      odd_count--;
      std::cout << "Consumed odd: " << number << "\n";
    }

    buf_sem.v();
    consume_sem.v();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
  }
}

int main() {
  std::thread a1(producer_even);
  std::thread a2(producer_odd);
  std::thread b1(consumer_even);
  std::thread b2(consumer_odd);

  a1.join();
  a2.join();
  b1.join();
  b2.join();

  return 0;
}
