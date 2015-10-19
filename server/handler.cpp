#include "handler.h"
#include "led_state.h"

#include <map>
#include <sstream>
#include <stdlib.h>

bool handle_set_state(std::vector<std::string> &args, std::string &out)
{
    if (args.size() != 1) return false;

    std::string &state = args[0];
    if (state != "on" && state != "off") return false;

    get_led_state().state = state;
    return true;
}

bool handle_get_state(std::vector<std::string> &args, std::string &out)
{
    if (!args.empty()) return false;
    out = get_led_state().state;
    return true;
}

bool handle_set_color(std::vector<std::string> &args, std::string &out)
{
    if (args.size() != 1) return false;

    std::string &color = args[0];
    if (color != "red" && color != "green" && color != "blue") return false;

    get_led_state().color = color;
    return true;
}

bool handle_get_color(std::vector<std::string> &args, std::string &out)
{
    if (!args.empty()) return false;
    out = get_led_state().color;
    return true;
}

bool handle_set_rate(std::vector<std::string> &args, std::string &out)
{
    if (args.size() != 1) return false;

    double rate = atof(args[0].c_str());
    if (rate < 0 || rate > 5) return false;

    get_led_state().rate = rate;
    return true;
}

bool handle_get_rate(std::vector<std::string> &args, std::string &out)
{
    if (!args.empty()) return false;

    std::stringstream ss;
    ss << get_led_state().rate;

    out = ss.str();
    return true;
}

command_handler *get_handler(const std::string &cmd)
{
    static std::map<std::string, command_handler *> handlers =
    {
        { "set-led-state", handle_set_state },
        { "get-led-state", handle_get_state },
        { "set-led-color", handle_set_color },
        { "get-led-color", handle_get_color },
        { "set-led-rate",  handle_set_rate  },
        { "get-led-rate",  handle_get_rate  },
    };

    auto it = handlers.find(cmd);
    return it != handlers.end() ? it->second : 0;
}
