
#include "wordCompletion.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>

inline auto get_time() { return std::chrono::high_resolution_clock::now(); }

int main() {
  wordCompletion dict;

  const std::string file_name = "combined.txt";
  const int k = 64;

  std::ifstream word_file{file_name};
  std::string token;
  int count = 0;
  const int max_count = 1000000000;
  const auto benchmark_start = get_time();
  std::cout << "Running benchmark with " << file_name << ", "
            << "k = " << k << std::endl;
  while (word_file >> token && count++ < max_count) {
    dict.access(token);
    const auto completions = dict.getCompletions(token, k);
  }
  const auto benchmark_end = get_time();
  const long long nano_taken =
      std::chrono::nanoseconds(benchmark_end - benchmark_start).count();
  std::cout << nano_taken << "ns / " << count << "ws" << std::endl;
  std::cout << "(" << (nano_taken / count) << " ns/w)" << std::endl;
}
