#pragma once
#include <thread>
#include <chrono>
#include "app.h"

#define SEQUENTIAL_EXECUTION // ЗАКОММЕНТИРУЙТЕ ЭТУ СТРОКУ, ЕСЛИ ХОТИТЕ ПЕРЕВЕСТИ ВЫПОЛНЕНИЯ ПРОГРАММЫ В МНОГОПОТОЧНЫЙ РЕЖИМ

class StopWatch { //класс для расчета времени выполнения
public:
     using clock = std::chrono::high_resolution_clock;
     using time_point = std::chrono::time_point<clock>;
     using seconds = std::chrono::duration<double>;
public:
     ~StopWatch() {
           seconds duration = clock::now() - m_start;
           std::cout << YELLOW << "Execution Time: " << RESET << duration.count() << " seconds\n";
     }
private:
     time_point m_start = clock::now();
};

void print(const matrix_t& mtr, node_name_t key_from, node_name_t key_to, components_t comp, std::pair<weight_t, route_t> shortest_path){
        std::cout << CYAN << "Strong components:\n" << RESET;
    size_t i = 0;
    for (const auto& set : comp) {
        std::cout << i++ << ") ";
        for (const auto& el : set)
            std::cout << el << ' ';
        std::cout << '\n';
    }
    std::cout << CYAN << "\nShortest route from " << key_from << " to " << key_to << RESET << '\n';
    std::cout << MAGENTA << "Weight: " << RESET << shortest_path.first << '\n';
    std::cout << MAGENTA << "Route: " << RESET;
    for (auto el : shortest_path.second) {
        std::cout << el << ' ';
    }
    std::cout << '\n';
}

#ifdef SEQUENTIAL_EXECUTION // последовательный режим выполнения:
void components_and_path(const matrix_t& mtr, node_name_t key_from, node_name_t key_to) noexcept {
    using namespace std::chrono_literals;
    StopWatch s;

    components_t strong_components = compute_components(mtr); // расчитываем сильные компоненты связанности
    std::pair<weight_t, route_t> shortest_path;
    if (is_matrix_positive(mtr)) // расчитываем кратчайший путь в соответствии с входной матрицей
        shortest_path = dijkstra(mtr, key_from, key_to);
    else
        shortest_path = spfa(mtr, key_from, key_to);

    print(mtr, key_from, key_to, strong_components, shortest_path);
}
#else // параллельный режим выполнения:
void components_and_path(const matrix_t& mtr, node_name_t key_from, node_name_t key_to) noexcept {
    using namespace std::chrono_literals;
    StopWatch s;

    components_t strong_components;
    std::pair<weight_t, route_t> shortest_path;
    std::thread th_comp([&strong_components, &mtr]() { strong_components = compute_components_parallel(mtr); });
    std::thread th_path([&shortest_path, &mtr, &key_from, &key_to]() {
        if (is_matrix_positive(mtr))
            shortest_path = dijkstra(mtr, key_from, key_to);
        else
            shortest_path = spfa(mtr, key_from, key_to);
    });
    th_comp.join();
    th_path.join();

    print(mtr, key_from, key_to, strong_components, shortest_path);
}
#endif