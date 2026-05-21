idf_component_register(SRCS "RGB_LED.cpp" "Notification.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES arduino-esp32 Adafruit_NeoPixel freertos)