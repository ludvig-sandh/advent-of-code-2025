#include "utils.hpp"

#include <fstream>
#include <stdexcept>
#include <format>
#include <cctype>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <ranges>

namespace Util {

// --- Internal helpers. Not exposed publicly
static std::string ReadFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path.string());
    }

    file.seekg(0, std::ios::end);
    std::string contents(static_cast<size_t>(file.tellg()), '\0');
    file.seekg(0);
    file.read(contents.data(), contents.size());

    return contents;
}

static std::vector<std::string> ReadLines(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path.string());
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    if (!file.eof()) {
        throw std::runtime_error("Error while reading file: " + path.string());
    }

    return lines;
}

// Loads input from a file with name such as "inputs/01/a.txt"
std::vector<std::string> LoadInput(Day day, Part part) {
    const char partChar = (part == Part::A ? 'a' : 'b');

    const std::string dayDirectory = std::format("{:02d}", day.value);
    const std::string filename = std::format("{}.txt", partChar);
    const std::filesystem::path path = std::filesystem::path("inputs") / dayDirectory / filename;

    try {
        return ReadLines(path);
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::format("Failed to read input file '{}': {}", path.string(), e.what()));
    }
}

std::vector<std::string> SplitString(const std::string& str, char splitter) {
    return str
        | std::views::split(splitter)
        | std::views::transform([](auto&& view) {
              return std::string(std::ranges::begin(view), std::ranges::end(view));
          })
        | std::ranges::to<std::vector>();
}

} // namespace Util
