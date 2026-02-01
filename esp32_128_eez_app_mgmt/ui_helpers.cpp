#include "ui_helpers.h"
#include "ui.h"
#include "lvgl.h"
#include <ChronosESP32.h>

void goToScreen(lv_obj_t* target) {
    if (target == nullptr) return;

    lv_scr_load_anim(target, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    lv_obj_invalidate(target);  
}



void invalidateAllChildren(lv_obj_t* parent) {
    if (!parent) return;

    uint32_t count = lv_obj_get_child_cnt(parent);
    for (uint32_t i = 0; i < count; i++) {
        lv_obj_t* child = lv_obj_get_child(parent, i);
        lv_obj_invalidate(child); 
    }

    lv_obj_invalidate(parent);
}

lv_obj_t* notif_cards[MAX_NOTIFICATION_GROUPS];
lv_obj_t* app_labels[MAX_NOTIFICATION_GROUPS];
lv_obj_t* title_labels[MAX_NOTIFICATION_GROUPS];
lv_obj_t* message_labels[MAX_NOTIFICATION_GROUPS];
lv_obj_t* time_labels[MAX_NOTIFICATION_GROUPS];


void registerNotificationCardPointers() {
    notif_cards[0] = objects.notif_card_0;
    notif_cards[1] = objects.notif_card_1;
    notif_cards[2] = objects.notif_card_2;
    notif_cards[3] = objects.notif_card_3;
    notif_cards[4] = objects.notif_card_4;
    notif_cards[5] = objects.notif_card_5;
    notif_cards[6] = objects.notif_card_6;
    notif_cards[7] = objects.notif_card_7;
    notif_cards[8] = objects.notif_card_8;
    notif_cards[9] = objects.notif_card_9;

    app_labels[0] = objects.app_label_0;
    app_labels[1] = objects.app_label_1;
    app_labels[2] = objects.app_label_2;
    app_labels[3] = objects.app_label_3;
    app_labels[4] = objects.app_label_4;
    app_labels[5] = objects.app_label_5;
    app_labels[6] = objects.app_label_6;
    app_labels[7] = objects.app_label_7;
    app_labels[8] = objects.app_label_8;
    app_labels[9] = objects.app_label_9;

    title_labels[0] = objects.title_label_0;
    title_labels[1] = objects.title_label_1;
    title_labels[2] = objects.title_label_2;
    title_labels[3] = objects.title_label_3;
    title_labels[4] = objects.title_label_4;
    title_labels[5] = objects.title_label_5;
    title_labels[6] = objects.title_label_6;
    title_labels[7] = objects.title_label_7;
    title_labels[8] = objects.title_label_8;
    title_labels[9] = objects.title_label_9;

    message_labels[0] = objects.message_label_0;
    message_labels[1] = objects.message_label_1;
    message_labels[2] = objects.message_label_2;
    message_labels[3] = objects.message_label_3;
    message_labels[4] = objects.message_label_4;
    message_labels[5] = objects.message_label_5;
    message_labels[6] = objects.message_label_6;
    message_labels[7] = objects.message_label_7;
    message_labels[8] = objects.message_label_8;
    message_labels[9] = objects.message_label_9;

    time_labels[0] = objects.time_label_0;
    time_labels[1] = objects.time_label_1;
    time_labels[2] = objects.time_label_2;
    time_labels[3] = objects.time_label_3;
    time_labels[4] = objects.time_label_4;
    time_labels[5] = objects.time_label_5;
    time_labels[6] = objects.time_label_6;
    time_labels[7] = objects.time_label_7;
    time_labels[8] = objects.time_label_8;
    time_labels[9] = objects.time_label_9;
}


// 1. Stable cleanup callback
static void toast_delete_ready_cb(lv_anim_t * a) {
    if (a->var) {
        lv_obj_del((lv_obj_t*)a->var);
    }
}

void show_notification_toast(Notification notification) {
    // 2. Create toast on the Top Layer
    lv_obj_t* toast = lv_obj_create(lv_layer_top());
    
    // Set size to fit within the 240px circle
    lv_obj_set_size(toast, 210, 210); 
    lv_obj_center(toast);
    
    // 3. Circular Styling
    lv_obj_set_style_radius(toast, LV_RADIUS_CIRCLE, 0); // Force circular
    lv_obj_set_style_bg_color(toast, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_bg_opa(toast, 240, 0);
    lv_obj_set_style_border_width(toast, 2, 0);
    lv_obj_set_style_border_color(toast, lv_color_hex(0x00AEEF), 0); // Highlight color
    lv_obj_set_style_clip_corner(toast, true, 0); // Prevent text spillover

    // 2. Time (Small, Top Center)
    lv_obj_t* lbl_time = lv_label_create(toast);
    lv_label_set_text(lbl_time, notification.time.c_str());
    lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_12, 0); 
    lv_obj_set_style_text_color(lbl_time, lv_color_hex(0x888888), 0);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_MID, 0, 15); // Pushed down slightly for the curve

    // 3. App Name (Bold, Medium Blue)
    lv_obj_t* lbl_app = lv_label_create(toast);
    lv_label_set_text(lbl_app, notification.app.c_str());
    lv_obj_set_style_text_font(lbl_app, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_app, lv_color_hex(0x00AEEF), 0); 
    lv_obj_align(lbl_app, LV_ALIGN_TOP_MID, 0, 35);

    // 4. Title (Large, Center)
    lv_obj_t* lbl_title = lv_label_create(toast);
    lv_label_set_text(lbl_title, notification.title.c_str());
    lv_label_set_long_mode(lbl_title, LV_LABEL_LONG_DOT);
    lv_obj_set_width(lbl_title, 160); // Narrower width to stay inside circle
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(lbl_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 60);

    // 5. Message Box (Left Aligned, Scrollable)
    lv_obj_t* msg_cont = lv_obj_create(toast);
    lv_obj_set_size(msg_cont, 150, 70); // Small box in the widest part of the circle
    lv_obj_align(msg_cont, LV_ALIGN_BOTTOM_MID, 0, -25);
    lv_obj_set_style_bg_opa(msg_cont, 0, 0);
    lv_obj_set_style_border_width(msg_cont, 0, 0);
    lv_obj_clear_flag(msg_cont, LV_OBJ_FLAG_SCROLLABLE); // Keep it simple for toast

    lv_obj_t* lbl_msg = lv_label_create(msg_cont);
    lv_label_set_text(lbl_msg, notification.message.c_str());
    lv_label_set_long_mode(lbl_msg, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lbl_msg, 150);
    lv_obj_set_style_text_font(lbl_msg, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(lbl_msg, LV_TEXT_ALIGN_LEFT, 0);


    // Robust Animation Logic
    // FADE IN
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, toast);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_time(&a, 500);
    lv_anim_set_exec_cb(&a, [](void* var, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)var, v, 0);
    });
    lv_anim_start(&a);

    // FADE OUT & DELETE
    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, toast);
    lv_anim_set_values(&b, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_time(&b, 500);
    lv_anim_set_delay(&b, 4000); // 4 seconds visible
    lv_anim_set_exec_cb(&b, [](void* var, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)var, v, 0);
    });
    lv_anim_set_ready_cb(&b, toast_delete_ready_cb); 
    lv_anim_start(&b);
}