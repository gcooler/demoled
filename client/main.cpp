#include "server.h"

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

bool handle_help(std::vector<std::string> &args)
{
    std::cout << "Available commmands:\n"
        "\thelp\t\t\tShow this help\n"
        "\tstate\t\t\tGet led state from the server\n"
        "\tstate {on|off}\t\tSet led state\n"
        "\tcolor\t\t\tGet led color from the server\n"
        "\tcolor {red|green|blue}\tSet led color\n"
        "\trate\t\t\tGet led rate from the server\n"
        "\trate 0..5\t\tSet led rate\n";
    return true;
}

bool common_handler(const std::string &type, std::vector<std::string> &args)
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

bool handle_state(std::vector<std::string> &args)
{
    return common_handler("state", args);
}

bool handle_color(std::vector<std::string> &args)
{
    return common_handler("color", args);
}

bool handle_rate(std::vector<std::string> &args)
{
    return common_handler("rate", args);
}

bool handle_exit(std::vector<std::string> &args)
{
    return false;
}

typedef bool command_handler(std::vector<std::string> &args);
const std::map<std::string, command_handler *> handlers =
{
    { "help",  handle_help  },
    { "state", handle_state },
    { "color", handle_color },
    { "rate",  handle_rate  },
    { "exit",  handle_exit  },
};

bool process_command(const std::string &cmd, std::vector<std::string> &args)
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

    if (!server_init(argv[1], atoi(argv[2])))
    {
        return 1;
    }

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

