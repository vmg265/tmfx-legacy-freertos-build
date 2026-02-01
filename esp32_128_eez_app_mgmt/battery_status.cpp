#include "battery_status.h"
#include "DEV_Config.h"  

#define BATTERY_SAMPLES 5
#define BATTERY_UPDATE_INTERVAL 30000 

// Init
void batteryStatusInit() {
    pinMode(BAT_ADC_PIN, INPUT);
}

// Read Battery Voltage
float getBatteryVoltage() {
    const float VOLTAGE_DIVIDER_RATIO = 2.0f; 
    uint32_t total = 0;

    for (int i = 0; i < BATTERY_SAMPLES; i++) {
        total += DEC_ADC_Read(); 
        delay(5);
    }

    float mV_at_pin = (float)total / BATTERY_SAMPLES;
    
    float voltage = (mV_at_pin / 1000.0f) * VOLTAGE_DIVIDER_RATIO;
    return voltage;
}

// Estimate Battery Percentage
int getBatteryPercentage(float voltage) {
    if (voltage >= 4.20f) return 100; 
    if (voltage <= 3.20f) return 0;   
    
    if (voltage > 4.00f) {
        return 80 + (int)((voltage - 4.00f) * 100.0f); // Mapping 4.0-4.2 to 80-100%
    } else if (voltage > 3.70f) {
        return 10 + (int)((voltage - 3.70f) * (70.0f / 0.30f)); // Mapping 3.7-4.0 to 10-80%
    } else {
        return (int)((voltage - 3.20f) * (10.0f / 0.50f)); // Mapping 3.2-3.7 to 0-10%
    }
}

// Return battery string for watchface manager
String getBatteryString() {
    static unsigned long lastUpdate = 0;
    static String cached = "";

    unsigned long now = millis();
    if (now - lastUpdate < BATTERY_UPDATE_INTERVAL) {
        return cached;
    }
    lastUpdate = now;

    float voltage = getBatteryVoltage();
    int percent = getBatteryPercentage(voltage);

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d%% %.2fV", percent, voltage);

    cached = buffer;
    return cached;
}
