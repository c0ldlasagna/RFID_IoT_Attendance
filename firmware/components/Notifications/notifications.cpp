#include "notifications.h"
#include "esp32-hal-ledc.h"
#include "Adafruit_NeoPixel.h"
#include "freertos/idf_additions.h"

static int buzzer_pin;
static int led_pin;
static Adafruit_NeoPixel pixel;

void init_notifications(int buzzerPin, int builtinLEDPin, rgb_config_t rgbLED){
    buzzer_pin = buzzerPin;
    led_pin = builtinLEDPin;

    ledcAttach(buzzer_pin, 1000, 14);
    pixel = Adafruit_NeoPixel(1,led_pin,NEO_GRB+NEO_KHZ800);

}

void success_notification(){
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

void fail_notification(){
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

void disconnect_notification(){
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

TaskHandle_t notification_loop_handle = nullptr;
void notification_loop(void* pvParameters){
    uint32_t ulNotificationValue;
    for(;;){
        xTaskNotifyWait(0, ULONG_MAX, &ulNotificationValue, portMAX_DELAY);
        switch(ulNotificationValue){
            case ATTENDANCE_ACCEPTED:
                success_notification();
                break;

            case ATTENDANCE_REJECTED:
                disconnect_notification();
                break;

            case ATTENDANCE_FAILED:
                fail_notification();
                break;
        }
    }
}


