#include "utils.hpp"

struct Point {
    long long x, y;
};

using PointPair = std::pair<Point, Point>;

long long ComputeArea(const PointPair& rect) {
    const auto& [p1, p2] = rect;
    return (std::abs(p2.x - p1.x) + 1) * (std::abs(p2.y - p1.y) + 1);
}

std::vector<PointPair> PairUpAndSortByArea(const auto& points) {
    // Pair up all points
    std::vector<PointPair> pointPairs;
    for (auto it = points.begin(); it != points.end(); ++it) {
        for (auto jt = std::next(it); jt != points.end(); ++jt) {
            pointPairs.emplace_back(*it, *jt);
        }
    }

    // Sort rectangles by largest area
    std::sort(pointPairs.begin(), pointPairs.end(), [](const auto& rect1, const auto& rect2) {
        return ComputeArea(rect1) > ComputeArea(rect2);
    });

    return pointPairs;
}

bool DoLinesIntersect(const PointPair& line1, const PointPair& line2) {
    auto [min1X, max1X] = std::minmax(line1.first.x, line1.second.x);
    auto [min2X, max2X] = std::minmax(line2.first.x, line2.second.x);
    auto [min1Y, max1Y] = std::minmax(line1.first.y, line1.second.y);
    auto [min2Y, max2Y] = std::minmax(line2.first.y, line2.second.y);
    if (max1X < min2X || min1X > max2X) {
        return false;
    }
    if (max1Y < min2Y || min1Y > max2Y) {
        return false;
    }
    return true;
}

bool DoesLineIntersectInputShape(const PointPair& line, const std::vector<Point>& points) {
    for (int i = 0; i < std::ssize(points); ++i) {
        int j = (i == std::ssize(points) - 1) ? 0 : i + 1; // j = (i + 1) mod #points
        if (DoLinesIntersect(line, {points[i], points[j]})) {
            return true;
        }
    }
    return false;
}

int main() {
    auto lines = Util::LoadInput(Util::Day(9));
    Util::Timer t;

    // Create a list of points from input
    std::vector<Point> points;
    std::transform(lines.begin(), lines.end(), std::back_inserter(points), [](auto line) {
        auto comma = line.find(',');
        long long x = std::stoll(line.substr(0, comma));
        long long y = std::stoll(line.substr(comma + 1));
        return Point{x, y};
    });

    auto rectangles = PairUpAndSortByArea(points);

    // Since rectangles are ordered by area (decreasing), pick the first one
    long long answerA = ComputeArea(rectangles.front());
    Util::ProvideSolution(answerA, Util::Part::A);

    for (const auto& rectangle : rectangles) {
        // Idea: Shrink rectangle by 1. If this rectangle doesn't intersect with any edge of the input shape,
        // expanding it again will be safe (meaning it will only contain red/green tiles).
        const auto& [p1, p2] = rectangle;
        auto [minX, maxX] = std::minmax(p1.x, p2.x);
        auto [minY, maxY] = std::minmax(p1.y, p2.y);

        // It's enough to only check the rectangle borders (4 lines)
        PointPair horizLine1{{minX + 1, minY + 1}, {maxX - 1, minY + 1}};
        PointPair horizLine2{{minX + 1, maxY - 1}, {maxX - 1, maxY - 1}};
        PointPair vertiLine1{{minX + 1, minY + 1}, {minX + 1, maxY - 1}};
        PointPair vertiLine2{{maxX - 1, minY + 1}, {maxX - 1, maxY - 1}};

        if (DoesLineIntersectInputShape(horizLine1, points)) continue;
        if (DoesLineIntersectInputShape(horizLine2, points)) continue;
        if (DoesLineIntersectInputShape(vertiLine1, points)) continue;
        if (DoesLineIntersectInputShape(vertiLine2, points)) continue;

        // Successful candidate. Pick first and break since rectangles are ordered by area
        Util::ProvideSolution(ComputeArea(rectangle), Util::Part::B);
        break;
    }
}
