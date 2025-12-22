#include "utils.hpp"

#include <numeric>

int splitCount = 0;

std::vector<long long> computeNextRow(const std::vector<long long>& lastRow, const std::vector<long long>& nextRow) {
    std::vector<long long> newNextRow = nextRow;
    for (int i = 0; i < std::ssize(lastRow); ++i) {
        if (lastRow[i] >= 1 && nextRow[i] == -1) {
            newNextRow[i - 1] += lastRow[i];
            newNextRow[i + 1] += lastRow[i];
            splitCount++;
        }else if (lastRow[i] >= 1) {
            newNextRow[i] += lastRow[i];
        }
    }
    return newNextRow;
}

int main() {
    auto lines = Util::LoadInput(Util::Day(7));
    Util::Timer t;

    std::vector<std::vector<long long>> grid(lines.size());
    std::transform(lines.begin(), lines.end(), grid.begin(), [](const auto& line) {
        std::vector<long long> row(line.size());
        std::transform(line.begin(), line.end(), row.begin(), [](unsigned char c) {
            switch (c)
            {
            case 'S':
                return 1;
            case '^':
                return -1;
            default:
                return 0;
            }
        });
        return row;
    });

    auto lastRow = std::accumulate(grid.begin() + 1, grid.end(), grid.front(), computeNextRow);
    auto numTimelines = std::accumulate(lastRow.begin(), lastRow.end(), 0LL);
    
    Util::ProvideSolution(splitCount, Util::Part::A);
    Util::ProvideSolution(numTimelines, Util::Part::B);
}
