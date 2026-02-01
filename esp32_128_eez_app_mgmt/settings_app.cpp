#include "app_list.h"
#include "ui.h"
#include <Arduino.h>
#include "ui_helpers.h"
#include <lvgl.h>

void openSystemSettingsApp() {
    goToScreen(objects.system_settings_page);
}

// Optional: Button handling
void handleSystemSettingsAppButtons(ButtonType btn, bool longPress) {
    if (btn == BTN_BACK) {
        goToScreen(objects.applistpage);
    }
}

__attribute__((constructor))
void _include_system_settings_app() {
    Serial.println("Registering Settings App");

    registerApp({
        "system_settings",                 // app_id
        "Settings",                        // name
        lv_color_hex(0x111111),            // bg_grad_start
        lv_color_hex(0x333333),            // bg_grad_end
        "DND Enabled",      // EMPTY → "Good day :)" appears
        openSystemSettingsApp,            // open_function
        handleSystemSettingsAppButtons,   // button_handler
        nullptr,
        nullptr,
        objects.system_settings_page      // screen
    });
}