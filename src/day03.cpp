#include "utils.hpp"
#include <algorithm>
#include <vector>
#include <numeric>

using Bank = std::string;

// Fast lookup table for powers of 10
static constexpr long long POW10[] = {
    1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL,
    1000000LL, 10000000LL, 100000000LL, 1000000000LL,
    10000000000LL, 100000000000LL
};

// Greedy solution works. Just pick the next largest digit such that we don't run out of space
long long findMaxJoltage(const Bank& bank, int batteries, auto firstIt) {
    if (batteries == 0) {
        return 0;
    }
    auto maxIt = std::max_element(firstIt, bank.end() - batteries + 1);
    return (*maxIt - '0') * POW10[batteries - 1] + findMaxJoltage(bank, batteries - 1, maxIt + 1);
}

// Computes the total output joltage given the number of batteries we're allowed to turn on
long long computeJolts(const auto lines, int batteries) {
    std::vector<long long> jolts(lines.size());
    std::transform(lines.begin(), lines.end(), jolts.begin(), [batteries](const Bank& bank) {
        return findMaxJoltage(bank, batteries, bank.begin());
    });
    return std::accumulate(jolts.begin(), jolts.end(), 0LL);
}

int main() {
    const auto lines = Util::LoadInput(Util::Day(3), Util::Part::A);
    Util::Timer t;

    Util::ProvideSolution(computeJolts(lines, 2), Util::Part::A);
    Util::ProvideSolution(computeJolts(lines, 12), Util::Part::B);
}
