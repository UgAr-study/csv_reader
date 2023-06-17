#pragma once

#include <unordered_map>
#include <vector>
#include <set>

template <typename Vertex>
class DirectedGraph {
    using visited_t = std::unordered_map<Vertex, bool>;
    using recursive_stack_t = std::unordered_map<Vertex, bool>;

    std::unordered_map<Vertex, std::vector<Vertex>> adj; // vertex and its adjacent nodes
    std::unordered_map<Vertex, bool> vertices; // all the vertices in graph
public:
    DirectedGraph() {}
    // add directed edge from v to w
    void addEdge(const Vertex& v, const Vertex& w);
    bool isCyclic() const;

private:
    // DFS function to find if a cycle exists
    bool isCyclicUtil(const Vertex& v, visited_t& visited, recursive_stack_t& rs) const;
};

template <typename Vertex>
void DirectedGraph<Vertex>::addEdge(const Vertex& v, const Vertex& w)
{
    if (adj.find(v) == agj.end()){
        agj[v] = std::vector<Vertex>{};
    }
    if (vertices.find(v) == vertices.end()) {
        vertices[v] = false;
    }
    if (vertices.find(w) == vertices.end()) {
        vertices[w] = false;
    }
    adj[v].push_back(w);
}

template <typename Vertex>
bool DirectedGraph<Vertex>::isCyclic() const
{
    // Mark all the vertices as not visited
    // and not part of recursion stack
    visited_t visited = vertices;
    recursive_stack_t rs = vertices;
    // Call the recursive helper function
    // to detect cycle in different DFS trees
    for (auto && vb: vertices) {
        auto i = vb.first;
        if (!visited[i] && isCyclicUtil(i, visited, rs))
            return true;
    }
 
    return false;
}

template <typename Vertex>
bool DirectedGraph<Vertex>::isCyclicUtil(const Vertex& v, visited_t &visited, recursive_stack_t &rs) const
{
    if (visited[v] == false) {
        // Mark the current node as visited
        // and part of recursion stack
        visited[v] = true;
        rs[v] = true;
 
        // Recursive for all the vertices adjacent to this vertex
        for (auto && i: adj[v]) {
            if ((!visited[i] && isCyclicUtil(i, visited, rs)) || rs[i])
                return true;
        }
    }
 
    // Remove the vertex from recursion stack
    rs[v] = false;
    return false;
}
