#pragma once
#include <unordered_map>
#include <sstream>
#include <algorithm>

node_name_t convert(const char* str) {
    using namespace std::string_literals;
    node_name_t value = 0;
    std::istringstream sout(str);
    if (!(sout >> value).eof())
        throw std::runtime_error("Invalid argument parameter: "s + str);
    return value;
}

std::tuple<const char*, node_name_t, node_name_t> parse(int arg_count, char* arg_vars[])
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

