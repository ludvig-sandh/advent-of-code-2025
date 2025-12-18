#include "utils.hpp"

#include <string>
#include <sstream>
#include <utility>
#include <cmath>
#include <numeric>
#include <array>

struct Point {
    int x, y, z, id;

    Point(std::string description, int id) : id(id) {
        std::stringstream ss{description};
        std::string token;
        std::getline(ss, token, ',');
        x = std::stoi(token);
        std::getline(ss, token, ',');
        y = std::stoi(token);
        std::getline(ss, token, ',');
        z = std::stoi(token);
    }
};

struct UnionFind {
	std::vector<int> e;
	UnionFind(int n) : e(n, 1) {}
	bool sameSet(int a, int b) { return find(a) == find(b); }
	int size(int x) { return e[-find(x)]; }
	int find(int x) { return e[x] > 0 ? -x : e[x] = find(-e[x]); }
	bool join(int a, int b) {
		a = -find(a), b = -find(b);
		if (a == b) return false;
		if (e[a] < e[b]) std::swap(a, b);
		e[a] += e[b]; e[b] = -a;
		return true;
	}
};

double DistBetweenPoints(const auto& pointPair) {
    const auto& [pointA, pointB] = pointPair;
    return std::hypot(
        pointA.x - pointB.x,
        pointA.y - pointB.y,
        pointA.z - pointB.z
    );
}

long long GetProductOfLargestThreeValues(const std::vector<int>& list) {
    std::array<int, 3> top;
    std::partial_sort_copy(list.begin(), list.end(), top.begin(), top.end(), std::greater<int>());
    return std::accumulate(top.begin(), top.end(), 1LL, std::multiplies<int>());
}

int main() {
    auto lines = Util::LoadInput(Util::Day(8), Util::Part::A);
    Util::Timer t;

    // Create a list of points from input
    std::vector<Point> points;
    std::transform(lines.begin(), lines.end(), std::back_inserter(points), [&points](auto line) {
        return Point(line, std::ssize(points)); // Give each point a unique id
    });

    // Pair up all points
    std::vector<std::pair<Point, Point>> pointPairs;
    for (auto it = points.begin(); it != points.end(); ++it) {
        const auto& pointA = *it;
        std::for_each(it + 1, points.end(), [&pointPairs, &pointA](const auto& pointB) {
            pointPairs.emplace_back(pointA, pointB);
        });
    }

    // Sort them by closest euclidean distance
    std::sort(pointPairs.begin(), pointPairs.end(), [](const auto& pair1, const auto& pair2) {
        return DistBetweenPoints(pair1) < DistBetweenPoints(pair2);
    });

    // Connect pairs of points one by one
    UnionFind uf(std::ssize(lines));
    for (int i = 0; true; i++) {
        if (i == 1000) {
            // At the 1000:th connection, find the largest three unions
            Util::ProvideSolution(GetProductOfLargestThreeValues(uf.e), Util::Part::A);
        }

        const auto& [pointA, pointB] = pointPairs[i];
        uf.join(pointA.id, pointB.id);

        // Check if the union is full
        if (uf.size(0) == std::ssize(lines)) {
            Util::ProvideSolution((long long)pointA.x * pointB.x, Util::Part::B);
            break;
        }
    }
}