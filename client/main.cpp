#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

sockaddr_in addr;

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

bool handle_help(std::vector<std::string>& args)
{
    std::cout << "Available commmands:\n"
        "\thelp\t\tView this help\n"
        "\tstate\t\tGet led state from server\n"
        "\tstate {on|off}\tSet led state\n"
        ;
    return true;
}

bool common_handler(const std::string &type, std::vector<std::string>& args)
{
    std::string out;
    if (args.empty())
    {
        if (send_request("get-led-" + type, out))
        {
            std::cout << "Current led " + type + " is " << out << std::endl;
        }
    }
    else
    {
        if (send_request("set-led-" + type + " " + args[0], out))
        {
            std::cout << "Ok" << std::endl;
        }
    }
    return true;
}

bool handle_state(std::vector<std::string>& args)
{
    return common_handler("state", args);
}

bool handle_color(std::vector<std::string>& args)
{
    return common_handler("color", args);
}

bool handle_rate(std::vector<std::string>& args)
{
    return common_handler("rate", args);
}

bool handle_exit(std::vector<std::string>& args)
{
    return false;
}

typedef bool command_handler(std::vector<std::string>& args);
const std::map<std::string, command_handler *> handlers =
{
    { "help",  handle_help  },
    { "state", handle_state },
    { "color", handle_color },
    { "rate",  handle_rate  },
    { "exit",  handle_exit  },
};

bool process_command(const std::string& cmd, std::vector<std::string>& args)
{
    std::map<std::string, command_handler *>::const_iterator it = handlers.find(cmd);
    if (it == handlers.end())
    {
        std::cout << "Invalid command. Type 'help' to view commands list" << std::endl;
        return true;
    }
    return it->second(args);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return 1;
    }

    hostent *server = gethostbyname(argv[1]);
    if (!server)
    {
        std::cerr << "Failed to find host " << argv[1] << ", error code: " << h_errno << std::endl;
        return 1;
    }

    memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr = *(in_addr *)server->h_addr;

    for (;;)
    {
        std::cout << "Enter command: ";

        std::string line;
        std::getline(std::cin, line);

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        std::string buf;
        std::vector<std::string> args;
        while (ss >> buf) args.push_back(buf);

        std::string out;
        if (!process_command(cmd, args)) break;
    }
}

