#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2
#define PIXEL_COUNT 61
#define SCALE_AMOUNT 0.5
#define FLICKER_SPEED 5
#define FLICKER_AMOUNT 50

struct RGBW {
  byte r;
  byte g;
  byte b;
  byte w;
};

RGBW colors[] = {
  { 255, 150, 0, 255},    // yellow + white
  { 255, 120, 0, 0},      // yellow + white
  { 255, 90, 0, 0},       // orange
  { 255, 30, 0, 0},       // orangie-red
  { 255, 0, 0, 0},        // red
  { 255, 0, 0, 0}         // extra red
};

int NUMBER_OF_COLORS = sizeof(colors) / sizeof(RGBW);

int percentBetween(int a, int b, float percent) {
  return (int)(((b - a) * percent) + a);
}

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIN, NEO_RGBW + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

float scale = 1;
float inc = 0.1;
float dir = 1.0;
const int arrayLength = round(PIXEL_COUNT / 3);
byte pixelBrightness[arrayLength];
boolean pixelDirection[arrayLength];

// prepopulate the pixel array
void seedArray() {
  for (uint16_t i=0; i < PIXEL_COUNT; i++) {
    uint16_t p = i % arrayLength;
    pixelBrightness[p] = random(255-FLICKER_AMOUNT, 255);
    pixelDirection[p] = !!random(0, 1);
  }
}

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  seedArray();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // how many leds for each color
  int ledsPerColor = ceil(PIXEL_COUNT / (NUMBER_OF_COLORS-1));

  // the scale animation direction
  if (scale <= 0.5 || scale >= 1) {
    dir = dir * -1;
  }

  // add a random amount to inc
  inc = ((float)random(0, 50)/1000);

  // add the increment amount to the scale
  scale += (inc * dir);

  // constrain the scale
  scale = constrain(scale, 0.5, 1);

  for (uint16_t i=0; i < PIXEL_COUNT; i++) {
    uint16_t p = i % arrayLength;

    float val = ((float)i * scale) / (float)ledsPerColor;
    int currentIndex = floor(val);
    int nextIndex = ceil(val);
    float transition = fmod(val, 1);

    // color variations
    if (pixelDirection[p]) {
      pixelBrightness[p] += FLICKER_SPEED;

      if (pixelBrightness[p] >= 255) {
        pixelBrightness[p] = 255;
        pixelDirection[p] = false;
      }
    } else {
      pixelBrightness[p] -= FLICKER_SPEED;

      if (pixelBrightness[p] <= 255-FLICKER_AMOUNT) {
        pixelBrightness[p] = 255-FLICKER_AMOUNT;
        pixelDirection[p] = true;
      }
    }

    RGBW currentColor = colors[currentIndex];
    RGBW nextColor = colors[nextIndex];
    float flux = (float)pixelBrightness[p] / 255;

    byte r = percentBetween(currentColor.r, nextColor.r, transition) * flux;
    byte g = percentBetween(currentColor.g, nextColor.g, transition) * flux;
    byte b = percentBetween(currentColor.b, nextColor.b, transition) * flux;
    byte w = percentBetween(currentColor.w, nextColor.w, transition) * flux;

    strip.setPixelColor(i, g, r, b, w);
  }

  strip.show();
}
