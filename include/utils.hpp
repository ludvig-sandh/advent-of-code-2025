#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <ostream>
#include <format>
#include <iostream>
#include <print>
#include <chrono>

namespace Util {

enum class Part { A, B };

struct Day {
    int value;

    explicit Day(int d) : value(d) {
        if (d < 1 || d > 25) {
            throw std::out_of_range("Day must be between 1 and 25");
        }
    }
};

class Timer {
public:
    explicit Timer()
        : m_start(std::chrono::steady_clock::now()) {}

    ~Timer() {
        const auto end   = std::chrono::steady_clock::now();
        const auto dur   = end - m_start;
        const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

        std::println("Execution time: {}µs", micros);
    }

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

private:
    std::chrono::steady_clock::time_point m_start;
};

// Loads input for given day/part into vector<string>
std::vector<std::string> LoadInput(Day day, Part part);

template<class T>
concept Formattable = requires {
    typename std::formatter<T>; // formatter exists
};

// Helper for formatting solution to a part
void ProvideSolution(Formattable auto solution, Part part) {
    const char partChar = (part == Part::A ? 'A' : 'B');
    std::println("Solution to part {} is: {}", partChar, solution);
}

} // namespace Util
