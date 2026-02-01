#include "watchface_manager.h"
#include "battery_status.h"
#include <string>
#include <Preferences.h>
#include "ui.h"
#include "vars.h"
#include "ChronosESP32.h"
#include "ESP32Time.h"
#include <lvgl.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern ChronosESP32 chronos;
extern ESP32Time rtc;
extern volatile bool newNotificationReceived;
extern char notifBuffer[128];

static std::string v_battery;
static std::string v_conn;
static std::string v_notif;
static std::string v_hr;
static std::string v_min;
static std::string v_sec;
static std::string v_hm;
static std::string v_hms;

extern "C" {

const char *get_var_var_battery_text()     { return v_battery.c_str(); }
void set_var_var_battery_text(const char *v) { v_battery = (v ? v : ""); }

const char *get_var_var_connection_text()  { return v_conn.c_str(); }
void set_var_var_connection_text(const char *v) { v_conn = (v ? v : ""); }

const char *get_var_var_notification_text(){ return v_notif.c_str(); }
void set_var_var_notification_text(const char *v) { v_notif = (v ? v : ""); }

const char *get_var_var_time_hr_text()     { return v_hr.c_str(); }
void set_var_var_time_hr_text(const char *v) { v_hr = (v ? v : ""); }

const char *get_var_var_time_min_text()    { return v_min.c_str(); }
void set_var_var_time_min_text(const char *v) { v_min = (v ? v : ""); }

const char *get_var_var_time_sec_text()    { return v_sec.c_str(); }
void set_var_var_time_sec_text(const char *v) { v_sec = (v ? v : ""); }

const char *get_var_var_time_hm_text()     { return v_hm.c_str(); }
void set_var_var_time_hm_text(const char *v) { v_hm = (v ? v : ""); }

const char *get_var_var_time_hms_text()    { return v_hms.c_str(); }
void set_var_var_time_hms_text(const char *v) { v_hms = (v ? v : ""); }

} 

// Watchface registry auto-generated from WATCHFACE_LIST

#define X(name, containerPtr, hasAnalog) { name, containerPtr, hasAnalog },
static WatchfaceDescriptor wfList[] = {
    WATCHFACE_LIST
};
#undef X

static int WF_COUNT = sizeof(wfList) / sizeof(wfList[0]);
static int currentIndex = 0;

static Preferences prefs;
static TaskHandle_t wfTask = nullptr;

// Persistence keys
static const char *PNS = "wfspace";
static const char *PKEY = "index";


// Helpers
static void hideAll() {
    for (int i = 0; i < WF_COUNT; i++) {
        if (*wfList[i].containerPtr)
            lv_obj_add_flag(*wfList[i].containerPtr, LV_OBJ_FLAG_HIDDEN);
    }
}

static void applyWatchface(int idx) {
    hideAll();
    if (idx < 0 || idx >= WF_COUNT) idx = 0;
    currentIndex = idx;

    lv_obj_clear_flag(*wfList[idx].containerPtr, LV_OBJ_FLAG_HIDDEN);

    prefs.begin(PNS, false);
    prefs.putInt(PKEY, currentIndex);
    prefs.end();
}


// Update functions (write only to EEZ vars)
static void updateTimeVars() {
    int h = rtc.getHour(true);
    int m = rtc.getMinute();
    int s = rtc.getSecond();

    char bufHr[4], bufMin[4], bufSec[4], bufHM[8], bufHMS[12];

    snprintf(bufHr,  sizeof(bufHr),  "%02d", h);
    snprintf(bufMin, sizeof(bufMin), "%02d", m);
    snprintf(bufSec, sizeof(bufSec), "%02d", s);
    snprintf(bufHM,  sizeof(bufHM),  "%02d:%02d", h, m);
    snprintf(bufHMS, sizeof(bufHMS), "%02d:%02d:%02d", h, m, s);

    set_var_var_time_hr_text(bufHr);
    set_var_var_time_min_text(bufMin);
    set_var_var_time_sec_text(bufSec);
    set_var_var_time_hm_text(bufHM);
    set_var_var_time_hms_text(bufHMS);
}

static void updateConnectionVar() {
    set_var_var_connection_text(chronos.isConnected() ? "Connected" : "Disconnected");
}

static void updateNotificationVar() {
    if (newNotificationReceived) {
        set_var_var_notification_text(notifBuffer);
        newNotificationReceived = false;
    }
}

// Battery variable: YOU DO NOT TOUCH ANY LABELS HERE.
static void updateBatteryVar() {
    // Your battery_status.h should expose a "getBatteryString()" or I will simulate:
    extern String getBatteryString();   // <-- you must implement this small helper
    String txt = getBatteryString();    // returns "78% 3.8v" or anything you want printed
    set_var_var_battery_text(txt.c_str());
}


// Analogue hands rotation (only if wf.hasAnalog = true)
static void updateHands() {
    WatchfaceDescriptor &wf = wfList[currentIndex];
    if (!wf.hasAnalog) return;

    int h = rtc.getHour(true) % 12;
    int m = rtc.getMinute();
    int s = rtc.getSecond();

    int angleHour = (h * 300) + (m * 5);
    int angleMin  = m * 60;
    int angleSec  = s * 60;

    if (objects.hour_hand)   lv_img_set_angle(objects.hour_hand, angleHour);
    if (objects.minute_hand) lv_img_set_angle(objects.minute_hand, angleMin);
    if (objects.second_hand) lv_img_set_angle(objects.second_hand, angleSec);
}


// Background task (replaces old clockTask)
static void watchfaceTask(void *arg) {
    const TickType_t delay = pdMS_TO_TICKS(200);

    while (true) {
        updateTimeVars();
        updateConnectionVar();
        updateNotificationVar();
        updateBatteryVar();
        updateHands();
        vTaskDelay(delay);
    }
}


// Public API
void watchfaceManager_init() {
    prefs.begin(PNS, true);
    currentIndex = prefs.getInt(PKEY, 0);
    prefs.end();

    if (currentIndex < 0 || currentIndex >= WF_COUNT) currentIndex = 0;

    applyWatchface(currentIndex);

    if (!wfTask) {
        xTaskCreatePinnedToCore(watchfaceTask,
                                "wfTask",
                                4096,
                                NULL,
                                1,
                                &wfTask,
                                1);
    }
}

void watchfaceManagerSetIndex(int index) {
    if (index < 0) index = 0;
    if (index >= WF_COUNT) index = 0;
    applyWatchface(index);
}

int watchfaceManagerGetIndex() {
    return currentIndex;
}

void watchfaceManagerOnSelectDoublePress() {
    int next = (currentIndex + 1) % WF_COUNT;
    watchfaceManagerSetIndex(next);

    updateTimeVars();
    updateConnectionVar();
    updateNotificationVar();
    updateBatteryVar();
    updateHands();
}

void startWatchfaceTask() {
   
        xTaskCreatePinnedToCore(
            watchfaceTask,
            "Watchface Task",
            4096,
            NULL,
            1,
            NULL,
            1
        );
    
}

