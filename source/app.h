#pragma once
#include "pseudonyms.h"

std::tuple<const char*, node_name_t, node_name_t> parse_args(int arg_count, char* arg_vars[]);

#include "app.hpp"
