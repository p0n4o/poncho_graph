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



