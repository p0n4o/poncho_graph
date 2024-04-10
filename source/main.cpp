#include "app.h"

int main(int arg_count, char* arg_values[]) {
    try {
        auto [file_name, from, to] = parse(arg_count, arg_values);
    }
    catch (std::exception& err) {
        std::cout << err.what();
    }

    return 0;
}
