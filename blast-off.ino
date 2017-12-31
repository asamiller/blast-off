#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2

struct RGBW {
  byte r;
  byte g;
  byte b;
  byte w;
};

RGBW colors[] = {
  { 255, 150, 0, 255},    // yellow + white
  { 255, 150, 0, 0},      // yellow
  { 255, 90, 0, 0},       // orange
  { 255, 30, 0, 0},       // orangie-red
  { 255, 0, 0, 0},        // red
  { 155, 0, 0, 0}         // dark red
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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(61, PIN, NEO_RGBW + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

float scale = 1;
float inc = 0.1;
float dir = 1.0;

void loop() {
  uint16_t i;
  uint16_t numPixels = strip.numPixels();

  // how many leds for each color
  int ledsPerColor = ceil(numPixels / (NUMBER_OF_COLORS-1));

  // the scale animation direction
  if (scale <= 0.5 || scale >= 1) {
    dir = dir * -1;
  }

  // add a random amount to inc
  inc = ((float)random(0, 100)/1000);

  // add the increment amount to the scale
  scale += (inc * dir);

  // constrain the scale
  scale = constrain(scale, 0.5, 1);

  for(i=0; i<numPixels; i++) {
    float val = ((float)i * scale) / (float)ledsPerColor;
    int currentIndex = floor(val);
    int nextIndex = ceil(val);

    float flux = (float)random(80, 100) / 100;
    float transition = fmod(val, 1);

    RGBW currentColor = colors[currentIndex];
    RGBW nextColor = colors[nextIndex];

    int r = percentBetween(currentColor.r, nextColor.r, transition) * flux;
    int g = percentBetween(currentColor.g, nextColor.g, transition) * flux;
    int b = percentBetween(currentColor.b, nextColor.b, transition) * flux;
    int w = percentBetween(currentColor.w, nextColor.w, transition) * flux;

    strip.setPixelColor(i, g, r, b, w);
  }

  strip.show();
//  delay(10);
}
