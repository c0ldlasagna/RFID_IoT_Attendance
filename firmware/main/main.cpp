#include "Arduino.h"
#include "HardwareSerial.h"
#include "IPAddress.h"
#include "MFRC522Constants.h"
#include "NetworkClient.h"
#include "NetworkManager.h"
#include "SPI.h"
#include "MFRC522v2.h"
#include "MFRC522DriverSPI.h"
#include "MFRC522DriverPinSimple.h"
#include "MFRC522Debug.h"
#include "esp32-hal-ledc.h"
#include "esp32-hal.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "portmacro.h"
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <sstream>
#include <string>
#include <sys/_types.h>
#include <sys/types.h>
#include "notifications.h"
#include "notifications.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "string.h"

// SPI Pins
#define SS_PIN   10
#define SCK_PIN  13
#define MISO_PIN 12
#define MOSI_PIN 11
#define BUZZER_PIN 37
#define LED_PIN 48

// WiFi Information
#define SSID    "hassanminar1003"
#define PASSWD  "eliasadil1708"

//Notifications

rgb_config_t rgb_config{19,20,21};

// Networking
NetworkClient client;
HTTPClient httpclient;

IPAddress serverIP(192,168,68,60);

// Network test
TaskHandle_t network_test_handle = NULL;
void network_test(void* pvParameters){
    uint32_t ulNotificationValue;
    for(;;){
        
        //  Wait for card to send request
        xTaskNotifyWait(0, ULONG_MAX, &ulNotificationValue, portMAX_DELAY);

        if(Network.isOnline()){           
        
            int start = millis();
        
            int status = httpclient.GET();
        
            switch (status){
        
                case t_http_codes::HTTP_CODE_OK:
                    Serial.print("Ping:");
                    Serial.print(millis()-start);
                    Serial.println("ms");
                    if(httpclient.getString() == "0"){
                        xTaskNotify(notification_task_handle, 0, eSetBits);
                    }
                    else {
                        xTaskNotify(notification_task_handle, 1, eSetBits);                    
                    }
                    continue;
        
                default:
                    Serial.println(httpclient.errorToString(status));
                    xTaskNotify(notification_task_handle, 2, eSetBits);
                    continue;
            }
        }
        xTaskNotify(notification_task_handle, 2, eSetBits);
    }
}

// RFID Setup and Read loop task
MFRC522DriverPinSimple ss_pin(SS_PIN);
MFRC522DriverSPI driver(ss_pin);
MFRC522 reader(driver);

TaskHandle_t reader_loop_handle = NULL;
void reader_loop(void* pvParameters){
    for(;;){
            if(reader.PICC_IsNewCardPresent()){
                if(reader.PICC_ReadCardSerial()){
                    char uid_buffer[64];
                    int offset = 0;
                    uid_buffer[0] = '\0';
                        for (int i = 0; i < reader.uid.size; i++) {
                        offset += snprintf(
                            uid_buffer + offset,
                            sizeof(uid_buffer) - offset,
                            "%02X",
                            reader.uid.uidByte[i]
                        );
                        }   
                    Serial.printf("UID: %s ",uid_buffer);
                    Serial.print("\n");
                xTaskNotify(network_test_handle, 0, eSetBits);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(500));
        }
}

// Main Function
extern "C" void app_main()
{
    // Initialize Arduino Library
    initArduino();

    // Begin Serial
    Serial.begin(115200);
    
    // Initialize SPI
    SPI.begin(13,12,11,10);
    SPI.setFrequency(250000);

    // Initialize RFID Reader
    reader.PCD_Init();
    reader.PCD_SetAntennaGain(MFRC522Constants::RxGain_max);
    MFRC522Debug::PCD_DumpVersionToSerial(reader, Serial);


    // Initialize Notifications
    init_notifications(BUZZER_PIN, LED_PIN,rgb_config);

    ledcAttach(19, 10000, 8);
    ledcAttach(20, 10000, 8);
    ledcAttach(21, 10000, 8);

    // Networking Setup
    Network.begin();
    WiFi.setSleep(false);
    WiFi.STA.begin();
    WiFi.STA.connect(SSID,PASSWD);
    httpclient.begin(client,"http://192.168.68.60:3000");
    httpclient.setConnectTimeout(200);


    // Reader Task Loop
    xTaskCreate(reader_loop,
                "reader_loop",
                4096,
                NULL,
                1,
                &reader_loop_handle);
    
    xTaskCreate(notification_task,
                "notification_loop",
                4096,
                NULL,
                2,
                &notification_task_handle);

    xTaskCreate(network_test,
                "network_test",
                4096,
                NULL,
                2,
                &network_test_handle);
}