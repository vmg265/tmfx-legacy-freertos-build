#include "button_handler.h"
#include <lvgl.h>
#include "ui.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "notification_handler.h"  
#include "pwm_test.h"
#include "quicksettings.h"
#include "app_list.h"  
#include "esp_sleep.h"
#include "ESP32Time.h"
#include "ui_helpers.h"
#include "DEV_Config.h"
#include "watchface_manager.h"
#include <ChronosESP32.h>
#include "display_manager.h"

extern ChronosESP32 chronos;

const AppEntry* getAppByID(const char* id);
void routeAppButtonPress(ButtonType btn, bool isLong);

extern int currentAppIndex;

static int notifScrollIndex = 0;  
extern int currentSelectedIndex;
void openSelectedThread();
void dismissSelectedNotification();

bool demomode = false;

// enum ButtonType { BTN_UP, BTN_DOWN, BTN_SELECT, BTN_BACK, BTN_UNKNOWN };
enum ScreenID { WF, NOTIF, DEMO, QUICK, APPLIST, THREAD, ACTIONS, ACC_GYRO_DEMO, SETTINGS };

// State tracking
static lv_obj_t* currentScreen = nullptr;
static unsigned long pressStartTime = 0;
static unsigned long lastPressTime = 0;
static int pressCount = 0;
static bool waitingForRelease = false;
static ButtonType lastKey = BTN_UNKNOWN;

// Configurable thresholds
const unsigned long LONG_PRESS_MS = 800;
const unsigned long DOUBLE_PRESS_GAP = 300; ////////decrease for less delay for double press checking

///// debouncing
static bool singlePressPending = false;
static ButtonType pendingButton = BTN_UNKNOWN;
static unsigned long pendingTime = 0;

// GPIO Button Setup 
void setupButtons() {
    pinMode(BTN_UP_PIN, INPUT_PULLUP);
    pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
    pinMode(BTN_SELECT_PIN, INPUT_PULLUP);
    pinMode(BTN_BACK_PIN, INPUT_PULLUP);
}

// Helper 
bool isPressed(uint8_t pin) {
    return digitalRead(pin) == LOW;
}

//  Button Mapping
ButtonType getPressedButton() {
    if (isPressed(BTN_UP_PIN)) {
        if (BUTTON_DEBUG) Serial.printf("UP button (GPIO %d) pressed\n", BTN_UP_PIN);
        return BTN_UP;
    }
    if (isPressed(BTN_DOWN_PIN)) {
        if (BUTTON_DEBUG) Serial.printf("DOWN button (GPIO %d) pressed\n", BTN_DOWN_PIN);
        return BTN_DOWN;
    }
    if (isPressed(BTN_SELECT_PIN)) {
        if (BUTTON_DEBUG) Serial.printf("SELECT button (GPIO %d) pressed\n", BTN_SELECT_PIN);
        return BTN_SELECT;
    }
    if (isPressed(BTN_BACK_PIN)) {
        if (BUTTON_DEBUG) Serial.printf("BACK button (GPIO %d) pressed\n", BTN_BACK_PIN);
        return BTN_BACK;
    }
    return BTN_UNKNOWN;
}


// === Screen Navigation ===
ScreenID getCurrentScreenID(lv_obj_t* screen) {
    if (screen == objects.wfpage) return WF;
    if (screen == objects.notificationspage) return NOTIF;
    if (screen == objects.pwm_motor_demoapppage) return DEMO;
    if (screen == objects.quicksettingspage) return QUICK;
    if (screen == objects.applistpage) return APPLIST;
    if (screen == objects.threadviewpage) return THREAD;
    if (screen == objects.acc_gyro_demopage) return ACC_GYRO_DEMO;
    if (screen == objects.notif_actionspage) return ACTIONS;
    if (screen == objects.system_settings_page) return SETTINGS;
    return WF;
}

// === Press Handlers ===
void handleSinglePress(ButtonType btn) {
    extern void routeAppButtonPress(ButtonType btn, bool isLongPress);
    for (const auto& app : appList) {
    if (app.button_handler && app.screen == lv_scr_act()) {
        app.button_handler(btn, false);
        return;
       }
    }
    ScreenID sid = getCurrentScreenID(currentScreen);
    switch (sid) {
        case WF:
            if (btn == BTN_UP) {
                setSelectedNotifIndex(0);  // Always highlight latest on entering
                goToScreen(objects.notificationspage);
            }
            else if (btn == BTN_SELECT) {
                currentAppIndex = 2;
                updateAppListUI();
                goToScreen(objects.applistpage);
                }
            else if (btn == BTN_DOWN) {
                 chronos.musicControl(MUSIC_TOGGLE); // MUSIC_PLAY, MUSIC_PAUSE, MUSIC_PREVIOUS, MUSIC_NEXT, MUSIC_TOGGLE, VOLUME_UP, VOLUME_DOWN, VOLUME_MUTE
                 Serial.println("Sent music command");
            }
            else if (btn == BTN_BACK) {
                 sleepDisplay();
                 }
            resetScreenTimer();

            break;


        case NOTIF:
            if (btn == BTN_BACK) {
                currentSelectedIndex = 0;
                goToScreen(objects.wfpage);
            }
            else if (btn == BTN_UP) {
                if (storedNotifs.empty() || currentSelectedIndex == 0) {
                    currentSelectedIndex = 0;
                    goToScreen(objects.wfpage);
                } else {
                    setSelectedNotifIndex(currentSelectedIndex - 1);
                }
            }
            else if (btn == BTN_DOWN) {
                if (storedNotifs.empty() || currentSelectedIndex >= storedNotifs.size() - 1) {
                    currentSelectedIndex = 0;
                    goToScreen(objects.wfpage);
                } else {
                    setSelectedNotifIndex(currentSelectedIndex + 1);
                }
            }
            else if (btn == BTN_SELECT) {
                openSelectedThread();
            }
            break;

        case QUICK:
            if (btn == BTN_BACK || (btn == BTN_UP && qsfocusIndex == 0)) {
                qsfocusIndex = 0;  // Reset focus on exit
                updateQuickSettingsUI();
                goToScreen(objects.wfpage);
            } else if (btn == BTN_UP) {
                scrollQuickSettingsUp();
            } else if (btn == BTN_DOWN) {
                scrollQuickSettingsDown();
            } else if (btn == BTN_SELECT) {
                triggerQuickSettingsAction(false);
            }
            break;

        case APPLIST:
            handleAppListButtons(btn, false);
            break;


        case THREAD:  // assume `THREAD` is added to ScreenID enum for threadviewpage
            if (btn == BTN_SELECT) goToScreen(objects.notif_actionspage);
            else if (btn == BTN_BACK) goToScreen(objects.notificationspage);
            break;

        case ACTIONS:
            if (btn == BTN_SELECT) {
                dismissSelectedNotification();
                goToScreen(objects.notificationspage);
            } else if (btn == BTN_BACK) {
                goToScreen(objects.threadviewpage);
            }
            break;
                  
    Serial.printf("DBG: screen=%d, pressed=%d\n", sid, btn);
    }
}

