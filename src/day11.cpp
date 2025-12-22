#include "utils.hpp"

#include <unordered_map>
#include <unordered_set>

using Node = std::string;
using Graph = std::unordered_map<Node, std::vector<Node>>;
using Cache = std::unordered_map<Node, long long>;

static constexpr Node inNode("you");
static constexpr Node outNode("out");
static constexpr Node fftNode("fft");
static constexpr Node dacNode("dac");
static constexpr Node svrNode("svr");

std::pair<Node, std::vector<Node>> CreateNode(std::string line) {
    auto colon = line.find(':');
    auto from = line.substr(0, colon);
    auto tos = Util::SplitString(line.substr(colon + 2), ' ');
    return std::make_pair(from, tos);
}

long long DFS(const Graph& reverseGraph, const Node& node, Cache& cache, const Node& fromNode) {
    if (node == fromNode) {
        return 1;
    }

    if (cache.find(node) != cache.end()) {
        return cache[node];
    }

    if (reverseGraph.find(node) == reverseGraph.end()) {
        return 0;
    }

    long long totalCount = 0;
    for (const Node& neighborNode : reverseGraph.at(node)) {
        totalCount += DFS(reverseGraph, neighborNode, cache, fromNode);
    }
    cache[node] = totalCount;
    return totalCount;
}

long long CountPaths(const Graph& reverseGraph, const Node& fromNode, const Node& toNode) {
    Cache cache;
    return DFS(reverseGraph, toNode, cache, fromNode);
}

int main() {
    auto lines = Util::LoadInput(Util::Day(11), Util::Part::A);
    Util::Timer t;

    Graph graph;
    std::transform(lines.begin(), lines.end(), std::inserter(graph, graph.end()), [](const auto& line) {
        return CreateNode(line);
    });

    Graph reverseGraph;
    for (const auto& [from, tos] : graph) {
        for (const auto& to : tos) {
            reverseGraph[to].push_back(from);
        }
    }

    long long answerA = CountPaths(reverseGraph, inNode, outNode);
    Util::ProvideSolution(answerA, Util::Part::A);

    long long answerB =
        CountPaths(reverseGraph, svrNode, fftNode) *
        CountPaths(reverseGraph, fftNode, dacNode) *
        CountPaths(reverseGraph, dacNode, outNode) + 
        CountPaths(reverseGraph, svrNode, dacNode) *
        CountPaths(reverseGraph, dacNode, fftNode) *
        CountPaths(reverseGraph, fftNode, outNode);
    Util::ProvideSolution(answerB, Util::Part::B);
}