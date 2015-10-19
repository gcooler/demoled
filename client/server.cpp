#include "server.h"

#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

sockaddr_in addr;

bool server_init(const char *host, int port)
{
    hostent *server = gethostbyname(host);
    if (!server)
    {
        std::cerr << "Failed to find host " << host << ", error code: " << h_errno << std::endl;
        return false;
    }

    memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *(in_addr *)server->h_addr;

    return true;
}

bool send_request(const std::string &req, std::string &resp)
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cerr << "Failed to create socket, error code: " << errno << std::endl;
        return false;
    }

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Failed to connect to server, error code: " << errno << std::endl;
        return false;
    }

    send(fd, req.data(), req.size(), 0);

    resp.clear();
    for (;;)
    {
        char buf[256];
        int res = recv(fd, buf, sizeof(buf), 0);
        if (resp.empty()) fcntl(fd, F_SETFL, O_NONBLOCK);
        if (res > 0) resp.append(buf, res);
        if (res < (int)sizeof(buf)) break;
    }
    fcntl(fd, F_SETFL, 0);
    close(fd);

    if (!resp.empty() && resp.back() == '\n') resp.resize(resp.size() - 1);
    if (resp.size() > 1 && resp[0] == 'O' && resp[1] == 'K' && (resp.size() == 2 || resp[2] == ' '))
    {
        resp.erase(0, resp.size() == 2 ? 2 : 3);
        return true;
    }

    std::cout << "Failed" << std::endl;

    return false;
}
