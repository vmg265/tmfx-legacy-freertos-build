#ifndef WATCHFACE_MANAGER_H
#define WATCHFACE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Watchface Descriptor
typedef struct {
    const char* name;          // String name for debugging
    lv_obj_t** containerPtr;   // Pointer to objects.<container>
    bool hasAnalog;            // Whether analogue hands should be updated
} WatchfaceDescriptor;

// Add all watchfaces here 
#define WATCHFACE_LIST \
    X("classic_analogue",   &objects.classic_analogue_twf_container, true) \
    X("elegant_digital",    &objects.elegant_digital_twf_container, false) \
    X("elegant_analogue",   &objects.elegant_analogue_container, true)

// Public API
void watchfaceManager_init();
void watchfaceManagerOnSelectDoublePress();
void watchfaceManagerSetIndex(int index);
int  watchfaceManagerGetIndex();
void startWatchfaceTask();
#ifdef __cplusplus
}
#endif

#endif // WATCHFACE_MANAGER_H
