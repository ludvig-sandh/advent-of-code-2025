#include "utils.hpp"

#include <sstream>
#include <functional>
#include <iterator>
#include <numeric>
#include <algorithm>

std::vector<std::string> transpose(const std::vector<std::string>& matrix) {
    std::vector<std::string> out;
    for (int i = 0; i < std::ssize(matrix.front()); ++i) {
        out.push_back({});
        std::transform(matrix.begin(), matrix.end(), std::back_inserter(out.back()), [i](const auto& row) {
            return row[i];
        });
    }
    return out;
}

struct Problem {
    std::vector<std::string> ops;
    std::function<long long(long long, long long)> operatorFn;
    long long identity;

    Problem(std::vector<std::string> rows) {
        std::copy(rows.begin(), rows.end() - 1, std::back_inserter(ops));
        if (rows.back()[0] == '*') {
            operatorFn = std::multiplies<>();
            identity = 1;
        }else {
            operatorFn = std::plus<>();
            identity = 0;
        }
    }

    long long SolveA() const noexcept {
        return SolveGeneral(ops);
    }

    long long SolveB() const noexcept {
        auto cols = transpose(ops);
        return SolveGeneral(cols);
    }

private:
    long long SolveGeneral(const std::vector<std::string>& opsStrings) const noexcept {
        std::vector<long long> opsInt(opsStrings.size());
        std::transform(opsStrings.begin(), opsStrings.end(), opsInt.begin(), [](const auto& s) {
            return std::stoll(s);
        });
        return std::accumulate(opsInt.begin(), opsInt.end(), identity, operatorFn);
    }
};

bool containsOnlySpaces(const std::string& col) {
    return std::ranges::all_of(col, [](unsigned char c) {
        return std::isspace(c);
    }); 
}

int main() {
    auto rows = Util::LoadInput(Util::Day(6));
    Util::Timer t;

    auto columns = transpose(rows);

    std::vector<Problem> problems;
    auto lastBlankLine = columns.begin();
    do {
        auto blankLine = std::find_if(lastBlankLine, columns.end(), containsOnlySpaces);
        std::vector<std::string> problemColumns;
        std::copy(lastBlankLine, blankLine, std::back_inserter(problemColumns));
        problems.emplace_back(transpose(problemColumns));
        if (blankLine == columns.end()) {
            break;
        }
        lastBlankLine = blankLine + 1;
    }while (true);

    std::vector<long long> solutionsA, solutionsB;
    std::transform(problems.begin(), problems.end(), std::back_inserter(solutionsA), [](const auto& problem) {
        return problem.SolveA();
    });
    std::transform(problems.begin(), problems.end(), std::back_inserter(solutionsB), [](const auto& problem) {
        return problem.SolveB();
    });
    
    Util::ProvideSolution(std::accumulate(solutionsA.begin(), solutionsA.end(), 0LL), Util::Part::A);
    Util::ProvideSolution(std::accumulate(solutionsB.begin(), solutionsB.end(), 0LL), Util::Part::B);
}