#include "handler.h"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 9999

bool process_command(const std::string& cmd, std::vector<std::string>& args, std::string& out)
{
    command_handler *handler = get_handler(cmd);
    if (!handler) return false;
    return handler(args, out);
}

void process_request(int fd)
{
    std::string req;
    for (;;)
    {
        char buf[256];
        int res = recv(fd, buf, sizeof(buf), 0);

        if (req.empty()) fcntl(fd, F_SETFL, O_NONBLOCK);
        if (res > 0) req.append(buf, res);
        if (res < (int)sizeof(buf)) break;
    }
    fcntl(fd, F_SETFL, 0);

    std::stringstream ss(req);
    std::string cmd;
    ss >> cmd;

    std::string buf;
    std::vector<std::string> args;
    while (ss >> buf) args.push_back(buf);

    std::string out;
    if (process_command(cmd, args, out))
    {
        if (!out.empty()) out = " " + out;
        out = "OK" + out;
    }
    else
    {
        out = "FAILED";
    }
    out += "\n";

    if (send(fd, out.data(), out.size(), 0) != (int)out.size())
    {
        std::cerr << "Failed to send data, error code: " << errno << std::endl;
    }

    close(fd);
}

int main()
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cerr << "Failed to create socket, error code: " << errno << std::endl;
        return 1;
    }

    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        std::cerr << "Failed to set socket options, error code: " << errno << std::endl;
        return 1;
    }

    sockaddr_in addr;
    memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Failed to bind, error code: " << errno << std::endl;
        return 1;
    }

    if (listen(fd, SOMAXCONN) < 0)
    {
        std::cerr << "Failed to start port listening, error code: " << errno << std::endl;
        return 1;
    }

    std::cout << "Server started on port " << SERVER_PORT << std::endl;

    for (;;)
    {
        sockaddr_in addr;
        socklen_t size = sizeof(addr);
        int new_fd = accept(fd, (sockaddr *)&addr, &size);
        if (new_fd < 0)
        {
            std::cerr << "Failed to accept connection, error code: " << errno << std::endl;
            continue;
        }

        process_request(new_fd);
    }

    return 0;
}