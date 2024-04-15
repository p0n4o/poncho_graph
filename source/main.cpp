#include "executor.hpp"
#include "/Users/nazarurakov/CLionProjects/laba3/workdir/generate_matrix.hpp"

int main(int arg_count, char* arg_values[]) {
    try {
        auto [file_name, from, to] = parse_args(arg_count, arg_values);

        // TODO: исправить проблему с относительным путем!!!

        generate_matrix(1000, "/Users/nazarurakov/CLionProjects/laba3/workdir/input.txt");
        matrix_t m0 = load_matrix("/Users/nazarurakov/CLionProjects/laba3/workdir/input.txt");
        components_and_path(m0,2,1);

    }
    catch (std::exception& err) {
        std::cout << RED << err.what() << RESET;
    }

    return 0;
}
