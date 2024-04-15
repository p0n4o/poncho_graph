#pragma once
#include "pseudonyms.h"
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <fstream>

// ANSI escape codes for colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

node_name_t convert(const char* str);
std::tuple<const char*, node_name_t, node_name_t> parse_args(int arg_count, char* arg_vars[]);

void divider(std::istream& in);
void next_row(std::istream& in);
void matrix_check(const std::vector<std::vector<weight_t>>& mtr_vec);
matrix_t load_matrix(const char* filename);
bool is_matrix_positive(const matrix_t& mtr);

template<typename Node> graph_t<Node> create_graph(const matrix_t& matr) noexcept;

namespace Kosaraju_Sharir{
   template<typename func>
   void dfs(graph_t<bool> &graph, const node_name_t &key_from, func add);
   std::vector<node_name_t> topology_sort(graph_t<bool> &graph) noexcept;
   components_t components_by_dfs(graph_t<bool> &graph, const std::vector<node_name_t> &nodes);
}
components_t compute_components(const matrix_t& mtr) noexcept;
components_t compute_components_parallel(const matrix_t& mtr) noexcept;

namespace Dijikstra{

    struct NodeDijkstra {
        weight_t marker = INF;
        bool is_visited = false;
        node_name_t prev = MAX;
    };

    class NodesToBeVisited {
    public:
        NodesToBeVisited(graph_t<NodeDijkstra>& graph) noexcept;
        graph_t<NodeDijkstra>::iterator pop_min_weight() noexcept;
        bool empty() const noexcept { return unattended.empty(); }
    private:
        std::vector<graph_t<NodeDijkstra>::iterator> unattended;
    };

    void dijkstra_step(graph_t<NodeDijkstra>& graph, NodesToBeVisited& nodes_to_be_visited) noexcept;
    route_t find_route(graph_t<NodeDijkstra>& graph, node_name_t key_from, node_name_t key_to) noexcept;
}
std::pair<weight_t, route_t> dijkstra(const matrix_t& mtr, node_name_t key_from, node_name_t key_to) noexcept;

namespace SPFA{
    struct NodeSPFA {
        weight_t marker = INF;
        size_t updates = 0;
        bool is_working = false;
        node_name_t prev = MAX;
    };

    class NodeQueue {
    public:
        void push(graph_t<NodeSPFA>::iterator it) noexcept;
        graph_t<NodeSPFA>::iterator pop() noexcept;
        bool empty() const noexcept { return queue.empty(); }
    private:
        std::deque<graph_t<NodeSPFA>::iterator> queue;
    };

    void spfa_step(graph_t<NodeSPFA>& graph, NodeQueue& nodes_in_work) noexcept;
    route_t make_route(graph_t<NodeSPFA>& graph, node_name_t key_from, node_name_t key_to) noexcept;
}
std::pair<weight_t, route_t> spfa(const matrix_t& mtr, node_name_t key_from, node_name_t key_to) noexcept;

#include "app.hpp"
