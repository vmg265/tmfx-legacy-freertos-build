#pragma once

#include <lvgl.h>
#include <vector>
#include <functional>
#include <string>

struct QuickSettingEntry {
    const char* label;
    const char* sublabel1;
    const char* sublabel2;
    const char* actionLabel;
    lv_color_t color;
    std::function<void(bool doubleTap)> action;
};

// Global quicksettings data
extern std::vector<QuickSettingEntry> qsEntries;
extern int qsfocusIndex;

void registerQuickSetting(QuickSettingEntry entry);
void updateQuickSettingsUI();
void scrollQuickSettingsUp();
void scrollQuickSettingsDown();
void triggerQuickSettingsAction(bool doubleTap);