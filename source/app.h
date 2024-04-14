#pragma once
#include "pseudonyms.h"
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <fstream>

std::tuple<const char*, node_name_t, node_name_t> parse_args(int arg_count, char* arg_vars[]);
matrix_t load_matrix(const char* filename);
template<typename Node> graph_t<Node> create_graph(const matrix_t& matr) noexcept;
components_t compute_components(const matrix_t& mtr) noexcept;
bool is_matrix_positive(const matrix_t& m);

namespace Kosaraju_Sharir{
   template<typename func>
   void dfs(graph_t<bool> &graph, const node_name_t &key_from, func add);
   std::vector<node_name_t> topology_sort(graph_t<bool> &graph) noexcept;
   components_t components_by_dfs(graph_t<bool> &graph, const std::vector<node_name_t> &nodes);
}

namespace Dijikstra{

    struct NodeDijkstra {
        NodeDijkstra& operator=(const weight_t& m) { marker = m; return *this; }
        weight_t marker = INF;
        bool is_visited = false;
        node_name_t prev = MAX;
    };

    class NodesToBeVisited {
    public:
        NodesToBeVisited(graph_t<NodeDijkstra>& graph) noexcept;
        graph_t<NodeDijkstra>::iterator pop_min_weight() noexcept;
        bool empty() noexcept { return unattended.empty(); }
    private:
        std::vector<graph_t<NodeDijkstra>::iterator> unattended;
    };

    void dijkstra_step(graph_t<NodeDijkstra>& graph, NodesToBeVisited& nodes_to_be_visited) noexcept;
    route_t find_route(graph_t<NodeDijkstra>& graph, node_name_t key_from, node_name_t key_to) noexcept;
}
std::pair<weight_t, route_t> dijkstra(const matrix_t& mtr, node_name_t key_from, node_name_t key_to) noexcept;

#include "app.hpp"
