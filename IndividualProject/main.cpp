#include <iostream>
#include <set>
#include <queue>
#include <random>
#include <chrono>
#include <sstream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

auto startTime = chrono::steady_clock::now();

class Graph {
private:
    const int C, R;
    vector<vector<int>> adjacencyList;
    vector<int> Degree;
    vector<vector<int>> closeCities;
    mt19937 rng;

    void preprocessDistances(int maxDistance) {
        for (int startCity = 0; startCity < C; ++startCity) {
            vector<bool> visited(C, false);
            queue<pair<int, int>> q;

            q.push({startCity, 0});
            visited[startCity] = true;

            while (!q.empty()) {
                auto [currentCity, distance] = q.front();
                q.pop();

                if (distance > maxDistance) continue;

                closeCities[startCity].push_back(currentCity);

                for (int neighbor : adjacencyList[currentCity]) {
                    if (!visited[neighbor]) {
                        q.push({neighbor, distance + 1});
                        visited[neighbor] = true;
                    }
                }
            }
        }
    }

    int calculateUncovered(const vector<int>& solution, const vector<bool>& covered) {
        int uncovered = 0;
        for (int i = 0; i < C; ++i) {
            if (!covered[i]) ++uncovered;
        }
        return uncovered;
    }

    vector<bool> markCovered(const vector<int>& solution) {
        vector<bool> covered(C, false);
        for (const auto& shop : solution) {
            covered[shop] = true;
            for (const auto& city : closeCities[shop]) {
                covered[city] = true;
            }
        }
        return covered;
    }

    vector<int> generateNeighbor(const vector<int>& currentSolution) {
        vector<int> neighbor = currentSolution;
        if (!neighbor.empty() && rng() % 2) {
            neighbor.erase(neighbor.begin() + rng() % neighbor.size());
        } else {
            int randomCity = rng() % C;
            if (find(neighbor.begin(), neighbor.end(), randomCity) == neighbor.end()) {
                neighbor.push_back(randomCity);
            }
        }
        return neighbor;
    }

public:
    Graph(int n, int m) : C(n), R(m),
                         adjacencyList(n),
                         Degree(n, 0),
                         closeCities(n) {
        rng.seed(chrono::steady_clock::now().time_since_epoch().count());
    }

    void addEdge(const int& c1, const int& c2) {
        adjacencyList[c1].push_back(c2);
        adjacencyList[c2].push_back(c1);
        Degree[c1]++;
        Degree[c2]++;
    }

    vector<int> findFrogShops(const int& d) {
        preprocessDistances(d);

        vector<int> currentSolution = buildGreedily(d);
        vector<bool> covered = markCovered(currentSolution);
        int currentEnergy = currentSolution.size();

        double temperature = 3000.0;
        double coolingRate = 0.9999;
        int bestEnergy = currentEnergy;
        vector<int> bestSolution = currentSolution;

        while (chrono::steady_clock::now() - startTime < chrono::seconds(26)) {
            vector<int> neighbor = generateNeighbor(currentSolution);
            vector<bool> neighborCovered = markCovered(neighbor);
            if(calculateUncovered(neighbor, neighborCovered) > 0) continue;

            int neighborEnergy = neighbor.size();

            if (neighborEnergy < currentEnergy || exp((currentEnergy - neighborEnergy) / temperature) > rng() / double(rng.max())) {
                currentSolution = neighbor;
                currentEnergy = neighborEnergy;

                if (currentEnergy < bestEnergy) {
                    bestEnergy = currentEnergy;
                    bestSolution = currentSolution;
                }
            }
            if(temperature < 0.01) break;
            temperature *= coolingRate;
        }

        return bestSolution;
    }

    vector<int> buildGreedily(const int& d) {
        vector<int> solution;
        vector<bool> covered(C, false);

        for (int i = 0; i < C; ++i) {
            if (!covered[i]) {
                solution.push_back(i);
                for (auto& city : closeCities[i]) {
                    covered[city] = true;
                }
            }
        }
        return solution;
    }
};

int main() {
    startTime = chrono::steady_clock::now();

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;
    Graph g(n, m);

    while (m--) {
        int city1, city2;
        cin >> city1 >> city2;
        g.addEdge(city1, city2);
    }

    int d;
    cin >> d;
    auto shops = g.findFrogShops(d);

    ostringstream oss;
    oss << shops.size() << '\n';
    for (auto& shop : shops) {
        oss << shop << ' ';
    }

    cout << oss.str();
    return 0;
}