void handleLongPress(ButtonType btn) {
    extern void routeAppButtonPress(ButtonType btn, bool isLongPress);
    routeAppButtonPress(btn, true);
    ScreenID sid = getCurrentScreenID(currentScreen);

    if (getCurrentScreenID(currentScreen) == WF && btn == BTN_DOWN) {
        qsfocusIndex = 0;  // Reset when entering QS
        updateQuickSettingsUI();
        goToScreen(objects.quicksettingspage);
    }

    if (sid == WF && btn == BTN_BACK) {
    
    return;
    }

    // Universal BACK long-press logic
    if ((sid == NOTIF || sid == THREAD || sid == ACTIONS) && btn == BTN_BACK) {
        setSelectedNotifIndex(0);
        goToScreen(objects.wfpage);
    }
}

void handleDoublePress(ButtonType btn) {
    ScreenID sid = getCurrentScreenID(currentScreen);
    if (sid == WF && btn == BTN_SELECT) {
        watchfaceManagerOnSelectDoublePress();
    }
    // Toggle demo mode on BACK double press (optional logic)
    if (sid == WF && btn == BTN_BACK) {
        demomode = !demomode;
    }

    if (sid == WF && btn == BTN_DOWN) {
        goToScreen(objects.acc_gyro_demopage);
    }

    if (sid == QUICK && btn == BTN_SELECT) {
      triggerQuickSettingsAction(true);
    }
}

// === Main Task ===
void buttonTask(void* pvParameters) {
    ButtonType physicalBtn = BTN_UNKNOWN;
    ButtonType pendingBtn = BTN_UNKNOWN;
    unsigned long pressTime = 0;
    unsigned long lastReleaseTime = 0;
    bool waitingForDouble = false;

    while (true) {
        currentScreen = lv_scr_act();
        ButtonType currentPress = getPressedButton();

        if (currentPress != BTN_UNKNOWN && physicalBtn == BTN_UNKNOWN) {
            
            // screen wake logic
            if (!isDisplayOn()) {
                wakeDisplay(); 
                
                // Block this press from triggering UI actions
                // We wait for release to ensure the "wake" press is consumed
                while(getPressedButton() != BTN_UNKNOWN) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                
                physicalBtn = BTN_UNKNOWN; 
                continue; 
            }

            // If screen was already on, process as normal
            physicalBtn = currentPress;
            pressTime = millis();
            resetScreenTimer(); // Keep screen alive because user is interacting
        }

        // 3. Button Just Released
        if (currentPress == BTN_UNKNOWN && physicalBtn != BTN_UNKNOWN) {
            unsigned long duration = millis() - pressTime;

            if (duration > LONG_PRESS_MS) {
                handleLongPress(physicalBtn);
                waitingForDouble = false;
                pendingBtn = BTN_UNKNOWN;
            } else {
                // Prepare for Double Press check
                if (waitingForDouble && (millis() - lastReleaseTime < DOUBLE_PRESS_GAP) && (physicalBtn == pendingBtn)) {
                    handleDoublePress(physicalBtn);
                    waitingForDouble = false;
                    pendingBtn = BTN_UNKNOWN;
                } else {
                    waitingForDouble = true;
                    pendingBtn = physicalBtn;
                    lastReleaseTime = millis();
                }
            }
            physicalBtn = BTN_UNKNOWN;
        }

        if (waitingForDouble && (millis() - lastReleaseTime >= DOUBLE_PRESS_GAP)) {
            handleSinglePress(pendingBtn);

            // Delegate to custom app button handler if the app has one
            for (const auto& app : appList) {
                if (app.button_handler && lv_obj_get_parent(app.screen) == nullptr && lv_scr_act() == app.screen) {
                    app.button_handler(pendingBtn, false);
                    break;
                }
            }

            waitingForDouble = false;
            pendingBtn = BTN_UNKNOWN;
        }

        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}