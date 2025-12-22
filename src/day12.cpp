#include "utils.hpp"

#include <span>
#include <sstream>
#include <queue>
#include <algorithm>

static constexpr int shapeSize = 3;
static constexpr int numInputShapes = 6;

using Shape = std::array<std::array<bool, shapeSize>, shapeSize>;
using Requirements = std::vector<int>;

Shape RotateShapeCW(const Shape& shape) {
    Shape out;
    for (size_t row{0}; row < shapeSize; ++row) {
        for (size_t col{0}; col < shapeSize; ++col) {
            out[row][col] = shape[col][shapeSize - 1 - row];
        }
    }
    return out;
}

Shape FlipVertically(const Shape& shape) {
    Shape out;
    for (size_t col{0}; col < shapeSize; ++col) {
        for (size_t row{0}; row < shapeSize; ++row) {
            out[row][col] = shape[row][shapeSize - 1 - col];
        }
    }
    return out;
}

std::vector<Shape> GetAllRotations(const Shape& shape) {
    std::vector<Shape> rotations{shape};
    Shape rotated = shape;
    for (int i{0}; i < 3; ++i) {
        rotated = RotateShapeCW(shape);
        rotations.push_back(rotated);
    }
    return rotations;
}

std::vector<Shape> GetAllVariants(const Shape& shape) {
    std::vector<Shape> variants;
    auto rotations = GetAllRotations(shape);
    auto flippedRotations = GetAllRotations(FlipVertically(shape));

    variants.insert(variants.begin(), rotations.begin(), rotations.end());
    variants.insert(variants.end(), flippedRotations.begin(), flippedRotations.end());

    // Remove duplicates
    std::sort(variants.begin(), variants.end());
    auto eraseIt = std::unique(variants.begin(), variants.end());
    variants.erase(eraseIt, variants.end());

    return variants;
}

Shape ParseShape(std::span<std::string> lines) {
    Shape shape;
    for (size_t row{0}; row < lines.size(); ++row) {
        for (size_t col{0}; col < lines.size(); ++col) {
            shape[row][col] = lines[row][col] == '#';
        }
    }
    return shape;
}

struct Region {
    std::vector<std::vector<bool>> grid;

    Region(int rows, int cols) : grid(rows, std::vector<bool>(cols, false)) {}

    bool CanFitShape(const Shape& shape, int row, int col) const {
        for (size_t rowOffset{0}; rowOffset < shapeSize; ++rowOffset) {
            for (size_t colOffset{0}; colOffset < shapeSize; ++colOffset) {
                if (shape[rowOffset][colOffset] && grid[row + rowOffset][col + colOffset]) {
                    return false;
                }
            }
        }
        return true;
    }

    int CalculateInsertionScore(const Shape& shape, int row, int col) const {
        int rows = std::ssize(grid);
        int cols = std::ssize(grid[0]);
        int score{0};
        std::array<int, 4> drs{1, 0, -1, 0};
        std::array<int, 4> dcs{0, 1, 0, -1};
        for (size_t rowOffset{0}; rowOffset < shapeSize; ++rowOffset) {
            for (size_t colOffset{0}; colOffset < shapeSize; ++colOffset) {
                if (!shape[rowOffset][colOffset]) {
                    continue;
                }

                // For each surrounding tile, if it is a border or an already set tile,
                // add a point
                for (size_t i{0}; i < 4; ++i) {
                    int newRow = row + rowOffset + drs[i];
                    int newCol = col + colOffset + dcs[i];
                    if (newRow < 0 || newCol < 0 || newRow >= rows || newCol >= cols) {
                        ++score;
                    }else if (grid[newRow][newCol]) {
                        ++score;
                    }
                }
            }
        }
        return score;
    }

    void InsertShape(const Shape& shape, int row, int col) {
        for (size_t rowOffset{0}; rowOffset < shapeSize; ++rowOffset) {
            for (size_t colOffset{0}; colOffset < shapeSize; ++colOffset) {
                if (shape[rowOffset][colOffset]) {
                    grid[row + rowOffset][col + colOffset] = true;
                }
            }
        }
    }
};

struct Puzzle {
    int rows, cols;
    Requirements requirements;

    Puzzle(std::string line) {
        auto x = line.find('x');
        auto colon = line.find(':');
        rows = std::stoi(line.substr(0, x));
        cols = std::stoi(line.substr(x + 1, colon));
        std::stringstream ss{line.substr(colon + 1)};
        for (int shapeIdx{0}; shapeIdx < numInputShapes; ++shapeIdx) {
            int requirement;
            ss >> requirement;
            requirements.push_back(requirement);
        }
    }
};

