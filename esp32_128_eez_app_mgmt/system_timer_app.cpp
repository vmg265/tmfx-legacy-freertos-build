#include "app_list.h"
#include "ui.h"
#include "lvgl.h"
#include <Arduino.h>
#include "system_timer_app.h"
#include "ui_helpers.h"

void openSystemTimerApp() {
    goToScreen(objects.system_timer_app_page); 

void handleSystemTimerAppButtons(ButtonType btn, bool longPress) {
    if (btn == BTN_BACK) {
        goToScreen(objects.applistpage); 
    }
    
}

__attribute__((constructor))
void _include_system_timer_app() {
    Serial.println("Registering Timer App");
    registerApp({
        "system_timer",                    // app_id
        "Timer",                           // name
        lv_color_hex(0x111111),           // bg_grad_start
        lv_color_hex(0x333333),           // bg_grad_end
        "No Timers Running",      // EMPTY → "Good day :)" appears
        openSystemTimerApp,               // open_function
        handleSystemTimerAppButtons,       // button_handler
        nullptr,
        nullptr,
        objects.system_timer_app_page
    });
}