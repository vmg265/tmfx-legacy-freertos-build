#include "pwm_test.h"
#include <cstdio>
#include "lvgl.h"
#include "app_list.h"
#include "ui.h"
#include <Arduino.h>
#include "ui_helpers.h"

void goToScreen(lv_obj_t* target);  

void openDemoApp() {
    goToScreen(objects.pwm_motor_demoapppage);  
    updateDemoLabels();               
    updateDemoFocus();               
}

void handle_pwm_motors_App_Buttons(ButtonType btn, bool longPress) {
            if (btn == BTN_BACK) {
                goToScreen(objects.applistpage);
                return;
            } else if (btn == BTN_UP) {
                focusIndex = (focusIndex + 5) % 6;
                updateDemoFocus();
            } else if (btn == BTN_DOWN) {
                focusIndex = (focusIndex + 1) % 6;
                updateDemoFocus();
            } else if (btn == BTN_SELECT) {
                switch (focusIndex) {
                    case 0:
                        dirCW = !dirCW;
                        break;
                    case 1:
                        stepsPerMove = (stepsPerMove == 10) ? 1 : stepsPerMove + 1;
                        break;
                    case 2:
                        pulseWidthMicros = (pulseWidthMicros == 1000) ? 500 : 1000;
                        break;
                    case 3:
                        switch (stepIntervalMs) {
                            case 100: stepIntervalMs = 500; break;
                            case 500: stepIntervalMs = 1000; break;
                            case 1000: stepIntervalMs = 2000; break;
                            case 2000: stepIntervalMs = 100; break;
                        }
                        break;
                    case 4:
                        autoMode = !autoMode;
                        autoStepping = autoMode;
                        break;
                    case 5:
                        if (!autoMode) {
                            digitalWrite(DIR_PIN, dirCW ? HIGH : LOW);
                            for (int i = 0; i < stepsPerMove; i++) {
                                digitalWrite(STEP_PIN, HIGH);
                                delayMicroseconds(pulseWidthMicros);
                                digitalWrite(STEP_PIN, LOW);
                                delayMicroseconds(pulseWidthMicros);
                            }
                        }
                        break;
                }
                updateDemoLabels();
            }
}

void _include_PwmDemoApp() {
    Serial.println("Registering PWM Demo App");
    registerApp({
        "pwm",
        "PWM Demo M1",
        lv_color_hex(0x222244),
        lv_color_hex(0x334477),

        "Motor > Stepper Test",     

        openDemoApp,
        handle_pwm_motors_App_Buttons,
        nullptr,
        nullptr,
        objects.pwm_motor_demoapppage
    });
}


// Stepper configuration
const int DIR_PIN = 36;
const int STEP_PIN = 13;

bool dirCW = true;
bool autoMode = false;
bool autoStepping = false;
int stepsPerMove = 10;
int pulseWidthMicros = 1000;
int stepIntervalMs = 1000;
int focusIndex = 0;




void updateDemoLabels() {
    lv_label_set_text(objects.dir_pwm_demo_label, dirCW ? "CW" : "CCW");
    static char buf[16];
    snprintf(buf, sizeof(buf), "%d", stepsPerMove);
    lv_label_set_text(objects.step_pwm_demo_label, buf);
    snprintf(buf, sizeof(buf), "%d", pulseWidthMicros);
    lv_label_set_text(objects.pulsewid_pwm_demo_label, buf);
    snprintf(buf, sizeof(buf), "%d", stepIntervalMs);
    lv_label_set_text(objects.interval_pwm_demo_label, buf);
    lv_label_set_text(objects.obj17, autoMode ? "Auto" : "Manual");
}

void updateDemoFocus() {
    lv_obj_t* containers[] = {
        objects.obj5,   // Direction
        objects.obj7,   // Steps
        objects.obj10,  // Pulse width
        objects.obj13,  // Interval
        objects.obj16,  // Mode
        objects.obj3    // Trigger (manual mode)
    };
    for (int i = 0; i < 6; i++) {
        lv_obj_set_style_bg_opa(containers[i], (i == focusIndex ? LV_OPA_100 : LV_OPA_0), LV_PART_MAIN);
    }
}