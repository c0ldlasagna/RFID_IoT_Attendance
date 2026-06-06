#include "RGB_LED.h"
#include "esp32-hal-ledc.h"
#include "HardwareSerial.h"

bool RGB_LED::begin(){
    if(!ledcAttach(r, 10000, 8)){
        Serial.println("Failed to attach pin R");
        return false;
    }
    if(!ledcAttach(g, 10000, 8)){
        Serial.println("Failed to attach pin G");
        return false;        
    }
    if(!ledcAttach(b, 10000, 8)){
        Serial.println("Failed to attach pin B");
        return false;        
    }
    return true;
}

