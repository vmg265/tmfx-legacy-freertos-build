#include "app_list.h"
#include "ui.h"
#include <Arduino.h>
#include "ui_helpers.h"


void openWeatherApp() {
    goToScreen(objects.system_weatherpage);  
}

void handleWeatherAppButtons(ButtonType btn, bool longPress) {
    if (btn == BTN_BACK) {
        goToScreen(objects.applistpage);
    }
}

__attribute__((constructor))
void _include_weather_app() {
    Serial.println("Registering Weather App");

    registerApp({
        "weather",                          // app_id
        "Weather",                          // name
        lv_color_hex(0x111111),             // bg_grad_start
        lv_color_hex(0x333333),             // bg_grad_end
        "No weather data",      // EMPTY → "Good day :)" appears
        openWeatherApp,                     // open_function (optional)
        handleWeatherAppButtons,            // button_handler (optional)
        nullptr,
        nullptr,
        objects.system_weatherpage                 // screen 
    });
}