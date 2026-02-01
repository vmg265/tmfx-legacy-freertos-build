#ifndef NOTIFICATION_HANDLER_H
#define NOTIFICATION_HANDLER_H

#include <Arduino.h>
#include <vector>
#include <Preferences.h>
#include <ChronosESP32.h>
#include <lvgl.h>

#define MAX_NOTIFICATION_GROUPS 10

void setupNotificationUI();
void updateNotifications();
void setSelectedNotifIndex(int index);
void dismissSelectedNotification();
void openSelectedThread();
void notificationTask(void* pvParameters);
void handleIncomingNotification(const Notification& notif);
// void saveNotificationsToFlash();
// void loadNotificationsFromFlash();

// External array pointers for notification card UI
extern lv_obj_t* notif_cards[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* app_labels[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* title_labels[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* message_labels[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* time_labels[MAX_NOTIFICATION_GROUPS];

extern int currentSelectedIndex;
extern std::vector<Notification> storedNotifs;


#endif
