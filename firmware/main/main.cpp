#include "Arduino.h"
#include "HardwareSerial.h"
#include "MFRC522Constants.h"
#include "NetworkClient.h"
#include "NetworkManager.h"
#include "SPI.h"
#include "MFRC522v2.h"
#include "MFRC522DriverSPI.h"
#include "MFRC522DriverPinSimple.h"
#include "MFRC522Debug.h"
#include "WString.h"
#include "esp32-hal.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "portmacro.h"
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <sys/_types.h>
#include <sys/types.h>
#include "notifications.h"
#include "notifications.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "string.h"
#include "ArduinoJson-v7.4.3.h"

// SPI Pins
#define SS_PIN      10
#define SCK_PIN     13
#define MISO_PIN    12
#define MOSI_PIN    11

// Pins for Notifications
#define BUZZER_PIN  37
#define LED_PIN     48
RGB_LED rgb_led{19,20,21};

// WiFi Information
#define SSID    "hassanminar1003"
#define PASSWD  "eliasadil1708"

// Networking Information
String SERVER_URL = "http://192.168.68.60:5173";

// Networking
NetworkClient client;
HTTPClient http_client;

QueueHandle_t network_queue;

typedef enum{
    ACTIVE,
    LATE,
    REGISTERING,
    IDLE
}session_state_t;

typedef enum{
    STATUS_REQUEST,
    SCAN_REQUEST
}request_type_t;

typedef struct{
    request_type_t request_type;
    String uid; 
}request_t;

void scan_request(String uid){
    if(Network.isOnline()){

        // Configure http client
        http_client.setConnectTimeout(2000);
        http_client.addHeader("content-type", "application/json");
        http_client.begin(client,SERVER_URL+"/scan");

        // Initialize Buffers
        JsonDocument request_json,response_json;
        String request_string,response_string;
        
        request_json["uid"] = uid;
        serializeJson(request_json,request_string);
        int status = http_client.POST(request_string);
        Serial.println(http_client.getString());

        switch (status){
            case t_http_codes::HTTP_CODE_OK:
                xTaskNotify(notification_task_handle, NOTIFY_SUCCESS, eSetBits);
                break;

            case t_http_codes::HTTP_CODE_CREATED:
                xTaskNotify(notification_task_handle, NOTIFY_REGISTERED, eSetBits);
                break;

            case t_http_codes::HTTP_CODE_CONFLICT:
                xTaskNotify(notification_task_handle, NOTIFY_DUPLICATE, eSetBits);
                break;
            
            case t_http_codes::HTTP_CODE_REQUEST_TIMEOUT:
                xTaskNotify(notification_task_handle, NOTIFY_LATE, eSetBits);
                break;

            case t_http_codes::HTTP_CODE_NOT_FOUND:
                xTaskNotify(notification_task_handle, NOTIFY_FAIL, eSetBits);
                break;
            
            default:
            Serial.println(http_client.errorToString(status));
            xTaskNotify(notification_task_handle, NOTIFY_DISCONNECTED, eSetBits);
            break;
        }
        http_client.end();
    }
    else{
        xTaskNotify(notification_task_handle, NOTIFY_DISCONNECTED, eSetBits);
    }
}

void status_request(){
    if(Network.isOnline()){
        http_client.setConnectTimeout(2000);
    http_client.addHeader("content-type", "application/json");
    http_client.begin(client,SERVER_URL+"/status");

    // Buffers
    JsonDocument response_json;
    String response_string;
    uint8_t session_status_code;

    // Request current status
    int status = http_client.GET();
    switch (status) {
    case t_http_codes::HTTP_CODE_OK:
        response_string = http_client.getString();
        deserializeJson(response_json,response_string);
        session_status_code =  response_json["session_status_code"].as<int>();

        switch (session_status_code){
            case session_state_t::ACTIVE:
                xTaskNotify(notification_task_handle, SET_STATUS_ACTIVE, eSetBits);
                break;
            case session_state_t::IDLE:
                xTaskNotify(notification_task_handle, SET_STATUS_IDLE, eSetBits);
                break;
            case session_state_t::LATE:
                xTaskNotify(notification_task_handle, SET_STATUS_LATE, eSetBits);
                break;
            case session_state_t::REGISTERING:
                xTaskNotify(notification_task_handle, SET_STATUS_REGISTERING, eSetBits);
                break; 
        }
        break;
    default:
        xTaskNotify(notification_task_handle, SET_STATUS_DISCONNECTED, eSetBits);
        break;
    }
    http_client.end();
    }
    else {
        xTaskNotify(notification_task_handle, SET_STATUS_DISCONNECTED, eSetBits);
    }
    
}

TaskHandle_t status_polling_task_handle = NULL;
void status_polling_task(void* pvParameters){
    for(;;){
        xQueueSend(network_queue, new request_t({.request_type = STATUS_REQUEST,.uid=""}), 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

TaskHandle_t network_task_handle = NULL;
void network_task(void* pvParameters){
    request_t request;
    for(;;){
        xQueueReceive(network_queue,&request,portMAX_DELAY);
        switch (request.request_type) {
            case STATUS_REQUEST:
                status_request();
                break;
            case SCAN_REQUEST:
                scan_request(request.uid);
                break;
        }
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
                    xQueueSend(network_queue, new request_t({.request_type = SCAN_REQUEST,.uid=uid_buffer}), 0);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
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

    // Create network queue
    network_queue = xQueueCreate(1,sizeof(request_t));

    // Initialize Notifications
    if(!Notification.begin(BUZZER_PIN, LED_PIN, rgb_led)){
        Serial.println("Failed to initialize notifications");
    };

    // Networking Setup
    Network.begin();
    WiFi.setSleep(false);
    WiFi.STA.begin();
    WiFi.STA.connect(SSID,PASSWD);

    // Begin Tasks
    // Reader Task
    xTaskCreate(reader_loop,
                "reader_loop",
                4096,
                NULL,
                3,
                &reader_loop_handle);
    
    // Network Task
    xTaskCreate(network_task, 
                "network_task", 
                4096,
                NULL,
                2,
                &network_task_handle);
    
    // Status Polling Task    
    xTaskCreate(status_polling_task, 
                "status_polling_task", 
                4096,
                NULL,
                2,
                &status_polling_task_handle);
}