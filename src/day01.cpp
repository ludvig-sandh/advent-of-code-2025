#include "utils.hpp"
#include <iostream>
#include <iterator>
#include <string>
#include <algorithm>
#include <charconv>
#include <ranges>
#include <numeric>
#include <utility>

// Counts the number of zeroes crossed (mod 100) in the range (from, to]
int countZeroDials(int from, int to) {
    // Order by ascending
    if (from < to) {
        std::swap(to, from);

        // Account for bounds: do [from, to) since we reverse the range
        from--;
        to--;
    }

    // Shift both numbers into positive range
    int min = std::min(from, to);
    if (min < 0) {
        int shift = (-min / 100 + 1) * 100;
        from += shift;
        to += shift;
    }

    // The difference in hundreds is the number of zeroes we cross
    return std::abs(from / 100 - to / 100);
}

// Turns eg. "R5" -> 5, and "L5" -> -5 (R = positive delta, L = negative)
int convertLineToDelta(const std::string_view& line) {
    int mag = 0;
    auto [ptr, ec] = std::from_chars(line.data() + 1, line.data() + line.size(), mag);
    if (ec != std::errc{}) throw std::runtime_error("Bad integer");
    return line[0] == 'L' ? -mag : mag;
}

int main() {
    const auto lines = Util::LoadInput(Util::Day(1));
    Util::Timer t;
    
    // Compute the deltas (turn eg. L14 into -14)
    std::vector<int> deltas(lines.size() + 1);
    deltas[0] = 50; // We start at dial 50
    std::transform(lines.begin(), lines.end(), deltas.begin() + 1, convertLineToDelta);

    // Compute all the dials we see
    std::vector<int> dials(deltas.size());
    std::partial_sum(deltas.begin(), deltas.end(), dials.begin());

    // Count the dials that show 0 when accounted for modular arithmetic
    int count = std::count_if(dials.begin(), dials.end(), [](auto e) {
        return e % 100 == 0;
    });
    Util::ProvideSolution(count, Util::Part::A);

    // Count the number of zeroes seen between every pair of dials
    auto pairs = std::views::pairwise(dials);
    std::vector<int> zeroCounts(lines.size());
    std::transform(pairs.begin(), pairs.end(), zeroCounts.begin(), [](const auto& pair) {
        const auto [from, to] = pair;
        return countZeroDials(from, to);
    });

    // Accumulate the count
    Util::ProvideSolution(
        std::accumulate(zeroCounts.begin(), zeroCounts.end(), 0),
        Util::Part::B
    );
}