#pragma once

typedef struct{
    int r;
    int g;
    int b;
}rgb_colour_t;

struct RGB_LED{
    int r;
    int g;
    int b;
    bool begin();
};
