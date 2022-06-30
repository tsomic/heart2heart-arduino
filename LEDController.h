#include <FastLED.h>

#define LED_PIN     4
#define NUM_LEDS    64
#define BRIGHTNESS  64
#define UPDATES_PER_SECOND 100

struct PaletteConfig{
   CRGBPalette16 palette;
   int loopDelay;
   float speed;
   int loopCount;
};

const int FADE_DURATION = 255;

CRGB leds[NUM_LEDS];

uint8_t startIndex = 0;
int stopTimer = -1;
bool isRunning = false;

PaletteConfig paletteConfigs[3];
int currentPaletteConfig;

class LEDController {
public:
  LEDController();
  
  void init(int defaultPaletteConfig) {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    reset();
    
    setupPalettes();
    currentPaletteConfig = defaultPaletteConfig;
  }

  void tick() {
    if (isRunning) {
      if (stopTimer > 0) {
        stopTimer--;
        FastLED.setBrightness(stopTimer);
      } else if (stopTimer == 0) {
        stopTimer--;
        reset();
      }
      Serial.println(startIndex);
      
      startIndex++;
      fillLEDsFromPaletteColors(startIndex);
    
      FastLED.show();
      FastLED.delay(1000 / UPDATES_PER_SECOND / paletteConfigs[currentPaletteConfig].speed);
    }
  }

  void start() {
    isRunning = true;
    stopTimer = -1;
    FastLED.setBrightness(BRIGHTNESS);
  }

  void stop() {
    stopTimer = FADE_DURATION;
  }

  void changePaletteConfig(int configIndex) {
    currentPaletteConfig = configIndex;
  }

private:
  void reset() {
    isRunning = false;
    stopTimer = -1;
    startIndex = 0;
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }
  
  void fillLEDsFromPaletteColors( uint8_t colorIndex) {
    const int QUARTER = NUM_LEDS / 4;
    for( int i = 0; i < QUARTER; i++) {
      leds[i + QUARTER] = ColorFromPalette( paletteConfigs[currentPaletteConfig].palette, colorIndex);
      leds[NUM_LEDS / 2 - i - 1 - QUARTER] = ColorFromPalette( paletteConfigs[currentPaletteConfig].palette, colorIndex);
      leds[NUM_LEDS - i - 1 - QUARTER] = ColorFromPalette( paletteConfigs[currentPaletteConfig].palette, colorIndex);
      leds[NUM_LEDS / 2 + i + QUARTER] = ColorFromPalette( paletteConfigs[currentPaletteConfig].palette, colorIndex);
      colorIndex++;
    }
  }

  void setupPalettes() {
    setupBeatPalette();
    setupBreathePalette();
  }

  void setupBeatPalette() {
    const int LENGTH_PULSE_SHORT = 6;
    const int LENGTH_PULSE_LONG = 11;
    const int PULSE_OFFSET = 3;
    CRGB gradient[16];
    fill_solid( gradient, 16, CRGB::Black);
  
    for (int i = 0; i < PULSE_OFFSET; i++) {
      gradient[i + 2] = CHSV(0, 255, (LENGTH_PULSE_SHORT - i) * 255/LENGTH_PULSE_SHORT);
    }
  
    for (int i = 0; i < LENGTH_PULSE_LONG; i++) {
      gradient[i + PULSE_OFFSET + 2] = CHSV(0, 255, (LENGTH_PULSE_LONG - i) * 255/LENGTH_PULSE_LONG);
    }
  
    paletteConfigs[1] = {CRGBPalette16(gradient), 0, 1, -1};
  }

  void setupBreathePalette() {
    CRGB gradient[16];
    const int MAX_BRIGHTNESS = 70;
    const int FADE_DURATION = 16;

    fill_solid( gradient, 16, CRGB::Black);

    for (int i = 0; i < FADE_DURATION; i++) {
      int value = (cos((2 * PI / FADE_DURATION) * i - PI) + 1) / 2 * MAX_BRIGHTNESS;
      if (value <= 3) {
        //value = 0;
      }
      gradient[i] = CHSV(0, 255, value);
    }

    paletteConfigs[2] = {CRGBPalette16(gradient), 255, 0.5, -1};
  }
};


extern LEDController LEDController;
