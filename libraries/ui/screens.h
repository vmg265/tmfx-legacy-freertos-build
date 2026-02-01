#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *wfpage;
    lv_obj_t *notificationspage;
    lv_obj_t *applistpage;
    lv_obj_t *pwm_motor_demoapppage;
    lv_obj_t *quicksettingspage;
    lv_obj_t *threadviewpage;
    lv_obj_t *notif_actionspage;
    lv_obj_t *acc_gyro_demopage;
    lv_obj_t *qr_demo_page;
    lv_obj_t *system_timer_app_page;
    lv_obj_t *system_settings_page;
    lv_obj_t *system_weatherpage;
    lv_obj_t *digital_wf;
    lv_obj_t *generic_list;
    lv_obj_t *time_label_zero;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *classic_analogue_twf_container;
    lv_obj_t *battery_status_wf_label;
    lv_obj_t *timelabel;
    lv_obj_t *minute_hand;
    lv_obj_t *hour_hand;
    lv_obj_t *second_hand;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *notiftext;
    lv_obj_t *connstatus;
    lv_obj_t *elegant_digital_twf_container;
    lv_obj_t *background_full_elegant_digital;
    lv_obj_t *digital_wf_hr_label_1;
    lv_obj_t *obj4;
    lv_obj_t *digital_wf_min_label_1;
    lv_obj_t *obj5;
    lv_obj_t *digital_wf_sec_label_2;
    lv_obj_t *obj6;
    lv_obj_t *notif_text_edwf;
    lv_obj_t *elegant_analogue_container;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *notif_list_container;
    lv_obj_t *notif_card_9;
    lv_obj_t *app_label_9;
    lv_obj_t *title_label_9;
    lv_obj_t *message_label_9;
    lv_obj_t *time_label_9;
    lv_obj_t *notif_card_8;
    lv_obj_t *app_label_8;
    lv_obj_t *title_label_8;
    lv_obj_t *message_label_8;
    lv_obj_t *time_label_8;
    lv_obj_t *notif_card_7;
    lv_obj_t *app_label_7;
    lv_obj_t *title_label_7;
    lv_obj_t *message_label_7;
    lv_obj_t *time_label_7;
    lv_obj_t *notif_card_6;
    lv_obj_t *app_label_6;
    lv_obj_t *title_label_6;
    lv_obj_t *message_label_6;
    lv_obj_t *time_label_6;
    lv_obj_t *notif_card_5;
    lv_obj_t *app_label_5;
    lv_obj_t *title_label_5;
    lv_obj_t *message_label_5;
    lv_obj_t *time_label_5;
    lv_obj_t *notif_card_4;
    lv_obj_t *app_label_4;
    lv_obj_t *title_label_4;
    lv_obj_t *message_label_4;
    lv_obj_t *time_label_4;
    lv_obj_t *notif_card_3;
    lv_obj_t *app_label_3;
    lv_obj_t *title_label_3;
    lv_obj_t *message_label_3;
    lv_obj_t *time_label_3;
    lv_obj_t *notif_card_2;
    lv_obj_t *app_label_2;
    lv_obj_t *title_label_2;
    lv_obj_t *message_label_2;
    lv_obj_t *time_label_2;
    lv_obj_t *notif_card_1;
    lv_obj_t *app_label_1;
    lv_obj_t *title_label_1;
    lv_obj_t *message_label_1;
    lv_obj_t *time_label_1;
    lv_obj_t *notif_card_0;
    lv_obj_t *app_label_0;
    lv_obj_t *title_label_0;
    lv_obj_t *message_label_0;
    lv_obj_t *time_label_0;
    lv_obj_t *no_notif_card;
    lv_obj_t *app_list_bg_container;
    lv_obj_t *obj9;
    lv_obj_t *app_label__0;
    lv_obj_t *app_label__1;
    lv_obj_t *app_in_focus_container;
    lv_obj_t *app_label__2;
    lv_obj_t *app_in_focus_app_list_quicklabel;
    lv_obj_t *app_label__3;
    lv_obj_t *app_label__4;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *dir_pwm_demo_label;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *step_pwm_demo_label;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *pulsewid_pwm_demo_label;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *interval_pwm_demo_label;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *glow_bottom;
    lv_obj_t *qsdemolabel;
    lv_obj_t *qs_container;
    lv_obj_t *focused_qs_container;
    lv_obj_t *qs_focused_text_container;
    lv_obj_t *qs_focused_text1;
    lv_obj_t *qs_focused_text2;
    lv_obj_t *qs_focused_text3;
    lv_obj_t *qs_focused_actiontext;
    lv_obj_t *normal_qs_container1;
    lv_obj_t *qs_normal_text_1;
    lv_obj_t *qs_normal_action_1;
    lv_obj_t *normal_qs_container_2;
    lv_obj_t *qs_normal_text_2;
    lv_obj_t *qs_normal_action_2;
    lv_obj_t *normal_qs_container_3;
    lv_obj_t *qs_normal_text_3;
    lv_obj_t *qs_normal_action_3;
    lv_obj_t *thread_label;
    lv_obj_t *acc_sensor_test_data_container;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *acc_label_x;
    lv_obj_t *acc_label_y;
    lv_obj_t *acc_label_z;
    lv_obj_t *acc_sensor_test_data_container_1;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *gyro_label_x;
    lv_obj_t *gyro_label_y;
    lv_obj_t *gyro_label_z;
    lv_obj_t *acc_sensor_test_data_container_2;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *timer_app_sec_label;
    lv_obj_t *timer_app_min_label;
    lv_obj_t *timer_app_hr_label;
    lv_obj_t *timer_app_days_label;
    lv_obj_t *settings_container_1;
    lv_obj_t *settings_wf_name_label;
    lv_obj_t *settings_container_2;
    lv_obj_t *digital_wf_hr_label;
    lv_obj_t *digital_wf_min_label;
    lv_obj_t *digital_wf_sec_label;
    lv_obj_t *digital_wf_sec_label_1;
    lv_obj_t *generic_list_name;
    lv_obj_t *glist_container_1;
    lv_obj_t *glist_container_2;
    lv_obj_t *glist_container_3;
    lv_obj_t *glist_container_4;
    lv_obj_t *glist_container_5;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_WFPAGE = 1,
    SCREEN_ID_NOTIFICATIONSPAGE = 2,
    SCREEN_ID_APPLISTPAGE = 3,
    SCREEN_ID_PWM_MOTOR_DEMOAPPPAGE = 4,
    SCREEN_ID_QUICKSETTINGSPAGE = 5,
    SCREEN_ID_THREADVIEWPAGE = 6,
    SCREEN_ID_NOTIF_ACTIONSPAGE = 7,
    SCREEN_ID_ACC_GYRO_DEMOPAGE = 8,
    SCREEN_ID_QR_DEMO_PAGE = 9,
    SCREEN_ID_SYSTEM_TIMER_APP_PAGE = 10,
    SCREEN_ID_SYSTEM_SETTINGS_PAGE = 11,
    SCREEN_ID_SYSTEM_WEATHERPAGE = 12,
    SCREEN_ID_DIGITAL_WF = 13,
    SCREEN_ID_GENERIC_LIST = 14,
};

void create_screen_wfpage();
void tick_screen_wfpage();

void create_screen_notificationspage();
void tick_screen_notificationspage();

void create_screen_applistpage();
void tick_screen_applistpage();

void create_screen_pwm_motor_demoapppage();
void tick_screen_pwm_motor_demoapppage();

void create_screen_quicksettingspage();
void tick_screen_quicksettingspage();

void create_screen_threadviewpage();
void tick_screen_threadviewpage();

void create_screen_notif_actionspage();
void tick_screen_notif_actionspage();

void create_screen_acc_gyro_demopage();
void tick_screen_acc_gyro_demopage();

void create_screen_qr_demo_page();
void tick_screen_qr_demo_page();

void create_screen_system_timer_app_page();
void tick_screen_system_timer_app_page();

void create_screen_system_settings_page();
void tick_screen_system_settings_page();

void create_screen_system_weatherpage();
void tick_screen_system_weatherpage();

void create_screen_digital_wf();
void tick_screen_digital_wf();

void create_screen_generic_list();
void tick_screen_generic_list();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/