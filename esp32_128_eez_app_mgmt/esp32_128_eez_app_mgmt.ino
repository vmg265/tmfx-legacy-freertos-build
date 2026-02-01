#include <lvgl.h>
#include "lv_conf.h"
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "ui.h"
#include <screens.h>
#include <TFT_eSPI.h>
#include <ChronosESP32.h>
#include <ESP32Time.h>
#include "button_handler.h"
#include "notification_handler.h"
#include "ui_helpers.h"
#include "app_list.h"
#include "pwm_test.h"
#include "qmi8658_driver.h"
#include "quicksettings.h"
#include "battery_status.h"
#include "watchface_manager.h"
#include "display_manager.h"
#include "esp_wifi.h"
#include "esp_bt.h"
#include "esp_sleep.h"

//////demo qs includes
void setupQuickSettingsEntries() {
    registerQuickSetting({
        .label = "Bluetooth",
        .sublabel1 = "On",
        .sublabel2 = "Connected",
        .actionLabel = "TOGGLE",
        .color = lv_color_hex(0xff20223c),
        .action = [](bool doubleTap) {
            // Toggle Bluetooth
            Serial.println(doubleTap ? "BT double tap!" : "BT single tap!");
        }
    });

    registerQuickSetting({
        .label = "Flashlight",
        .sublabel1 = "Off",
        .sublabel2 = "",
        .actionLabel = "ON",
        .color = lv_color_hex(0xfffbc02d),
        .action = [](bool doubleTap) {
            Serial.println(doubleTap ? "Flashlight (double)" : "Flashlight (single)");
        }
    });

    registerQuickSetting({
        .label = "Wi-Fi",
        .sublabel1 = "Offline",
        .sublabel2 = "SSID: none",
        .actionLabel = "CONNECT",
        .color = lv_color_hex(0xff4caf50),
        .action = [](bool doubleTap) {
            Serial.println(doubleTap ? "Wi-Fi (double)" : "Wi-Fi (single)");
        }
    });
    
    registerQuickSetting({
        .label = "Wi-Fi2",
        .sublabel1 = "Offline",
        .sublabel2 = "SSID: none",
        .actionLabel = "CONNECT",
        .color = lv_color_hex(0xff2f5d57),
        .action = [](bool doubleTap) {
            Serial.println(doubleTap ? "Wi-Fi (double)" : "Wi-Fi (single)");
        }
    });

    registerQuickSetting({
        .label = "Wi-Fi3",
        .sublabel1 = "Offline",
        .sublabel2 = "SSID: none",
        .actionLabel = "CONNECT",
        .color = lv_color_hex(0xff4caf50),
        .action = [](bool doubleTap) {
            Serial.println(doubleTap ? "Wi-Fi (double)" : "Wi-Fi (single)");
        }
    });
    updateQuickSettingsUI();
}

IMUData imuData;

extern const int appListSize;
extern lv_obj_t* notif_cards[MAX_NOTIFICATION_GROUPS];
Notification pendingToastNotif;
volatile bool toastPending = false;

extern bool demomode;
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define DEMO_SCREEN_COUNT 5
const unsigned long screenInterval = 5000;  

ChronosESP32 chronos;
ESP32Time rtc;
lv_obj_t* demoScreens[DEMO_SCREEN_COUNT];

char notifBuffer[128];
volatile bool newNotificationReceived = false;

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); 

#if LV_USE_LOG != 0
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp_drv );
}

void example_increase_lvgl_tick(void *arg)
{
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}


// ==================== BLE Callbacks ====================

void bleCallback(uint8_t* data, int length) {
    String jsonString;
    for (int i = 0; i < length; i++) {
        jsonString += (char)data[i];
    }
}

void notificationCallback(Notification notification) {
    snprintf(notifBuffer, sizeof(notifBuffer), "[%s] %s: %s: %s",
             notification.time.c_str(),
             notification.app.c_str(),
             notification.title.c_str(),
             notification.message.c_str());
    
    pendingToastNotif = notification;
    toastPending = true; 
    
    handleIncomingNotification(notification);
    newNotificationReceived = true;
}

void updateBatteryStatus() {
    static unsigned long lastSync = 0;
    const unsigned long syncInterval = 60000; 

    if (millis() - lastSync > syncInterval) {
        lastSync = millis();

        float voltage = getBatteryVoltage(); 
        
        int percent = getBatteryPercentage(voltage); 

        chronos.setBattery(percent); 
        
        Serial.printf("Battery Synced to App: %d%%\n", percent);
    }
}

// ==================== RTOS Tasks ====================

void uiTask(void* pvParameters) {
    while (true) {
        lv_timer_handler();  // Handle LVGL updates
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}


void stepperTask(void *parameter) {
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);

    while (1) {
        if (autoMode && autoStepping) {
            digitalWrite(DIR_PIN, dirCW ? HIGH : LOW);
            for (int i = 0; i < stepsPerMove; i++) {
                digitalWrite(STEP_PIN, HIGH);
                delayMicroseconds(pulseWidthMicros);
                digitalWrite(STEP_PIN, LOW);
                delayMicroseconds(pulseWidthMicros);
            }
            vTaskDelay(pdMS_TO_TICKS(stepIntervalMs));
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));  // Idle
        }
    }
}




