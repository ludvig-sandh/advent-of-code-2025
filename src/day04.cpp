#include "utils.hpp"

#include <vector>
#include <string>
#include <utility>
#include <queue>
#include <algorithm>

using Sheet = std::vector<std::string>;
using Tile = std::pair<int, int>;

// Returns a vector with the subset of the eight surrounding tiles that are within bounds
std::vector<Tile> GetSurroundingTiles(int r, int c, int rows, int cols) {
    std::vector<Tile> tiles = {
        {r - 1, c - 1},
        {r    , c - 1},
        {r + 1, c - 1},
        {r + 1, c    },
        {r + 1, c + 1},
        {r    , c + 1},
        {r - 1, c + 1},
        {r - 1, c    }
    };
    auto it = std::remove_if(tiles.begin(), tiles.end(), [rows, cols](auto tile) {
        const auto [r, c] = tile;
        return r < 0 || c < 0 || r >= rows || c >= cols;
    });
    tiles.erase(it, tiles.end());
    return tiles;
}

// Returns the number of paper rolls stored in the eight surrounding tiles
int CountNeighboringPapers(const Sheet& sheet, int r, int c) {
    const int rows = std::ssize(sheet);
    const int cols = std::ssize(sheet.front());
    auto tiles = GetSurroundingTiles(r, c, rows, cols);
    return std::count_if(tiles.begin(), tiles.end(), [&sheet](const Tile& tile) {
        const auto [r, c] = tile;
        return sheet[r][c] == '@';
    });
}

int main() {
    auto sheet = Util::LoadInput(Util::Day(4), Util::Part::A);
    Util::Timer t;

    const int rows = std::ssize(sheet);
    const int cols = std::ssize(sheet.front());

    // First pass: find all initially accessible papers
    int ans = 0;
    std::queue<Tile> candidates;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (sheet[r][c] != '@') {
                continue;
            }
            if (CountNeighboringPapers(sheet, r, c) < 4) {
                candidates.push({r, c});
            }
        }
    }

    Util::ProvideSolution(std::ssize(candidates), Util::Part::A);

    // Iteratively remove paper rolls add surrounding ones to the queue
    while (!candidates.empty()) {
        const auto [r, c] = candidates.front();
        candidates.pop();

        if (sheet[r][c] != '@') {
            continue;
        }

        if (CountNeighboringPapers(sheet, r, c) < 4) {
            ans++;
            sheet[r][c] = '.';
            auto surrounding = GetSurroundingTiles(r, c, rows, cols);
            std::for_each(surrounding.begin(), surrounding.end(), [&sheet, &candidates](const Tile& tile) {
                const auto [r, c] = tile;
                if (sheet[r][c] == '@') {
                    candidates.push(tile);
                }
            });
        }
    }

    Util::ProvideSolution(ans, Util::Part::B);
}