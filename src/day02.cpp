#include "utils.hpp"
#include <utility>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>

using Range = std::pair<long long, long long>;

// Turns "22-50" into ints 22 and 50
Range rangeFromString(const std::string& str) {
    std::vector<std::string> parts = Util::SplitString(str, '-');
    return std::make_pair(std::stoll(parts[0]), std::stoll(parts[1]));
}

// Replicates the digits of an integral. Eg. 123 -> 123123
long long replicate(long long val, int numTimes = 2) {
    std::string s = std::to_string(val);
    std::string out;
    for (int i = 0; i < numTimes; ++i) {
        out.insert(out.end(), s.begin(), s.end());
    }
    if (std::ssize(out) > 18) { // Avoid overflow. No range is this large anyways
        return std::numeric_limits<long long>::max();
    }
    return std::stoll(out);
}

// Splits integer of digits into numCuts parts and keeps first. Sort of inverse of replicate()
long long dereplicate(long long val, int numCuts) {
    std::string s = std::to_string(val);
    s.erase(s.begin() + s.size() / numCuts, s.end());
    if (s.empty()) {
        return 1;
    }
    return std::stoll(s);
}

int main() {
    const std::string input = Util::LoadInput(Util::Day(2), Util::Part::A)[0];
    Util::Timer t;

    // Parse into a vector of ranges represented as pairs (from, to)
    const auto rangeStrings = Util::SplitString(input, ',');
    std::vector<Range> ranges(rangeStrings.size());
    std::transform(rangeStrings.begin(), rangeStrings.end(), ranges.begin(), rangeFromString);

    constexpr int minCutsAllowed = 2;
    constexpr int maxCutsAllowed = 12;
    std::vector<int> allowedCuts(maxCutsAllowed - minCutsAllowed + 1);
    std::ranges::iota(allowedCuts, minCutsAllowed);

    std::vector<std::vector<long long>> invalid(allowedCuts.size());
    std::transform(allowedCuts.begin(), allowedCuts.end(), invalid.begin(), [&ranges](int cuts) {
        // For this number of "cuts", find all invalid IDs
        std::vector<long long> results;
        std::for_each(ranges.begin(), ranges.end(), [cuts, &results](const auto& range) {
            // For this range and this number of cuts, check all candidates
            const auto [from, to] = range;
            long long lower = dereplicate(from, cuts);
            long long upper = std::ceil(std::pow(to, 1.0 / cuts));
            if (replicate(upper, cuts) < from) { // Optimization: range doesn't overlap with our bounds
                return;
            };
            for (long long i = lower; i <= upper; i++) {
                long long candidate = replicate(i, cuts);
                if (candidate > to) { // Optimization: all candidates within bounds checked
                    break;
                }
                if (from <= candidate && candidate <= to) {
                    // Candidate was within bounds
                    results.push_back(candidate);
                }
            }
        });
        return results;
    });

    // A's solution is the sum of invalid IDs with 2 cuts
    long long solA = std::accumulate(invalid.front().begin(), invalid.front().end(), 0LL);
    Util::ProvideSolution(solA, Util::Part::A);

    // B's solution is the sum of invalid IDs with any number of cuts
    std::vector<long long> flat;
    for (auto& v : invalid) flat.insert(flat.end(), v.begin(), v.end());
    std::sort(flat.begin(), flat.end());
    auto it = std::unique(flat.begin(), flat.end());

    long long solB = std::accumulate(flat.begin(), it, 0LL);
    Util::ProvideSolution(solB, Util::Part::B);
}