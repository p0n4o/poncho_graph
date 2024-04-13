#pragma once
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <fstream>


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
            throw std::runtime_error("Dublicated argument: "s + *t);
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

matrix_t load_matrix(const char* filename)
{
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


// CREATE GRAPH:

template<typename Node>
graph_t<Node> create_graph(const matrix_t& mtr) noexcept{
    graph_t<Node> graph;
    for (size_t i = 0; i < mtr.rows(); ++i) // количество строк — число узлов
        graph.insert_node(i, Node());
    for (size_t i = 0; i < mtr.rows(); ++i) {
        for (size_t j = 0; j < mtr.columns(); ++j) {
            if (mtr(i, j) > EPS) // если точно не ноль
                graph.insert_edge(std::pair<node_name_t, node_name_t>(i, j), mtr(i,j)); // из определения матрицы смежности:
                                                                                                           // строка — откуда
        }
    }
    return graph;
}

// KOSARAJU-SHARIR:

template<typename func>
void dfs_step(graph_t<bool>& graph, const node_name_t& key_from, func add) { // в цикле от выбранной вершины проходится dfs'ом, пока не упрется в конец
                                                                             // (не умеет менять вершину, если встал в тупик)

    for (auto& [key_to, weight] : (*graph.find(key_from)).second) { // .find возвращает ноду,
                                                                                                   // после чего достаем unordered_map с ребрами
        if (!graph[key_to]) {
            graph[key_to] = true;
            dfs_step(graph, key_to, add); // рекурсивно продолжаем закрашивать дальше
        }
    }
    add(key_from); // добавили в список dfs пройденную вершину
}


std::vector<node_name_t> topology_sort(graph_t<bool>& graph) noexcept { // ВОЗВРАЩАЕТ ПЕРЕВЕРНУТЫЙ DFC-МАССИВ !!!
    std::vector<node_name_t> dfc_vec; // В цикле вызывает dfs_step для всех непосещенных вершин (выбирает вершину по флагу)
    for (auto& [key_from, nodes] : graph) {
        if (!graph[key_from]) {
            graph[key_from] = true;
            dfs_step(graph, key_from, [&dfc_vec](const node_name_t& key_from) { dfc_vec.push_back(key_from); });
            // insert может привести к невалидности итераторов, поэтому используем более безопасный push_back

            // Записываем все вершины В ОДИН вектор в обратном порядке топологической сортировки
        }
    }
    return dfc_vec;
}

components_t components_by_dfs(graph_t<bool>& graph, const std::vector<node_name_t>& nodes) { // Запсукает dfs для вершин в том порядке,
                                                                                              // в котором они записаны в переданном векторе
    components_t strong_components; // для хранения компонент связанности
    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
        std::set<node_name_t> comp;
        if (!graph[*it]) {
            graph[*it] = true;
            dfs_step(graph, *it, [&comp](const node_name_t& key_from) { comp.insert(key_from); });
        }
        if (!comp.empty()) strong_components.insert(comp); // На каждой итерации получает set — вершины в сильной компоненте связанности
                                                              // и вставляет его в set из сильных компонент связанности
    }
    return strong_components;
}

components_t compute_components(const matrix_t& mtr) noexcept {
    graph_t<bool> graph_initial = create_graph<bool>(mtr);
    graph_t<bool> graph_inverted = create_graph<bool>(transpose(mtr));
    std::vector<node_name_t> sorted_nodes = topology_sort(graph_inverted);
    return components_by_dfs(graph_initial, sorted_nodes);
}





