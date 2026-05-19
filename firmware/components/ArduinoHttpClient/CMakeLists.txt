idf_component_register(SRCS "src/b64.cpp" "src/HttpClient.cpp" "src/URLEncoder.cpp" "src/WebSocketClient.cpp" "src/utility/URLParser/http_parser.c"
                        INCLUDE_DIRS "src" "src/utility/URLParser"
                        REQUIRES arduino-esp32)