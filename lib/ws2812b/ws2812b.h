#ifndef _WS2812B_H_
#define _WS2812B_H_

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

const uint16_t PixelCount = 4; // make sure to set this to the number of pixels in your strip
const uint8_t PixelPin = 3;  // make sure to set this to the correct pin, ignored for Esp8266

extern uint8_t SetR,SetG,SetB;
extern uint8_t running_flag; //0-1
extern uint8_t backgroun_brightness;  //0-255
// what is stored for state is specific to the need, in this case, the colors.
// Basically what ever you need inside the animation update function
struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

void SetBackground(uint8_t r,uint8_t g, uint8_t b);
void SetBackgroundBrightness(uint8_t brightness);

void show_background();
void led_init();

#endif

