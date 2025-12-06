#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <ostream>
#include <format>
#include <iostream>

namespace Util {

enum class Part { A, B };

// Loads input for given day/part into vector<string>
std::vector<std::string> LoadInput(int day, Part part);

template<class T>
concept Printable = requires(std::ostream& os, const T& value) {
    { os << value } -> std::same_as<std::ostream&>;
};

template <Printable T>
void ProvideSolution(const T& solution, Part part) {
    const char partChar = (part == Part::A ? 'A' : 'B');
    std::string out = std::format("Solution to part {} is: {}\n", partChar, solution);
    std::cout << out;
}

} // namespace Util
