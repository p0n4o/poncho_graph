#pragma once

// PARSING:
node_name_t convert(const char* str) {
    using namespace std::string_literals;
    node_name_t value = 0;
    std::istringstream sout(str);
    if (!(sout >> value).eof())
        throw std::runtime_error("Invalid argument parameter: "s + str);
    return value;
}

std::tuple<const char*, node_name_t, node_name_t> parse_args(int arg_count, char* arg_vars[])
{
    using namespace std::string_literals;
    if (arg_count != 7)
        throw std::runtime_error("Invalid number of arguments!");

    std::unordered_map<std::string, const char*> args = {
        {"--file", nullptr},
        {"--from", nullptr},
        {"--to", nullptr}
    };

    for (char** t = arg_vars + 1; t < arg_vars + arg_count; t += 2) {
        auto it = args.find(*t);
        if (it == args.end())
            throw std::runtime_error("Invalid argument: "s + *t);
        auto& [key, value] = *it;
        if (value != nullptr)
            throw std::runtime_error("Duplicated argument: "s + *t);
        value = *(t + 1);
    }
#ifndef NDEBUG
    for (auto& [key, value] : args)
        std::cout << key << ':' << value << '\n';
#endif // NDEBUG
    return { args["--file"], convert(args["--from"]), convert(args["--to"]) };
}


// LOAD MATRIX:
void divider(std::istream& in) {
    char dev;
    if (!(in >> dev)) throw std::runtime_error("ERROR while read");
    if (dev != '|') throw std::runtime_error("Invalid divider");
}

void next_row(std::istream& in) {
    char dev;
    if (!(in >> dev)) {
        if (!in.eof()) throw std::runtime_error("ERROR while read the row");
        return;
    }
    if (dev != '|') throw std::runtime_error("Invalid begin of the row");
    in.unget(); // Если не конец файла, то считанная палка — начало следующей строки
}

void matrix_check(const std::vector<std::vector<weight_t>>& mtr_vec) {
    for (const auto& vec : mtr_vec) {
        if (vec.size() != mtr_vec.begin()->size())
            throw std::runtime_error("Incorrect matrix: Different rows sizes");
    }
    if (mtr_vec.size() != mtr_vec.begin()->size()) throw std::runtime_error("Incorrect matrix: Matrix is not square");
}

matrix_t load_matrix(const char* filename){
    std::ifstream fin(filename);
    if (!fin) throw std::runtime_error("ERROR reading the file");

    weight_t el = 0;

    std::vector<std::vector<weight_t>> rows_vec;
    while (!fin.eof()) {
        std::vector<weight_t> vec;
        divider(fin);
        while (fin >> el) vec.push_back(el); // кастомный тип, в который не могут неявно преобразовываться char'ы
        fin.clear();
        divider(fin);
        rows_vec.push_back(vec);
        next_row(fin);
    }

    matrix_check(rows_vec);
    matrix_t m(rows_vec.size(), rows_vec.size());

    size_t i = 0;
    for (const auto& vec : rows_vec)
        std::copy(vec.begin(), vec.end(), m.begin() + (i++) * vec.size());
    return m;
}

bool is_matrix_positive(const matrix_t& mtr) {
    auto it = std::find_if(mtr.begin(), mtr.end(), [](const weight_t& el) { return el < 0; });
    if (it == mtr.end()) return true;
    return false;
}


// CREATE GRAPH:
template<typename Node>
graph_t<Node> create_graph(const matrix_t& mtr) noexcept{
    graph_t<Node> graph;
    for (size_t i = 0; i < mtr.rows(); ++i) // количество строк — число узлов
        graph.insert_node(i, Node());
    for (size_t i = 0; i < mtr.rows(); ++i) {
        for (size_t j = 0; j < mtr.columns(); ++j) {
            if (abs(mtr(i, j)) > EPS) // если точно не ноль
                graph.insert_edge(std::pair<node_name_t, node_name_t>(i, j), mtr(i,j)); // из определения матрицы смежности:
                                                                                                           // строка — откуда
        }
    }
    return graph;
}


