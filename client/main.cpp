#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 9999

int main()
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cerr << "Failed to create socket, error code: " << errno << std::endl;
        return 1;
    }

    sockaddr_in addr;
    memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Failed to connect to server, error code: " << errno << std::endl;
        return 1;
    }

    char buf[] = "get-led-rate";
    send(fd, buf, sizeof(buf) - 1, 0);

    std::string resp;
    for (;;)
    {
        char buf[256];
        int res = recv(fd, buf, sizeof(buf), 0);
        if (resp.empty()) fcntl(fd, F_SETFL, O_NONBLOCK);
        if (res > 0) resp.append(buf, res);
        if (res < (int)sizeof(buf)) break;
    }
    fcntl(fd, F_SETFL, 0);

    std::cout << resp << "\n";

    close(fd);

    return 0;
}