struct QueueElement {
    int score; // Used for priority. The higher the better.
    Region region;
    Requirements requirementsLeft;

    QueueElement(const Puzzle& puzzle)
        : score(0)
        , region(puzzle.rows, puzzle.cols)
        , requirementsLeft(puzzle.requirements)
    {}

    bool operator<(const QueueElement& rhs) const {
        return score < rhs.score;
    }
};

std::vector<Puzzle> LoadPuzzles(std::vector<std::string> input) {
    std::vector<Puzzle> puzzles;
    for (size_t shapeLine{numInputShapes * 5}; shapeLine < input.size(); ++shapeLine) {
        puzzles.push_back(Puzzle(input[shapeLine]));
    }
    return puzzles;
}

std::vector<Shape> LoadShapes(std::vector<std::string> input) {
    std::vector<Shape> shapes;
    for (int shapeIdx{0}; shapeIdx < numInputShapes; ++shapeIdx) {
        auto startLine = input.begin() + 5 * shapeIdx + 1;
        auto endLine = input.begin() + 5 * shapeIdx + 4;
        std::span<std::string> shapeLines(startLine, endLine);
        shapes.push_back(ParseShape(shapeLines));
    }
    return shapes;
}

int CountTilesInShape(const Shape& shape) {
    int count{0};
    for (size_t row{0}; row < shapeSize; ++row) {
        for (size_t col{0}; col < shapeSize; ++col) {
            if (shape[row][col]) {
                count++;
            }
        }
    }
    return count;
}

int CountRequiredTotalTiles(const std::vector<Shape>& shapes, const Puzzle& puzzle) {
    int totalTiles{0};
    for (int shapeIdx{0}; shapeIdx < numInputShapes; ++shapeIdx) {
        int requiredNum = puzzle.requirements[shapeIdx];
        totalTiles += requiredNum * CountTilesInShape(shapes[shapeIdx]);
    }
    return totalTiles;
}

bool IsPuzzleSolvable(const std::vector<Shape>& shapes, const Puzzle& puzzle) {
    int requiredTiles = CountRequiredTotalTiles(shapes, puzzle);
    if (requiredTiles > puzzle.rows * puzzle.cols) {
        return false; // Impossible
    }

    std::vector<std::vector<Shape>> shapeVariants;
    for (int shapeIdx{0}; shapeIdx < numInputShapes; ++shapeIdx) {
        shapeVariants.push_back(GetAllVariants(shapes[shapeIdx]));
    }

    Region emptyRegion{puzzle.rows, puzzle.cols};
    std::priority_queue<QueueElement> pq;
    pq.push(QueueElement(puzzle));
    while (!pq.empty()) {
        auto qElem = pq.top();
        pq.pop();

        bool requirementsMet = true;
        for (int shapeIdx{0}; shapeIdx < numInputShapes; ++shapeIdx) {
            if (qElem.requirementsLeft[shapeIdx] == 0) {
                continue; // We are not required to insert more of this shape
            }
            requirementsMet = false;

            bool couldFitAnyShape = false;
            for (const Shape& shapeVariant : shapeVariants[shapeIdx]) {
                for (size_t row{0}; row < puzzle.rows - shapeSize + 1; ++row) {
                    for (size_t col{0}; col < puzzle.cols - shapeSize + 1; ++col) {
                        if (!qElem.region.CanFitShape(shapeVariant, row, col)) {
                            continue;
                        }

                        // Shape variant fits here. Compute the score of inserting it
                        auto fitScore = qElem.region.CalculateInsertionScore(shapeVariant, row, col);
                        if (fitScore < 3) {
                            continue;
                        }

                        auto newElement = qElem;
                        newElement.score += fitScore;
                        newElement.requirementsLeft[shapeIdx]--;
                        newElement.region.InsertShape(shapeVariant, row, col);
                        pq.push(newElement);
                        
                        couldFitAnyShape = true;
                    }
                }
            }

            if (!couldFitAnyShape) {
                return false;
            }
        }

        if (requirementsMet) {
            return true;
        }
    }

    return false;
}

int main() {
    auto lines = Util::LoadInput(Util::Day(12), Util::Part::A);
    Util::Timer t;

    auto shapes = LoadShapes(lines);
    auto puzzles = LoadPuzzles(lines);

    int solvableCount = 0;
    for (const auto& puzzle : puzzles) {
        auto solvable = IsPuzzleSolvable(shapes, puzzle);
        std::cout << std::boolalpha << solvable << "\n";
    }
    Util::ProvideSolution(solvableCount, Util::Part::A);
}