// KOSARAJU-SHARIR:
namespace Kosaraju_Sharir {
    template<typename func>
    void dfs(graph_t<bool> &graph, const node_name_t &key_from, func add) { // в цикле от выбранной вершины проходится dfs'ом, пока не упрется в конец
                                                                            // (не умеет менять вершину, если встал в тупик)

        for (auto &[key_to, weight]: graph.find(key_from)->second) { // .find возвращает ноду,
                                                                                                      // после чего достаем unordered_map с ребрами
            if (!graph[key_to]) {
                graph[key_to] = true;
                dfs(graph, key_to, add); // рекурсивно продолжаем закрашивать дальше
            }
        }
        add(key_from); // добавили в список dfs пройденную вершину
    }


    std::vector<node_name_t> topology_sort(graph_t<bool> &graph) noexcept { // ВОЗВРАЩАЕТ ПЕРЕВЕРНУТЫЙ DFC-МАССИВ !!!
        std::vector<node_name_t> dfc_vec; // В цикле вызывает dfs_step для всех непосещенных вершин (выбирает вершину по флагу)
        for (auto &[key_from, nodes]: graph) {
            if (!graph[key_from]) {
                graph[key_from] = true;
                dfs(graph, key_from, [&dfc_vec](const node_name_t &key_from) { dfc_vec.push_back(key_from); });
            }
        } // Записываем все вершины В ОДИН вектор в обратном порядке топологической сортировки
        return dfc_vec;
    }

    components_t components_by_dfs(graph_t<bool> &graph, const std::vector<node_name_t> &nodes) { // Запсукает dfs для вершин в том порядке,
        // в котором они записаны в переданном векторе
        components_t strong_components; // для хранения компонент связанности
        for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
            std::set<node_name_t> comp;
            if (!graph[*it]) {
                graph[*it] = true;
                dfs(graph, *it, [&comp](const node_name_t &key_from) { comp.insert(key_from); });
            }
            if (!comp.empty())
                strong_components.insert(comp); // На каждой итерации получает set — вершины в сильной компоненте связанности
                                                   // и вставляет его в set из сильных компонент связанности
        }
        return strong_components;
    }
}

components_t compute_components(const matrix_t &mtr) noexcept {
    graph_t<bool> graph_initial = create_graph<bool>(mtr);
    graph_t<bool> graph_inverted = create_graph<bool>(transpose(mtr));
    std::vector<node_name_t> sorted_nodes = Kosaraju_Sharir::topology_sort(graph_inverted);
    return Kosaraju_Sharir::components_by_dfs(graph_initial, sorted_nodes);
}


// DIJIKSTRA:
namespace Dijikstra{

    NodesToBeVisited::NodesToBeVisited(graph_t<NodeDijkstra>& graph) noexcept {
        for (auto it = graph.begin(); it != graph.end(); ++it)
            unattended.push_back(it);
    }

    graph_t<NodeDijkstra>::iterator NodesToBeVisited::pop_min_weight() noexcept {

        auto it_min = std::min_element(unattended.begin(), unattended.end(),
                                   [](const auto it_a, const auto it_b) {
            return it_a->second.value().marker < it_b->second.value().marker; }); // Компаратор, сравнивающий маркеры нод, перебирая итераторы

        auto it = *it_min;
        it->second.value().is_visited = true;
        unattended.erase(it_min);
        return it;
    }

    void dijkstra_step(graph_t<NodeDijkstra>& graph, NodesToBeVisited& nodes_to_be_visited) noexcept {
        auto [key_from, node] = *nodes_to_be_visited.pop_min_weight(); // Нашли итератор на непосещенную вершину
                                                                                                // с минимальным весом и разыменовали его
        if (node.empty()) return; // если обособлена
        for (auto& [key_to, weight] : node) { // достали unordered_map с ребрами
            if (!graph.loop(key_from) && (weight + graph[key_from].marker < graph[key_to].marker)) { // условие перерасчета метки
                graph[key_to].marker = weight + graph[key_from].marker;
                graph[key_to].prev = key_from; // запоминаем вершину, изменившую метку
            }
        }
    }

