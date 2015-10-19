#pragma once

#include <string>
#include <vector>

typedef bool command_handler(std::vector<std::string> &args, std::string &out);

command_handler *get_handler(const std::string &cmd);
