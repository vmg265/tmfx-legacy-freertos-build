#include "app_list.h"
#include "ui.h"
#include <Arduino.h>
#include "ui_helpers.h"

void goToScreen(lv_obj_t* target);  // Forward declaration

// Touching these functions to force Arduino IDE to link app files <<<<ADD App includes here
extern void _include_weather_app();
extern void _include_PwmDemoApp();
extern void _include_system_settings_app();
extern void _include_system_sensortest_app();
extern void _include_system_timer_app();

static void ensureAllAppsLinked() {
    static bool alreadyLinked = false;
    if (alreadyLinked) return;
    alreadyLinked = true;
    //<<<< ADD App includes here as well
    _include_system_settings_app();
    _include_PwmDemoApp();
    _include_system_sensortest_app();
    _include_weather_app();
    _include_system_timer_app();

    
}

std::vector<AppEntry> appList;
int currentAppIndex = 0;
lv_obj_t* applist_labels[APP_LIST_VISIBLE_COUNT];

void registerApp(const AppEntry& entry) {
    appList.push_back(entry);
}

void handleAppListButtons(ButtonType btn, bool longPress) {
    if (btn == BTN_UP && currentAppIndex > 0) {
        currentAppIndex--;
        updateAppListUI();
    } else if (btn == BTN_DOWN && currentAppIndex < appList.size() - 1) {
        currentAppIndex++;
        updateAppListUI();
    } else if (btn == BTN_BACK) {
        currentAppIndex = 2;
        updateAppListUI();
        goToScreen(objects.wfpage);
        invalidateAllChildren(objects.wfpage);  // Explicit full screen redraw
        lv_timer_handler();
    } else if (btn == BTN_SELECT) {
        const AppEntry& app = appList[currentAppIndex];
        if (app.open_function) {
            app.open_function();
        }
    }
}

void updateAppListUI() {
    lv_timer_handler();
    ensureAllAppsLinked();  
    
    if (appList.empty()) return;
    const AppEntry& app = appList[currentAppIndex];

    lv_obj_set_style_bg_color(objects.app_list_bg_container, appList[currentAppIndex].bg_grad_start, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(objects.app_list_bg_container, appList[currentAppIndex].bg_grad_end, LV_STATE_DEFAULT);


    int centerSlot = APP_LIST_VISIBLE_COUNT / 2;

    for (int i = 0; i < APP_LIST_VISIBLE_COUNT; i++) {
        int offset = i - centerSlot;
        int appIndex = currentAppIndex + offset;
        if (appIndex >= 0 && appIndex < appList.size()) {
            lv_label_set_text(applist_labels[i], appList[appIndex].name);
            lv_obj_clear_flag(applist_labels[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_label_set_text(applist_labels[i], "");
            lv_obj_add_flag(applist_labels[i], LV_OBJ_FLAG_HIDDEN);
        }
        lv_obj_invalidate(applist_labels[i]);
    }

    // UPDATE NEW QUICK INFO LABEL
    // ================================
    if (app.app_info_text && strlen(app.app_info_text) > 0) {
        lv_label_set_text(objects.app_in_focus_app_list_quicklabel,
                          app.app_info_text);
    } else {
        lv_label_set_text(objects.app_in_focus_app_list_quicklabel,
                          "Good day :)");
    }

    Serial.printf("App index: %d of %d\n", currentAppIndex, (int)appList.size());
    

}

void routeAppButtonPress(ButtonType btn, bool isLong) {
    lv_obj_t* current = lv_scr_act();
    for (const auto& app : appList) {
        if (app.button_handler && app.screen == current) {
            app.button_handler(btn, isLong);
            return;
        }
    }
}


const AppEntry* getAppByID(const char* id) {
    for (const auto& app : appList) {
        if (strcmp(app.app_id, id) == 0) return &app;
    }
    return nullptr;
}

