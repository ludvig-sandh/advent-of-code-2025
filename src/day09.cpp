#include "utils.hpp"

#include <string>
#include <print>
#include <set>

struct Point {
    long long x, y;
    bool operator<(const Point& other) const {
        return (x == other.x)
            ? y < other.y
            : x < other.x;
    }
};

using PointPair = std::pair<Point, Point>;

long long ComputeArea(const PointPair& pp) {
    const auto& [p1, p2] = pp;
    return (std::abs(p2.x - p1.x) + 1) * (std::abs(p2.y - p1.y) + 1);
}

long long ComputeAreaWith1Border(const PointPair& pp) {
    const auto& [p1, p2] = pp;
    return (std::abs(p2.x - p1.x) + 3) * (std::abs(p2.y - p1.y) + 3);
}

std::vector<PointPair> PairUpPoints(const auto& points) {
    // Pair up all points
    std::vector<PointPair> pointPairs;
    for (auto it = points.begin(); it != points.end(); ++it) {
        const auto& pointA = *it;
        std::for_each(it + 1, points.end(), [&pointPairs, &pointA](const auto& pointB) {
            pointPairs.emplace_back(pointA, pointB);
        });
    }

    return pointPairs;
}

template <int offsetX, int offsetY>
Point GetPointOffset(const Point& p) {
    return {p.x + offsetX, p.y + offsetY};
}

bool IsInsidePolygon(const std::vector<Point>& points, const Point p) {
    int numCrossings = 0;
    for (int i = 0; i < std::ssize(points); ++i) {
        int j = (i + 1) % std::ssize(points);
        const auto& red1 = points[i];
        const auto& red2 = points[j];

        if (red1.x != red2.x) {
            continue;
        }
        int lineX = red1.x;
        if (lineX < p.x) {
            continue;
        }

        auto [redTop, redBottom] = std::minmax(red1.y, red2.y);
        if (p.y < redTop || p.y > redBottom) {
            continue;
        }

        numCrossings++;
    }

    return numCrossings % 2 == 1;
}

bool Do2LinesIntersect(const PointPair& line1, const PointPair& line2) {
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
        int j = (i + 1) % std::ssize(points);
        if (Do2LinesIntersect(line, {points[i], points[j]})) {
            return true;
        }
    }
    return false;
}

bool CheckCandidate(const PointPair& DAPair, const std::set<Point>& pointsSet) {
    auto [minX, maxX] = std::minmax(DAPair.first.x, DAPair.second.x);
    auto [minY, maxY] = std::minmax(DAPair.first.y, DAPair.second.y);
    
    const Point topLeft{minX - 1, minY - 1};
    const Point bottomRight{maxX + 1, maxY + 1};
    if (pointsSet.find(topLeft) != pointsSet.end() &&
        pointsSet.find(bottomRight) != pointsSet.end()) {
        return true;
    }
    
    const Point topRight{maxX + 1, minY - 1};
    const Point bottomLeft{minX - 1, maxY + 1};
    if (pointsSet.find(topRight) != pointsSet.end() &&
        pointsSet.find(bottomLeft) != pointsSet.end()) {
        return true;
    }

    return false;
}

int main() {
    auto lines = Util::LoadInput(Util::Day(9), Util::Part::A);
    Util::Timer t;

    // Create a list of points from input
    std::vector<Point> points;
    std::transform(lines.begin(), lines.end(), std::back_inserter(points), [](auto line) {
        auto comma = line.find(',');
        long long x = std::stoll(line.substr(0, comma));
        long long y = std::stoll(line.substr(comma + 1));
        return Point{x, y};
    });

    std::set<Point> pointsSet;
    pointsSet.insert(points.begin(), points.end());

    auto pointPairs = PairUpPoints(points);

    // Sort rectangles by largest area
    std::sort(pointPairs.begin(), pointPairs.end(), [](const auto& pp1, const auto& pp2) {
        return ComputeArea(pp1) > ComputeArea(pp2);
    });

    long long answerA = ComputeArea(pointPairs.front());

    Util::ProvideSolution(answerA, Util::Part::A);
    
    // Find all diagonally adjacent points
    std::vector<Point> DAPoints;
    std::transform(points.begin(), points.end(), std::back_inserter(DAPoints), GetPointOffset<1, 1>);
    std::transform(points.begin(), points.end(), std::back_inserter(DAPoints), GetPointOffset<-1, 1>);
    std::transform(points.begin(), points.end(), std::back_inserter(DAPoints), GetPointOffset<-1, -1>);
    std::transform(points.begin(), points.end(), std::back_inserter(DAPoints), GetPointOffset<1, -1>);

    std::erase_if(DAPoints, [&points](const auto& point) {
        return !IsInsidePolygon(points, point);
    });

    auto DAPairs = PairUpPoints(DAPoints);

    // Sort rectangles by largest area with border
    std::sort(DAPairs.begin(), DAPairs.end(), [](const auto& pp1, const auto& pp2) {
        return ComputeAreaWith1Border(pp1) > ComputeAreaWith1Border(pp2);
    });

    for (const auto& DAPair : DAPairs) {
        // If this rectangle doesn't intersect with any edge, we can add a border of 1 around it and still fit
        const auto& [p1, p2] = DAPair;
        PointPair horizLine1{p1, {p2.x, p1.y}};
        PointPair horizLine2{p2, {p1.x, p2.y}};
        PointPair vertiLine1{p1, {p1.x, p2.y}};
        PointPair vertiLine2{p2, {p2.x, p1.y}};
        if (!DoesLineIntersectInputShape(horizLine1, points) &&
            !DoesLineIntersectInputShape(horizLine2, points) &&
            !DoesLineIntersectInputShape(vertiLine1, points) &&
            !DoesLineIntersectInputShape(vertiLine2, points)) {
            if (CheckCandidate(DAPair, pointsSet)) {
                long long answerB = ComputeAreaWith1Border(DAPair);
                Util::ProvideSolution(answerB, Util::Part::B);
                break;
            }
        }
    }
}
