#pragma once

#include "Adafruit_NeoPixel.h"
#include "freertos/idf_additions.h"
#include <cstdint>

// Struct for external RGB LED configuration
typedef struct{
    int r;
    int g;
    int b;
    bool begin();
}RGB_LED;

enum class Attendance_status:uint8_t{
    Present,
    Late
};

// Enum for notifications
typedef enum{
    NOTIFY_SUCCESS,
    NOTIFY_LATE,
    NOTIFY_FAIL,
    NOTIFY_DISCONNECTED
}notification_type_t;

enum Status:uint8_t{
    Session_Active,
    Session_Late,
    Inactive,
    Registering,
    Disconnected
};


class NotificationClass{
    public:
    uint8_t buzzer_pin;
    Adafruit_NeoPixel pixel;
    RGB_LED rgb_config;
    bool begin(int buzzer_pin, int led_pin, RGB_LED rgbLED);
    void notify(notification_type_t notification_type);
    void set_status(Status status);
    private:
    void notify_success();
    void notify_failure();
    void notify_disconnected();
};


void notification_task(void* pvParameters);

extern TaskHandle_t notification_task_handle;
extern NotificationClass Notification;