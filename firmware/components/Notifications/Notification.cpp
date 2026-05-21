#include "notifications.h"
#include "HardwareSerial.h"
#include "esp32-hal-ledc.h"
#include "Adafruit_NeoPixel.h"
#include "freertos/idf_additions.h"

NotificationClass Notification;

TaskHandle_t notification_task_handle = nullptr;
void notification_task(void* pvParameters){
    uint32_t ulNotificationValue;
    for(;;){
        xTaskNotifyWait(0, ULONG_MAX, &ulNotificationValue, portMAX_DELAY);
        Notification.notify((notification_type_t)ulNotificationValue);
        }
}

void NotificationClass::notify_success(){
    note_t tones[2] = {NOTE_G,NOTE_E};
        for(int i = 0;i<2;i++){

            pixel.setPixelColor(0,0,50,0);
            pixel.show();
            ledcWriteNote(buzzer_pin, tones[i],5+i);
            vTaskDelay(pdMS_TO_TICKS(100));

            pixel.setPixelColor(0,0,0,0);
            pixel.show();
            ledcWriteTone(buzzer_pin, 0);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
}

void NotificationClass::notify_failure(){
    note_t tones[2] = {NOTE_Cs,NOTE_C};
    for(int i = 0;i<2;i++){

        pixel.setPixelColor(0,50,0,0);
        pixel.show();
        ledcWriteNote(buzzer_pin, tones[i],3);
        vTaskDelay(pdMS_TO_TICKS(100));

        pixel.setPixelColor(0,0,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void NotificationClass::notify_disconnected(){
int tones[5] = {1000,900,800,700,600};
    for(int i = 0; i<5; i++){
        pixel.setPixelColor(0,50,50,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, tones[i]);
        vTaskDelay(pdMS_TO_TICKS(100));

        pixel.setPixelColor(0,0,0,0);
        pixel.show();
        ledcWriteTone(buzzer_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
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