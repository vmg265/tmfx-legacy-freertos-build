#include "notification_handler.h"
#include "ui.h"
#include "ChronosESP32.h"
#include "ui_helpers.h"

#include <vector>
#include <map>
#include <string>

extern ChronosESP32 chronos;
extern objects_t objects;

int currentSelectedIndex = 0;

#define MAX_GROUPED_NOTIFS 10
std::vector<Notification> visibleNotifs;

std::vector<Notification> storedNotifs;
// Preferences notifPrefs;

// Grouped thread data
struct GroupedNotif {
    String app;
    String sender;
    std::vector<Notification> messages;
};

static std::vector<GroupedNotif> notifGroups;


void setupNotificationUI() {
    // Initially hide all notification cards
    for (int i = 0; i < MAX_NOTIFICATION_GROUPS; i++) {
        if (notif_cards[i]) {
            lv_obj_add_flag(notif_cards[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    lv_obj_clear_flag(objects.no_notif_card, LV_OBJ_FLAG_HIDDEN);  // Show "no notifications"
}

void updateNotifications() {
    visibleNotifs.clear();

    // Filter storedNotifs to unique threads (latest only)
    for (const auto& notif : storedNotifs) {
        bool alreadyAdded = false;
        for (const auto& visible : visibleNotifs) {
            if (notif.app == visible.app &&
                notif.title == visible.title) {
                alreadyAdded = true;
                break;
            }
        }
        if (!alreadyAdded) {
            visibleNotifs.push_back(notif);
            if (visibleNotifs.size() >= MAX_NOTIFICATION_GROUPS) break;
        }
    }

    // Show visibleNotifs in UI
    for (int i = 0; i < MAX_NOTIFICATION_GROUPS; i++) {
        if (i < visibleNotifs.size()) {
            const auto& notif = visibleNotifs[i];
            if (notif_cards[i])       lv_obj_clear_flag(notif_cards[i], LV_OBJ_FLAG_HIDDEN);
            if (app_labels[i])        lv_label_set_text(app_labels[i], notif.app.c_str());
            if (title_labels[i])      lv_label_set_text(title_labels[i], notif.title.c_str());
            if (message_labels[i])    lv_label_set_text(message_labels[i], notif.message.c_str());
            if (time_labels[i])       lv_label_set_text(time_labels[i], notif.time.c_str());
        } else {
            if (notif_cards[i])       lv_obj_add_flag(notif_cards[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (visibleNotifs.empty()) {
        lv_obj_clear_flag(objects.no_notif_card, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(objects.no_notif_card, LV_OBJ_FLAG_HIDDEN);
    }
}


void notificationTask(void* pvParameters) {
    while (true) {
        updateNotifications();
        vTaskDelay(pdMS_TO_TICKS(3000));  
    }
}

void setSelectedNotifIndex(int index) {
    if (visibleNotifs.empty()) {
        lv_obj_clear_flag(objects.no_notif_card, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (index < 0) index = 0;
    if (index >= visibleNotifs.size()) {
        currentSelectedIndex = 0;
        lv_scr_load(objects.wfpage);
        return;
    }

    currentSelectedIndex = index;

    for (int i = 0; i < MAX_NOTIFICATION_GROUPS; i++) {
        if (notif_cards[i]) {
            lv_obj_clear_state(notif_cards[i], LV_STATE_FOCUSED);
        }
    }

    if (notif_cards[currentSelectedIndex]) {
        lv_obj_add_state(notif_cards[currentSelectedIndex], LV_STATE_FOCUSED);
        lv_obj_scroll_to_view(notif_cards[currentSelectedIndex], LV_ANIM_ON);
    }
}



void dismissSelectedNotification() {
    if (currentSelectedIndex < 0 || currentSelectedIndex >= storedNotifs.size()) return;

    Notification selected = storedNotifs[currentSelectedIndex];

    // Remove all messages with same app+title OR app only (if no title)
    auto isSameThread = [&](const Notification& n) {
        if (!selected.title.isEmpty()) {
            return n.app == selected.app && n.title == selected.title;
        } else {
            return n.app == selected.app;
        }
    };

    storedNotifs.erase(
        std::remove_if(storedNotifs.begin(), storedNotifs.end(), isSameThread),
        storedNotifs.end()
    );

    updateNotifications();
    setSelectedNotifIndex(0);
}


void handleIncomingNotification(const Notification& notif) {
    if (storedNotifs.size() >= MAX_NOTIFICATION_GROUPS) {
        storedNotifs.erase(storedNotifs.begin());
    }
    storedNotifs.insert(storedNotifs.begin(), notif);
    if (storedNotifs.size() > MAX_NOTIFICATION_GROUPS) {
    storedNotifs.pop_back();  // remove the oldest one at the end
    }
    // saveNotificationsToFlash();
}

void openSelectedThread() {
    if (currentSelectedIndex < 0 || currentSelectedIndex >= visibleNotifs.size()) return;

    Notification& base = visibleNotifs[currentSelectedIndex];
    lv_scr_load(objects.threadviewpage);

    std::string threadText;

    // Find matching messages in storedNotifs
    for (int i = storedNotifs.size() - 1; i >= 0; i--) {
        if (storedNotifs[i].app == base.app &&
            storedNotifs[i].title == base.title) {
            threadText += storedNotifs[i].time.c_str();
            threadText += " - ";
            threadText += storedNotifs[i].message.c_str();
            threadText += "\n";
        }
    }

    if (threadText.empty()) {
        threadText = base.time.c_str();
        threadText += " - ";
        threadText += base.message.c_str();
    }

    lv_label_set_text(objects.thread_label, threadText.c_str());
}





// void saveNotificationsToFlash() {
//     notifPrefs.begin("notifs", false);
//     notifPrefs.putUInt("count", storedNotifs.size());

//     for (int i = 0; i < storedNotifs.size(); i++) {
//         String iStr = String(i);
//         notifPrefs.putString(("app" + iStr).c_str(), storedNotifs[i].app);
//         notifPrefs.putString(("title" + iStr).c_str(), storedNotifs[i].title);
//         notifPrefs.putString(("msg" + iStr).c_str(), storedNotifs[i].message);
//         notifPrefs.putString(("time" + iStr).c_str(), storedNotifs[i].time);
//     }

//     notifPrefs.end();
// }

// void loadNotificationsFromFlash() {
//     notifPrefs.begin("notifs", true);
//     int count = notifPrefs.getUInt("count", 0);
//     storedNotifs.clear();

//     for (int i = 0; i < count && i < MAX_NOTIFICATION_GROUPS; i++) {
//         Notification n;
//         String iStr = String(i);
//         n.app = notifPrefs.getString(("app" + iStr).c_str());
//         n.title = notifPrefs.getString(("title" + iStr).c_str());
//         n.message = notifPrefs.getString(("msg" + iStr).c_str());
//         n.time = notifPrefs.getString(("time" + iStr).c_str());
//         storedNotifs.push_back(n);
//     }

//     notifPrefs.end();
// }
