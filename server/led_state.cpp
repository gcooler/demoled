#include "led_state.h"

led_state &get_led_state()
{
    static led_state state = { "on", "red", 0 };
    return state;
}