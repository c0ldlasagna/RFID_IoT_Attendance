#include "include/RGB_LED.h"
#include "include/notifications.h"
#include "notifications.h"
#include "HardwareSerial.h"
#include "esp32-hal-ledc.h"
#include "Adafruit_NeoPixel.h"
#include "freertos/idf_additions.h"
#include <cstdint>

NotificationClass Notification;

TaskHandle_t notification_task_handle = nullptr;
void notification_task(void* pvParameters){
    uint32_t ulNotificationValue;
    for(;;){
        xTaskNotifyWait(0, ULONG_MAX, &ulNotificationValue, portMAX_DELAY);
        Notification.notify((notification_t)ulNotificationValue);
        }
}

void NotificationClass::notify_success(){
        for(int i = 0;i<2;i++){
            pixel.setPixelColor(0,0,50,0);
            pixel.show();
            ledcWriteNote(buzzer_pin, success_tones[i],success_octaves[i]);
            vTaskDelay(pdMS_TO_TICKS(100));

            pixel.setPixelColor(0,0,0,0);
            pixel.show();
            ledcWriteTone(buzzer_pin, 0);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
}

void NotificationClass::notify_failure(){
    for(int i = 0;i<2;i++){

        pixel.setPixelColor(0,50,0,0);
        pixel.show();
        ledcWriteNote(buzzer_pin, fail_tones[i],fail_octaves[i]);
        vTaskDelay(pdMS_TO_TICKS(100));

        pixel.setPixelColor(0,0,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void NotificationClass::notify_disconnected(){
    for(int i = 0; i<5; i++){
        pixel.setPixelColor(0,50,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, disconnect_tones[i]);
        vTaskDelay(pdMS_TO_TICKS(100));

        pixel.setPixelColor(0,0,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void NotificationClass::notify_duplicate(){
    for(int i = 0; i<3; i++){
        pixel.setPixelColor(0,50,5,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 800);
        vTaskDelay(pdMS_TO_TICKS(100));

        pixel.setPixelColor(0,0,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void NotificationClass::notify_register(){
    for(int i =0;i<3;i++){
        pixel.setPixelColor(0,0,0,100);
        pixel.show();
        ledcWriteNote(buzzer_pin, register_tones[i],register_octaves[i]);
        vTaskDelay(pdMS_TO_TICKS(100));

        pixel.setPixelColor(0,0,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void NotificationClass::notify_late(){
        for(int i =0;i<3;i++){
        pixel.setPixelColor(0,100,100,0);
        pixel.show();
        ledcWriteNote(buzzer_pin, late_tones[i],3);
        vTaskDelay(pdMS_TO_TICKS(100));

        pixel.setPixelColor(0,0,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void NotificationClass::notify(notification_t notification){
    switch(notification){
        
        // Beep success and flash green
        case notification_t::NOTIFY_SUCCESS:
            notify_success();
            break;

        // Beep failure and flash red
        case notification_t::NOTIFY_FAIL:
            notify_failure();
            break;

        //Beep late and flash yellow
        case notification_t::NOTIFY_LATE:
            notify_late();
            break;
        
        //Beep registered and flash blue
        case notification_t::NOTIFY_REGISTERED:
            notify_register();
            break;

        //Beep disconnected and flash orange
        case notification_t::NOTIFY_DISCONNECTED:
            notify_disconnected();
            break;
        
        //Beep duplicate and flash orange
        case notification_t::NOTIFY_DUPLICATE:
            notify_duplicate();
            break;
        
        // Set status LED to red
        case notification_t::SET_STATUS_DISCONNECTED:
            set_rgb({100,0,0,});
            break;

        // Set status LED to green
        case notification_t::SET_STATUS_ACTIVE:
            set_rgb({0,100,0});
            break;

        // Set status LED to white
        case notification_t::SET_STATUS_IDLE:
            set_rgb({100,100,100});
            break;

        // Set status LED to blue
        case notification_t::SET_STATUS_REGISTERING:
            set_rgb({0,0,100});
            break;

        // Set status LED to orange
        case notification_t::SET_STATUS_LATE:
            set_rgb({255,20,0});
            break;
    }
}


void NotificationClass::set_rgb(rgb_colour_t colour){
    ledcWrite(this->rgb_config.r, colour.r);
    ledcWrite(this->rgb_config.g, colour.g);
    ledcWrite(this->rgb_config.b, colour.b);
}

bool NotificationClass::begin(int buzzer_pin, int builtinLEDPin, RGB_LED rgbLED){
    this->buzzer_pin = buzzer_pin;
    this->pixel = Adafruit_NeoPixel(1,
                                    builtinLEDPin,
                                    NEO_GRB+NEO_KHZ800);
    if(!ledcAttach(buzzer_pin, 1000, 14)){
        Serial.println("Failed to attach Buzzer");
        return false;
    }

    this->rgb_config = rgbLED;
    if(!rgbLED.begin()){
        return false;
    }
    
    xTaskCreate(notification_task,
                "notification_task",
                4096,
                NULL,
                1,
                &notification_task_handle);
                
    return true;
};