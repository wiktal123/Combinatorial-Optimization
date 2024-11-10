#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <random>
#include <chrono>
#include <algorithm>

// initialize global variables
struct Edge
{
    int to, weight;
};

using Graph = std::vector<std::vector<Edge>>;
using PII = std::pair<int, int>;
const int INF = std::numeric_limits<int>::max();
int min_dist = INF;

int choose_random(const std::vector<PII> &nodes)
{
    int n = nodes.size();
    if (n == 1) return nodes[0].second;

    // creating weights for each node
    std::vector<double> weights(n);
    double sum_weights = 0;

    for (int i = 0; i < n; ++i)
    {
        weights[i] = 1.0 / (nodes[i].first + 1);
        sum_weights += weights[i];
    }

    // normalizing weights
    for (int i = 0; i < n; ++i)
    {
        weights[i] /= sum_weights;
    }

    // random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist(weights.begin(), weights.end());

    int index = dist(gen);
    return nodes[index].second;
}

std::pair<std::vector<int>, int> dijkstra(const Graph &graph, std::vector<bool> &visited, int start, int end, int p)
{
    int n = graph.size();
    std::vector<int> dist(n, INF), prev(n, -1);
    std::priority_queue<PII, std::vector<PII>, std::greater<PII>> pq;
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty())
    {
        std::vector<PII> nodes;
        // Create a list of best p nodes
        for (int i = 0; i < p && !pq.empty(); ++i)
        {
            nodes.push_back(pq.top());
            pq.pop();
        }

        // Choose a random node from best p nodes
        int u = choose_random(nodes);
        visited[u] = true;

        if (u == end) break;

        // Reinserting nodes into the priority queue
        for (int i = 0; i < nodes.size(); ++i)
        {
            if (nodes[i].second != u) pq.push(nodes[i]);
        }

        //classic dijkstra loop, only difference is that we are checking if the distance is not greater than already found min_dist
        for (const auto &edge : graph[u])
        {
            int v = edge.to;
            int weight = edge.weight;
            if (dist[u] + weight < dist[v] && dist[u] + weight < min_dist && !visited[v])
            {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    std::vector<int> path;

    // If there is no path from start to end we return empty path
    if (dist[end] == INF)
        return {path, INF};

    // Reconstructing the path
    for (int at = end; at != -1; at = prev[at])
    {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    return {path, dist[end]};
}

int main()
{
    // initialize timer
    auto start_time = std::chrono::steady_clock::now();
    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    // Input reading
    int N, M, A;
    std::cin >> N >> M >> A;

    // initialize graph
    Graph graph(N * M);
    for (int i = 0; i < A; ++i)
    {
        int u, v, w;
        std::cin >> u >> v >> w;
        graph[u].push_back({v, w});
    }

    int a = 0;
    int b = M - 1;
    int c = (N - 1) * M;

    // Randomized Dijkstra with threshold of 10 successful attempts or 30 seconds of runtime

    // p denotes the amount of best nodes to pick random next node from
    int attempts = 0, p = 0;
    std::vector<int> best_path;

    // main loop
    while (attempts < 10 && (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time).count() < 29))
    {
        // initialize local variables
        int total_dist = 0;
        ++p;
        std::vector<bool> visited(N * M, false);

        // setting node c to true to make sure it is not visited before node b
        visited[c] = true;

        // Finding shortest path from a to b
        auto [path1, dist1] = dijkstra(graph, visited, a, b, p);
        if (path1.empty() || path1.back() != b) continue;

        total_dist += dist1;

        // Setting nodes which already are in current path to true
        std::fill(visited.begin(), visited.end(), false);
        for (int node : path1) visited[node] = true;

        // Finding shortest path from b to c
        auto [path2, dist2] = dijkstra(graph, visited, b, c, p);
        if (path2.empty() || path2.back() != c) continue;

        total_dist += dist2;

        // Concatenating paths
        path1.pop_back();
        path1.insert(path1.end(), path2.begin(), path2.end());

        // checking whether the path is better than the best path found so far
        if (total_dist < min_dist)
        {
            min_dist = total_dist;
            best_path = path1;
            ++attempts;
        }
    }
    // If no path is found
    if (best_path.empty())
    {
        std::cout << -1 << '\n';
    }
    else // Output the best path
    {
        std::cout << best_path.size() << '\n';
        for (size_t i = 0; i < best_path.size(); ++i)
        {
            std::cout << best_path[i] << (i + 1 < best_path.size() ? ' ' : '\n');
        }
    }

    return 0;
}
