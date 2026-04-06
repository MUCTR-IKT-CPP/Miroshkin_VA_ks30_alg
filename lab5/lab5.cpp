#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;

class Graph {
private:
    int vertices;
    vector<vector<pair<int, int>>> adjList;
    vector<vector<int>> adjMatrix;

public:
    Graph(int v) : vertices(v) {
        adjList.resize(v);
        adjMatrix.resize(v, vector<int>(v, 0));
    }

    void addEdge(int u, int v, int weight) {
        if (adjMatrix[u][v] == 0 && u != v) {
            adjList[u].push_back({v, weight});
            adjList[v].push_back({u, weight});
            adjMatrix[u][v] = weight;
            adjMatrix[v][u] = weight;
        }
    }

    const vector<vector<int>>& getAdjMatrix() const { return adjMatrix; }
    const vector<vector<pair<int, int>>>& getAdjList() const { return adjList; }

    vector<vector<int>> getIncMatrix() const {
        vector<pair<int, int>> edges;
        for (int i = 0; i < vertices; i++) {
            for (auto& [j, w] : adjList[i]) {
                if (i < j) edges.push_back({i, j});
            }
        }
        vector<vector<int>> inc(vertices, vector<int>(edges.size(), 0));
        for (size_t e = 0; e < edges.size(); e++) {
            inc[edges[e].first][e] = 1;
            inc[edges[e].second][e] = 1;
        }
        return inc;
    }

    vector<pair<int, int>> getEdgeList() const {
        vector<pair<int, int>> edges;
        for (int i = 0; i < vertices; i++) {
            for (auto& [j, w] : adjList[i]) {
                if (i < j) edges.push_back({i, j});
            }
        }
        return edges;
    }

    int getVertices() const { return vertices; }
    int getDegree(int v) const { return adjList[v].size(); }
};

class GraphGenerator {
public:
    static Graph generate(int vertex, int minDegree, int maxDegree, int minWeight, int maxWeight, mt19937& gen) {
        Graph graph(vertex);
        uniform_int_distribution<int> weightDist(minWeight, maxWeight);

        vector<vector<bool>> connected(vertex, vector<bool>(vertex, false));
        vector<int> degrees(vertex, 0);
        vector<int> candidates(vertex);
        for (int i = 0; i < vertex; i++) candidates[i] = i;

        for (int v = 0; v < vertex; v++) {
            shuffle(candidates.begin(), candidates.end(), gen);
            for (int u : candidates) {
                if (degrees[v] >= minDegree) break;
                if (u == v || connected[v][u]) continue;
                if (degrees[u] >= maxDegree) continue;

                int weight = weightDist(gen);
                graph.addEdge(v, u, weight);
                connected[v][u] = connected[u][v] = true;
                degrees[v]++;
                degrees[u]++;
            }
        }

        for (int v = 0; v < vertex; v++) {
            shuffle(candidates.begin(), candidates.end(), gen);
            for (int u : candidates) {
                if (degrees[v] >= maxDegree) break;
                if (u == v || connected[v][u]) continue;
                if (degrees[u] >= maxDegree) continue;

                int weight = weightDist(gen);
                graph.addEdge(v, u, weight);
                connected[v][u] = connected[u][v] = true;
                degrees[v]++;
                degrees[u]++;
            }
        }

        return graph;
    }
};

pair<vector<int>, vector<int>> dijkstra(const Graph& graph, int start) {
    int n = graph.getVertices();
    const int INF = 1e9;
    vector<int> dist(n, INF);
    vector<int> parent(n, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, v] = pq.top();
        pq.pop();
        if (d > dist[v]) continue;

        const auto& adj = graph.getAdjList()[v];
        for (const auto& edge : adj) {
            int to = edge.first;
            int w = edge.second;
            if (dist[v] + w < dist[to]) {
                dist[to] = dist[v] + w;
                parent[to] = v;
                pq.push({dist[to], to});
            }
        }
    }
    return {dist, parent};
}

