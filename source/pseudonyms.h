#include <set>
#include <vector>
#include <limits>
#include "../matrix/matrix.h"
#include "../graph/graph.h"

using node_name_t = size_t;
using weight_t = double;
using matrix_t = linalg::Matrix<weight_t>;
template <typename T>
using graph_t = graph::Graph<node_name_t, T, weight_t>;
using components_t = std::set<std::set<node_name_t>>;
using route_t = std::vector<node_name_t>;
const double INF = std::numeric_limits<double>::infinity();
const double EPS = std::numeric_limits<double>::epsilon();
const size_t MAX = std::numeric_limits<size_t>::max();

