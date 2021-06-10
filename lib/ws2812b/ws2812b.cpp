// NeoPixelFunRandomChange
// This example will randomly select a number pixels and then
// start an animation to blend them from their current color to
// randomly selected a color
// 
#include "ws2812b.h"
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

uint8_t SetR=255,SetG=24,SetB=180;

uint8_t running_flag = 1;
uint8_t backgroun_brightness = 255;  //0-16

RgbColor Background(255,0,255);
RgbColor Black(0,0,0);

NeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod> strip(PixelCount, PixelPin);
// For Esp8266, the Pin is omitted and it uses GPIO3 due to DMA hardware use.  
// There are other Esp8266 alternative methods that provide more pin options, but also have
// other side effects.
// for details see wiki linked here https://github.com/Makuna/NeoPixelBus/wiki/ESP8266-NeoMethods 
                  
 NeoPixelAnimator animations(PixelCount); // NeoPixel animation management object

// one entry per pixel to match the animation timing manager
MyAnimationState animationState[PixelCount];

void SetBackground(uint8_t r,uint8_t g, uint8_t b){
    Background.B = b;
    Background.R = r;
    Background.G = g;
}
void SetBackgroundBrightness(uint8_t brightness){
    Background =  Background.Brighten(brightness);
}

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

// simple blend function
void BlendAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);
    // apply the color to the strip
    strip.SetPixelColor(param.index, updatedColor);
}

void PickRandom(float luminance)
{
    // pick random count of pixels to animate
    uint16_t count = random(PixelCount);
    while (count > 0)
    {
        // pick a random pixel
        uint16_t pixel = random(PixelCount);

        // pick random time and random color
        // we use HslColor object as it allows us to easily pick a color
        // with the same saturation and luminance 
        uint16_t time = random(100, 400);
        animationState[pixel].StartingColor = strip.GetPixelColor(pixel);
        animationState[pixel].EndingColor = HslColor(random(360) / 360.0f, 1.0f, luminance);

        animations.StartAnimation(pixel, time, BlendAnimUpdate);

        count--;
    }
}


void showrandom()
{
    
    if (animations.IsAnimating())
    {
        // the normal loop just needs these two to run the active animations
        animations.UpdateAnimations();
        strip.Show();
    }
    else
    {
        // no animations runnning, start some 
        //
        PickRandom(0.2f); // 0.0 = black, 0.25 is normal, 0.5 is bright
    }
}
void led_init()
{
    strip.Begin();
    strip.SetPixelColor(0,Black);
    strip.SetPixelColor(1,Black);
    strip.SetPixelColor(2,Black);
    strip.SetPixelColor(3,Black);
    strip.Show();
    SetBackground(SetR,SetG,SetB);
    SetBackgroundBrightness(backgroun_brightness);
    SetRandomSeed();
}


void show_background(){
    uint8_t flag = running_flag&0x4;
    if(flag==0){
        strip.SetPixelColor(0,Black);
        strip.SetPixelColor(1,Black);
        strip.SetPixelColor(2,Black);
        strip.SetPixelColor(3,Black);
        strip.Show();
    }else if(running_flag&0x1){
          showrandom();
    }else{
        strip.SetPixelColor(0,Background);
        strip.SetPixelColor(1,Background);
        strip.SetPixelColor(2,Background);
        strip.SetPixelColor(3,Background);
        strip.Show();
    }
}