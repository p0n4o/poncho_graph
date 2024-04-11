#include "app.h"

int main(int arg_count, char* arg_values[]) {
    try {
        auto [file_name, from, to] = parse_args(arg_count, arg_values);

        // TODO: исправить проблему с относительным путем!!!

        matrix_t m0 = load_matrix(".../workdir/input.txt");
        std::cout << m0;
    }
    catch (std::exception& err) {
        std::cout << err.what();
    }

    return 0;
}
