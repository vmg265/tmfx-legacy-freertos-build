#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_VAR_BATTERY_TEXT = 0,
    FLOW_GLOBAL_VARIABLE_VAR_CONNECTION_TEXT = 1,
    FLOW_GLOBAL_VARIABLE_VAR_NOTIFICATION_TEXT = 2,
    FLOW_GLOBAL_VARIABLE_VAR_TIME_HR_TEXT = 3,
    FLOW_GLOBAL_VARIABLE_VAR_TIME_MIN_TEXT = 4,
    FLOW_GLOBAL_VARIABLE_VAR_TIME_SEC_TEXT = 5,
    FLOW_GLOBAL_VARIABLE_VAR_TIME_HM_TEXT = 6,
    FLOW_GLOBAL_VARIABLE_VAR_TIME_HMS_TEXT = 7
};

// Native global variables

extern const char *get_var_var_battery_text();
extern void set_var_var_battery_text(const char *value);
extern const char *get_var_var_connection_text();
extern void set_var_var_connection_text(const char *value);
extern const char *get_var_var_notification_text();
extern void set_var_var_notification_text(const char *value);
extern const char *get_var_var_time_hr_text();
extern void set_var_var_time_hr_text(const char *value);
extern const char *get_var_var_time_min_text();
extern void set_var_var_time_min_text(const char *value);
extern const char *get_var_var_time_sec_text();
extern void set_var_var_time_sec_text(const char *value);
extern const char *get_var_var_time_hm_text();
extern void set_var_var_time_hm_text(const char *value);
extern const char *get_var_var_time_hms_text();
extern void set_var_var_time_hms_text(const char *value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/