vector<int> getPath(int start, int end, const vector<int>& parent) {
    vector<int> path;
    if (parent[end] == -1 && start != end) return path;

    int cur = end;
    while (cur != -1) {
        path.push_back(cur);
        if (cur == start) break;
        cur = parent[cur];
    }
    reverse(path.begin(), path.end());
    return path;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    random_device rd;
    mt19937 gen(rd());

    vector<int> numVertex = {10, 20, 50, 100};
    vector<int> minDegree = {3, 4, 10, 20};
    vector<int> maxDegree = {5, 7, 15, 30};
    const int minWeight = 1;
    const int maxWeight = 20;
    const int numTests = 5;

    ofstream fout("results.txt");

    cout << "=== Weighted Graphs and Dijkstra Algorithm ===" << endl << endl;
    fout << "=== Weighted Graphs and Dijkstra Algorithm ===" << endl << endl;

    for (size_t i = 0; i < numVertex.size(); i++) {
        int v = numVertex[i];
        int minDeg = minDegree[i];
        int maxDeg = maxDegree[i];

        cout << "========================================" << endl;
        cout << "Vertices: " << v << ", Min degree: " << minDeg << ", Max degree: " << maxDeg << endl;
        cout << "========================================" << endl << endl;

        fout << "========================================" << endl;
        fout << "Vertices: " << v << ", Min degree: " << minDeg << ", Max degree: " << maxDeg << endl;
        fout << "========================================" << endl << endl;

        double totalTime = 0;

        for (int test = 0; test < numTests; test++) {
            Graph g = GraphGenerator::generate(v, minDeg, maxDeg, minWeight, maxWeight, gen);

            cout << "--- Test " << (test + 1) << " ---" << endl;
            fout << "--- Test " << (test + 1) << " ---" << endl;

            cout << "Adjacency Matrix:" << endl;
            fout << "Adjacency Matrix:" << endl;
            auto matrix = g.getAdjMatrix();
            for (int r = 0; r < v && r < 10; r++) {
                for (int c = 0; c < v && c < 10; c++) {
                    if (matrix[r][c] == 0) {
                        cout << setw(4) << ".";
                        fout << setw(4) << ".";
                    } else {
                        cout << setw(4) << matrix[r][c];
                        fout << setw(4) << matrix[r][c];
                    }
                }
                if (v > 10) {
                    cout << " ...";
                    fout << " ...";
                }
                cout << endl;
                fout << endl;
            }
            if (v > 10) {
                cout << "..." << endl;
                fout << "..." << endl;
            }

            cout << "Vertex degrees: ";
            fout << "Vertex degrees: ";
            for (int j = 0; j < v && j < 12; j++) {
                cout << g.getDegree(j) << " ";
                fout << g.getDegree(j) << " ";
            }
            if (v > 12) {
                cout << "...";
                fout << "...";
            }
            cout << endl << endl;
            fout << endl << endl;

            auto startTime = chrono::high_resolution_clock::now();

            cout << "Shortest paths (first 3 sources):" << endl;
            fout << "Shortest paths (first 3 sources):" << endl;
            for (int src = 0; src < v && src < 3; src++) {
                auto [dist, parent] = dijkstra(g, src);
                cout << "From " << src << ":" << endl;
                fout << "From " << src << ":" << endl;
                for (int dst = 0; dst < v && dst < 5; dst++) {
                    if (src == dst) continue;
                    auto path = getPath(src, dst, parent);
                    cout << "  to " << dst << ": dist=" << dist[dst] << ", path=";
                    fout << "  to " << dst << ": dist=" << dist[dst] << ", path=";
                    for (size_t p = 0; p < path.size(); p++) {
                        cout << path[p];
                        fout << path[p];
                        if (p < path.size() - 1) {
                            cout << "->";
                            fout << "->";
                        }
                    }
                    cout << endl;
                    fout << endl;
                }
                if (v > 5) {
                    cout << "  ..." << endl;
                    fout << "  ..." << endl;
                }
            }
            if (v > 3) {
                cout << "..." << endl;
                fout << "..." << endl;
            }

            for (int src = 3; src < v; src++) {
                dijkstra(g, src);
            }

            auto endTime = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> diff = endTime - startTime;
            totalTime += diff.count();

            cout << "Time for all-pairs: " << diff.count() << " ms" << endl << endl;
            fout << "Time for all-pairs: " << diff.count() << " ms" << endl << endl;
        }

        double avgTime = totalTime / numTests;
        cout << "Average time for " << v << " vertices: " << avgTime << " ms" << endl << endl;
        fout << "Average time for " << v << " vertices: " << avgTime << " ms" << endl << endl;
    }

    fout.close();

    return 0;
}
