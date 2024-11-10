#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

struct Edge
{
    int to, weight;
};

using Graph = std::vector<std::vector<Edge>>;
const int INF = INT_MAX;

int bidirectional_dijkstra(int &from, int &to, const Graph &graph)
{
    /*
    the idea is to run two Dijkstra's algorithms from the source and target nodes
    and stop when we meet in the middle 
    */

    if (from == to) return 0;

    int n = graph.size();
    std::vector<int> dist_forward(n, INF), dist_backward(n, INF);
    dist_forward[from] = 0;
    dist_backward[to] = 0;

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq_forward, pq_backward;
    pq_forward.push({0, from}); // {distance, node}
    pq_backward.push({0, to});

    int best_distance = INF;

    while (!pq_forward.empty() || !pq_backward.empty())
    {
        // Forward Search Step
        if (!pq_forward.empty())
        {
            int dist = pq_forward.top().first;
            int current_node = pq_forward.top().second;
            pq_forward.pop();

            if (dist > dist_forward[current_node])
                continue;

            for (const auto &edge : graph[current_node])
            {
                int destination_node = edge.to;
                int weight = edge.weight;

                if (dist_forward[current_node] + weight < dist_forward[destination_node])
                {
                    dist_forward[destination_node] = dist_forward[current_node] + weight;
                    pq_forward.push({dist_forward[destination_node], destination_node});

                    // Check if we met in the middle
                    if (dist_backward[destination_node] != INF)
                    {
                        best_distance = std::min(best_distance, dist_forward[destination_node] + dist_backward[destination_node]);
                    }
                }
            }
        }

        // Backward Search Step
        if (!pq_backward.empty())
        {
            int dist = pq_backward.top().first;
            int current_node = pq_backward.top().second;
            pq_backward.pop();

            if (dist > dist_backward[current_node])
                continue;

            for (const auto &edge : graph[current_node])
            {
                int destination_node = edge.to;
                int weight = edge.weight;

                if (dist_backward[current_node] + weight < dist_backward[destination_node])
                {
                    dist_backward[destination_node] = dist_backward[current_node] + weight;
                    pq_backward.push({dist_backward[destination_node], destination_node});

                    // Check if we met in the middle
                    if (dist_forward[destination_node] != INF)
                    {
                        best_distance = std::min(best_distance, dist_forward[destination_node] + dist_backward[destination_node]);
                    }
                }
            }
        }

        // Early stopping if we have found a path and cannot improve further
        if (!pq_forward.empty() && !pq_backward.empty())
        {
            int min_d_forward = pq_forward.top().first;
            int min_d_backward = pq_backward.top().first;
            
            if (min_d_forward + min_d_backward >= best_distance) break;
        }
    }

    return best_distance == INF ? 1e9 : best_distance;
}

int main()
{
    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    // input reading
    int nodes, edges;
    std::cin >> nodes >> edges;
    Graph graph(nodes);

    for (int i = 0; i < nodes; ++i)
    {
        int neighbours;
        std::cin >> neighbours;
        int to, weight;
        while (neighbours--)
        {
            std::cin >> to >> weight;
            graph[i].push_back({to, weight});
            graph[to].push_back({i, weight});
        }
    }

    // queries
    int queries;
    std::cin >> queries;
    while (queries--)
    {
        int start, end;
        std::cin >> start >> end;
        std::cout << bidirectional_dijkstra(start, end, graph) << '\n';
    }

    return 0;
}
