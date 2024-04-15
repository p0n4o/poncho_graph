#include "app.h"

int main(int arg_count, char* arg_values[]) {
    try {
        auto [file_name, from, to] = parse_args(arg_count, arg_values);

        // TODO: исправить проблему с относительным путем!!!

        matrix_t m0 = load_matrix("/Users/nazarurakov/CLionProjects/laba3/workdir/input.txt");
        std::cout << m0;

        graph::Graph graph = create_graph<int>(m0);

        std::cout << "\nStrong components\n";
        components_t comp = compute_components(m0);
        for (const auto& set : comp) {
            for (const auto& el : set)
                std::cout << el << ' ';
            std::cout << '\n';
        }

        std::cout << "\nShortest route from 2 to 1:\n";
        auto s = spfa(m0, 2, 1);
        std::cout << s.first << '\n';
        for (const auto& el : s.second) {
            std::cout << el << ' ';
        }


    }
    catch (std::exception& err) {
        std::cout << err.what();
    }

    return 0;
}