    route_t find_route(graph_t<NodeDijkstra>& graph, node_name_t key_from, node_name_t key_to) noexcept {
        route_t route;
        if (graph[key_to].prev == MAX) { // если путь не удалось найти
            route.push_back(key_from);
            route.push_back(key_to);
            return route;
        }

        route.push_back(key_to); // записали вершину, в которую хотим прийти
        while (graph[route.back()].prev != MAX)  // восстанавливаем маршрут, пока не дойдем до None (начала маршрута)
            route.push_back(graph[route.back()].prev);

        std::reverse(route.begin(), route.end());
        return route;
    }
}

std::pair<weight_t, route_t> dijkstra(const matrix_t& mtr, node_name_t key_from, node_name_t key_to) noexcept {
    graph_t<Dijikstra::NodeDijkstra> graph = create_graph<Dijikstra::NodeDijkstra>(mtr);
    graph[key_from].marker = 0.0; // инициализируем алгоритм

    Dijikstra::NodesToBeVisited nodes_to_be_visited{ graph };
    while (!nodes_to_be_visited.empty())
        dijkstra_step(graph, nodes_to_be_visited);
    route_t route = find_route(graph, key_from, key_to);
    return {graph[key_to].marker, route};
}


//SPFA:
namespace SPFA{

    void NodeQueue::push(graph_t<NodeSPFA>::iterator it) { // добавляем ноду в очередь обработки
        it->second.value().is_working = true;
        queue.push_back(it);
    }

    graph_t<NodeSPFA>::iterator NodeQueue::pop() { // убираем отработанную вершину из очереди обработки
        auto it_graph = queue.front();
        it_graph->second.value().is_working = false;
        queue.pop_front();
        return it_graph;
    }

    void spfa_step(graph_t<NodeSPFA>& graph, NodeQueue& nodes_in_work) {
        auto [key_from, node] = *nodes_in_work.pop(); // извлекли актуальную вершину из очереди
        if (++graph[key_from].updates == graph.size()) { // проверка на отрицательный цикл
            graph[key_from].marker = -INF;
        }
        for (auto& [key_to, weight] : node) { // достали unordered_map с ребрами
            if (weight + graph[key_from].marker < graph[key_to].marker) {
                graph[key_to].marker = weight + graph[key_from].marker;
                graph[key_to].prev = key_from;
                nodes_in_work.push(graph.find(key_to)); // добавили новую вершину в очередь
            }
        }
    }

    route_t make_route(graph_t<NodeSPFA>& graph, node_name_t key_from, node_name_t key_to) {
        route_t route;
        if (graph[key_to].marker == -INF || graph[key_to].marker == INF) { // если путь не удалось найти или путь включает отрицательный цикл
            route.push_back(key_from);
            route.push_back(key_to);
            return route;
        }
        route.push_back(key_to); // записали вершину, в которую хотим прийти
        while (graph[route.back()].prev != MAX) // восстанавливаем маршрут, пока не дойдем до None (начала маршрута)
            route.push_back(graph[route.back()].prev);

        std::reverse(route.begin(), route.end());
        return route;
    }
}

std::pair<weight_t, route_t> spfa(const matrix_t& mtr, node_name_t key_from, node_name_t key_to) noexcept {
    graph_t<SPFA::NodeSPFA> graph = create_graph<SPFA::NodeSPFA>(mtr);
    auto node_start_it = graph.find(key_from);
    node_start_it->second.value().marker = 0.0; // инициализируем алгоритм

    SPFA::NodeQueue nodes_in_work;
    nodes_in_work.push(node_start_it); // помещаем стартовую вершину в очередь
    while (!nodes_in_work.empty()) // пока очередь не освободиться
        spfa_step(graph, nodes_in_work);
    route_t route = make_route(graph, key_from, key_to);
    return {graph[key_to].marker, route};
}




