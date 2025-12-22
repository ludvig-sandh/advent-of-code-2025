#include "utils.hpp"

#include <print>
#include <ranges>
#include <numeric>

using Range = std::pair<long long, long long>;

// Turns "22-50" into ints 22 and 50
Range RangeFromString(const std::string& str) {
    std::vector<std::string> parts = Util::SplitString(str, '-');
    return std::make_pair(std::stoll(parts[0]), std::stoll(parts[1]));
}

// Merges all the ranges so they are non-overlapping
std::vector<Range> MergeRanges(std::vector<Range>& ranges) {
    // Sort ranges based on left end
    std::sort(ranges.begin(), ranges.end(), [](auto& a, auto& b) {
        return a.first < b.first;
    });

    // Merge all consecutive ranges that overlap
    std::vector<Range> merged = {ranges[0]};
    std::for_each(ranges.begin() + 1, ranges.end(), [&merged](const auto& range) {
        auto& lastHi = merged.back().second;
        const auto [lo, hi] = range;
        if (lo > lastHi) {
            // Disjoint range
            merged.push_back(range);
        }else {
            // Overlaps; choose the highest right end
            lastHi = std::max(lastHi, hi);
        }
    });

    return merged;
}

int main() {
    const auto lines = Util::LoadInput(Util::Day(5));
    Util::Timer t;

    // The blank line splits the ranges from the ingredients
    auto blankLineIt = std::find(lines.begin(), lines.end(), "");
    std::vector<Range> ranges(blankLineIt - lines.begin());
    std::vector<long long> ingredients(lines.end() - blankLineIt + 1);
    std::transform(lines.begin(), blankLineIt, ranges.begin(), RangeFromString);
    std::transform(blankLineIt + 1, lines.end(), ingredients.begin(), [](auto& s) { return std::stoll(s); });

    auto mergedRanges = MergeRanges(ranges);

    // Count the number of ingredients that lie within any range
    int ans = std::count_if(ingredients.begin(), ingredients.end(), [&mergedRanges](const auto ingredient) {
        // Use binary search to find the first range that starts to the right of our ingredient
        auto it = std::upper_bound(mergedRanges.begin(), mergedRanges.end(), ingredient, [](long long val, const auto& range) {
            return val < range.first;
        });
        if (it == mergedRanges.begin()) {
            // Ingredient is smaller than all intervals
            return false;
        }

        // If the ingredient is contained in any range, it must be in the one before
        const auto& [lo, hi] = *std::prev(it);
        return lo <= ingredient && ingredient <= hi;
    });

    Util::ProvideSolution(ans, Util::Part::A);

    auto rangeSizes = mergedRanges | std::views::transform([](const Range& range) { return range.second - range.first + 1; });
    auto totalRangeSize = std::accumulate(rangeSizes.begin(), rangeSizes.end(), 0LL);
    Util::ProvideSolution(totalRangeSize, Util::Part::B);
}