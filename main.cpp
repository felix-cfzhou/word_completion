#include "word_completion.h"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cassert>

inline auto get_time() { return std::chrono::high_resolution_clock::now(); }

int main(int, char **argv) {
    WordCompletion dict;
    const std::string file_name{argv[1]};
    const int k = 16;

    std::ifstream word_file{file_name};
    std::string token;
    int count = 0;
    long long running = 0;
    while (word_file >> token) {
        const auto benchmark_start = get_time();
        const auto &result = dict.getCompletions(token, k);
        const auto &id =
            dict.access(token);
        const auto benchmark_end = get_time();
        count++;
        running += std::chrono::nanoseconds(benchmark_end - benchmark_start).count();
        
        std::cout << token << " " << k << std::endl;
        std::cout << id << std::endl;
        std::cout << result.size() << " " << result[0].size() << std::endl;
        for (const auto &lst : result) {
            assert(lst.size() == k);
            for (const auto &id1 : lst) {
                std::cout << id1 << " ";
            }
            std::cout << std::endl;
        }
       
    }
    std::cout << "[END] " << static_cast<long long>(running / count) << std::endl;
}
