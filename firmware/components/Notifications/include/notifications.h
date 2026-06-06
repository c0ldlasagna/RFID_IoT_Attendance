#pragma once

#include "Adafruit_NeoPixel.h"
#include "esp32-hal-ledc.h"
#include "freertos/idf_additions.h"
#include <cstdint>
#include "RGB_LED.h"

// Enum for notifications
typedef enum{
    NOTIFY_SUCCESS,
    NOTIFY_LATE,
    NOTIFY_FAIL,
    NOTIFY_DISCONNECTED,
    NOTIFY_DUPLICATE,
    NOTIFY_REGISTERED,
    SET_STATUS_ACTIVE,
    SET_STATUS_LATE,
    SET_STATUS_REGISTERING,
    SET_STATUS_IDLE,
    SET_STATUS_DISCONNECTED
}notification_t;

class NotificationClass{
    public:

        uint8_t buzzer_pin;
        Adafruit_NeoPixel pixel;
        RGB_LED rgb_config;
        bool begin(int buzzer_pin, int led_pin, RGB_LED rgbLED);
        void notify(notification_t notification);
    
    private:

        void notify_success();
        inline static constexpr note_t success_tones[] = {NOTE_G,NOTE_E};
        inline static constexpr uint8_t success_octaves[] = {5,6};
        
        void notify_failure();
        inline static constexpr note_t fail_tones[] = {NOTE_Cs,NOTE_C};
        inline static constexpr uint8_t fail_octaves[] = {3,3};
        
        void notify_disconnected();
        inline static constexpr int disconnect_tones[] = {1000,900,800,700,600};

        void notify_duplicate();
        
        void notify_register();
        inline static constexpr note_t register_tones[] = {NOTE_Gs,NOTE_F,NOTE_Cs};
        inline static constexpr uint8_t register_octaves[] = {4,4,5};

        void notify_late();
        inline static constexpr note_t late_tones[] = {NOTE_Bb,NOTE_Gs,NOTE_D};

        void set_rgb(rgb_colour_t colour);
};

void notification_task(void* pvParameters);

extern TaskHandle_t notification_task_handle;
extern NotificationClass Notification;
