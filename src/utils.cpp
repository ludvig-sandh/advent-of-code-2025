#include "utils.hpp"

#include <fstream>
#include <stdexcept>
#include <format>
#include <cctype>
#include <iostream>

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

// --- Public AoC loader
std::vector<std::string> LoadInput(int day, Part part) {
    const char partChar = (part == Part::A ? 'a' : 'b');

    const std::string dayDirectory = std::format("{:02d}", day);
    const std::string filename = std::format("{}.txt", partChar);
    const std::filesystem::path path = std::filesystem::path("inputs") / dayDirectory / filename;

    try {
        return ReadLines(path);
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::format("Failed to read input file '{}': {}", path.string(), e.what()));
    }
}



} // namespace Util
