#pragma once
#include <iostream>
#include <fstream>
#include <random>

void generate_matrix(int n, const char* filename) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 20.0);
    std::ofstream matrix_file(filename);

    if (!matrix_file.is_open()) {
        throw std::runtime_error("ERROR while opening");
    }

    for (int i = 0; i < n; ++i) {
        matrix_file << '|';
        for (int j = 0; j < n; ++j) {
            matrix_file << dis(gen) << " ";
        }
        matrix_file << '|' << std::endl;
    }
    matrix_file.close();
}