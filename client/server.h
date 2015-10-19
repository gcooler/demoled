#pragma once

#include <string>

bool server_init(const char *host, int port);
bool send_request(const std::string &req, std::string &resp);