#include "sensor_test_app.h"
#include "ui.h"
#include "app_list.h"
#include <Arduino.h>
#include "ui_helpers.h"


void goToScreen(lv_obj_t* target);

void opensystem_sensortestApp() {
    goToScreen(objects.acc_gyro_demopage);
}

void handle_system_sensortest_App_Buttons(ButtonType btn, bool longPress) {
    if (btn == BTN_BACK) {
        goToScreen(objects.applistpage);
    }
}

__attribute__((constructor))
void _include_system_sensortest_app() {
    registerApp({
        "system_sensor_test",
        "Sensor Test",
        lv_color_hex(0x111111),
        lv_color_hex(0x333333),
        "Sensors Ready",    
        opensystem_sensortestApp,
        handle_system_sensortest_App_Buttons,
        nullptr,
        nullptr,
        objects.acc_gyro_demopage 
    });
}