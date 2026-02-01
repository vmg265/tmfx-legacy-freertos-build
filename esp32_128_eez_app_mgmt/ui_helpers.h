#ifndef UI_HELPERS_H
#define UI_HELPERS_H

#include <ChronosESP32.h> 
#include <lvgl.h>
#include "ui.h"

void show_notification_toast(Notification notification);
void goToScreen(lv_obj_t* target);
void invalidateAllChildren(lv_obj_t* parent); 

#define MAX_NOTIFICATION_GROUPS 10

extern lv_obj_t* notif_cards[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* app_labels[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* title_labels[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* message_labels[MAX_NOTIFICATION_GROUPS];
extern lv_obj_t* time_labels[MAX_NOTIFICATION_GROUPS];

void registerNotificationCardPointers();

extern lv_obj_t* notif_cards[10];
extern lv_obj_t* app_labels[10];
extern lv_obj_t* title_labels[10];
extern lv_obj_t* message_labels[10];
extern lv_obj_t* time_labels[10];

#endif
