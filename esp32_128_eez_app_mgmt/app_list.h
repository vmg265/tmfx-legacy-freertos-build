#pragma once
#include <lvgl.h>
#include <vector>
#include "button_handler.h"

void handleAppListButtons(ButtonType btn, bool longPress);

#define APP_LIST_VISIBLE_COUNT 5

// === Add these forward declarations ===
enum ButtonType; // Forward-declared to avoid header dependency
struct AppEntry {
    const char* app_id;
    const char* name;
    lv_color_t bg_grad_start;
    lv_color_t bg_grad_end;
    const char* app_info_text;
    void (*open_function)();                  // When app is opened
    void (*button_handler)(ButtonType, bool); // Button interactions
    void (*tick_function)();                  // Optional: called periodically
    void (*exit_function)();                  // Optional: when switching away
    lv_obj_t* screen;  
};

// Global app list
extern std::vector<AppEntry> appList;
extern int currentAppIndex;
extern lv_obj_t* applist_labels[APP_LIST_VISIBLE_COUNT];

// Registration
void registerApp(const AppEntry& entry);
void updateAppListUI();
const AppEntry* getAppByID(const char* id);  
void routeAppButtonPress(ButtonType btn, bool isLong);