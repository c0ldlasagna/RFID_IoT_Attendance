#pragma once

#include "freertos/idf_additions.h"

typedef struct{
    int r;
    int g;
    int b;
}rgb_config_t;


typedef enum{
    ATTENDANCE_ACCEPTED,
    ATTENDANCE_REJECTED,
    ATTENDANCE_FAILED,
}ATTENDANCE_STATUS;

extern TaskHandle_t notification_task_handle;

void init_notifications(int buzzerPin,int ledPin,rgb_config_t rgb_led_config);

void success_notification();

void fail_notification();

void disconnect_notification();

void notification_task(void* pvParameters);