//////////////////////////////////////
void setup(){ 

  Serial.begin( 115200 ); /* prepare for possible serial debug */
  // +++ INITIALIZE FILESYSTEM +++
  // if(!LittleFS.begin(true)){ // 'true' formats if not present
  //   Serial.println("LittleFS Mount Failed. Halting.");
  //   while(1);
  // }
  // Serial.println("LittleFS Mounted.");
  // +++++++++++++++++++++++++++++
  esp_wifi_stop();
  esp_wifi_deinit();

  setCpuFrequencyMhz(80);
    pinMode(40, OUTPUT); 
    digitalWrite(40, HIGH); // Turn it on at boot  
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );
    batteryStatusInit();

    ////Iniatialize LVGL
    lv_init();

    tft.begin();          /* TFT init */
    tft.setRotation( 2 ); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );
  
   
    const esp_timer_create_args_t lvgl_tick_timer_args = {
      .callback = &example_increase_lvgl_tick,
      .name = "lvgl_tick"
    };

    esp_timer_handle_t lvgl_tick_timer = NULL;
    esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

    ui_init();
    // Hide all watchface containers first
    lv_obj_add_flag(objects.classic_analogue_twf_container, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.elegant_digital_twf_container, LV_OBJ_FLAG_HIDDEN);

    // Unhide the default one (classic analogue)
    lv_obj_clear_flag(objects.elegant_digital_twf_container, LV_OBJ_FLAG_HIDDEN);
    
    registerNotificationCardPointers();
    // Optional: Debug check
    Serial.printf("notiftext ptr: %p\n", objects.notiftext);

    delay(10);
    demoScreens[0] = objects.wfpage;
    demoScreens[1] = objects.notificationspage;
    demoScreens[2] = objects.pwm_motor_demoapppage;
    demoScreens[3] = objects.quicksettingspage;
    demoScreens[4] = objects.applistpage;

    notif_cards[0] = objects.notif_card_0;
    app_labels[0] = objects.app_label_0;
    title_labels[0] = objects.title_label_0;
    message_labels[0] = objects.message_label_0;
    time_labels[0] = objects.time_label_0;

    notif_cards[1] = objects.notif_card_1;
    app_labels[1] = objects.app_label_1;
    title_labels[1] = objects.title_label_1;
    message_labels[1] = objects.message_label_1;
    time_labels[1] = objects.time_label_1;

    notif_cards[2] = objects.notif_card_2;
    app_labels[2] = objects.app_label_2;
    title_labels[2] = objects.title_label_2;
    message_labels[2] = objects.message_label_2;
    time_labels[2] = objects.time_label_2;

    notif_cards[3] = objects.notif_card_3;
    app_labels[3] = objects.app_label_3;
    title_labels[3] = objects.title_label_3;
    message_labels[3] = objects.message_label_3;
    time_labels[3] = objects.time_label_3;

    notif_cards[4] = objects.notif_card_4;
    app_labels[4] = objects.app_label_4;
    title_labels[4] = objects.title_label_4;
    message_labels[4] = objects.message_label_4;
    time_labels[4] = objects.time_label_4;

    notif_cards[5] = objects.notif_card_5;
    app_labels[5] = objects.app_label_5;
    title_labels[5] = objects.title_label_5;
    message_labels[5] = objects.message_label_5;
    time_labels[5] = objects.time_label_5;

    notif_cards[6] = objects.notif_card_6;
    app_labels[6] = objects.app_label_6;
    title_labels[6] = objects.title_label_6;
    message_labels[6] = objects.message_label_6;
    time_labels[6] = objects.time_label_6;

    notif_cards[7] = objects.notif_card_7;
    app_labels[7] = objects.app_label_7;
    title_labels[7] = objects.title_label_7;
    message_labels[7] = objects.message_label_7;
    time_labels[7] = objects.time_label_7;

    notif_cards[8] = objects.notif_card_8;
    app_labels[8] = objects.app_label_8;
    title_labels[8] = objects.title_label_8;
    message_labels[8] = objects.message_label_8;
    time_labels[8] = objects.time_label_8;

    notif_cards[9] = objects.notif_card_9;
    app_labels[9] = objects.app_label_9;
    title_labels[9] = objects.title_label_9;
    message_labels[9] = objects.message_label_9;
    time_labels[9] = objects.time_label_9;
    
    if (appList.size() > 2) {
    currentAppIndex = 2;
    } else {
    currentAppIndex = 0;
    }

    applist_labels[0] = objects.app_label__0;
    applist_labels[1] = objects.app_label__1;
    applist_labels[2] = objects.app_label__2;
    applist_labels[3] = objects.app_label__3;
    applist_labels[4] = objects.app_label__4;

    setupQuickSettingsEntries();

    chronos.setDataCallback(bleCallback);
    chronos.setNotificationCallback(notificationCallback);
    chronos.begin();

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1) {
        
        Serial.println("Woke up from deep sleep via button.");
    }


    setupButtons();
    setupNotificationUI();
    xTaskCreatePinnedToCore(notificationTask, "Notification Task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(uiTask, "UI Task", 4096, NULL, 1, NULL, 1);
    startWatchfaceTask();

    xTaskCreatePinnedToCore(buttonTask, "Button Task", 4096, NULL, 1, NULL, 1);
    Serial.println("Calling updateAppListUI...");
    if (appList.size() > 2) {
      currentAppIndex = 2;
      } else {
      currentAppIndex = 0;
      }
    delay(100); 
    if (appList.size() > 2) currentAppIndex = 2;
        else currentAppIndex = 0;
    updateAppListUI();
    Serial.println("Update finished!");
    xTaskCreatePinnedToCore(stepperTask,"Stepper Task", 2048 , NULL , 1 ,NULL, 1 );

    if (!initQMI8658Sensor()) {
        Serial.println("IMU init failed");
        while (true); // halt
    }
    startIMUTask();
    Serial.println( "Setup done" ); 
}

void loop() {
  if (toastPending) {
        toastPending = false;
        wakeDisplay(); 
        show_notification_toast(pendingToastNotif);
    }
    lv_timer_handler();
    updateDisplayManager(); 
    updateBatteryStatus();
    ui_tick();
    delay(5);
}

