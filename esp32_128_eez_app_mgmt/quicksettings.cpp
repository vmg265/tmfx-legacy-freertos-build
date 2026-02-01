#include "quicksettings.h"
#include "ui.h"
#include "Arduino.h"

std::vector<QuickSettingEntry> qsEntries;
int qsfocusIndex = 0;

void registerQuickSetting(QuickSettingEntry entry) {
    qsEntries.push_back(entry);
}

void updateQuickSettingsUI() {
    if (qsEntries.empty()) return;
    const QuickSettingEntry& focused = qsEntries[qsfocusIndex % qsEntries.size()];

    lv_label_set_text(objects.qs_focused_text1, focused.label);
    lv_label_set_text(objects.qs_focused_text2, focused.sublabel1);
    lv_label_set_text(objects.qs_focused_text3, focused.sublabel2);
    lv_label_set_text(objects.qs_focused_actiontext, focused.actionLabel);

    // Apply color
    lv_obj_set_style_bg_grad_color(objects.focused_qs_container, qsEntries[qsfocusIndex].color, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(objects.focused_qs_container, lv_color_mix(qsEntries[qsfocusIndex].color, lv_color_black(), 128), LV_PART_MAIN | LV_STATE_DEFAULT);                          

    // Setup normal entries below focus
    for (int i = 0; i < 3; ++i) {
        int entryIndex = (qsfocusIndex + i + 1) % qsEntries.size();
        const QuickSettingEntry& entry = qsEntries[entryIndex];

        switch (i) {
            case 0:
                lv_label_set_text(objects.qs_normal_text_1, entry.label);
                lv_label_set_text(objects.qs_normal_action_1, entry.actionLabel);
                break;
            case 1:
                lv_label_set_text(objects.qs_normal_text_2, entry.label);
                lv_label_set_text(objects.qs_normal_action_2, entry.actionLabel);
                break;
            case 2:
                lv_label_set_text(objects.qs_normal_text_3, entry.label);
                lv_label_set_text(objects.qs_normal_action_3, entry.actionLabel);
                break;
        }
    }
    Serial.println(qsfocusIndex);
}

void scrollQuickSettingsUp() {
    if (qsEntries.empty()) return;
    qsfocusIndex = (qsfocusIndex - 1 + qsEntries.size()) % qsEntries.size();
    updateQuickSettingsUI();
}

void scrollQuickSettingsDown() {
    if (qsEntries.empty()) return;
    qsfocusIndex = (qsfocusIndex + 1) % qsEntries.size();
    updateQuickSettingsUI();
}

void triggerQuickSettingsAction(bool doublePress) {
    if (qsEntries.empty()) return;
    qsEntries[qsfocusIndex].action(doublePress);
}