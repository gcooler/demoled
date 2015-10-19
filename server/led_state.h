#pragma once

#include <string>

struct led_state
{
    std::string state;
    std::string color;
    double      rate;
};

led_state &get_led_state();