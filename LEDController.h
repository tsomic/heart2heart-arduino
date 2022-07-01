#include <FastLED.h>

#define LED_PIN     4
#define NUM_LEDS    64
#define BRIGHTNESS  64
#define UPDATES_PER_SECOND 100

struct PaletteConfig{
   CRGBPalette32 palette;
   int loopDelay;
   float speed;
   int loopCount;
};

const int FADE_DURATION = 255;

CRGB leds[NUM_LEDS];

uint8_t startIndex = 0;
int stopTimer = -1;
bool isRunning = false;
int delayTimer = 0;
int loopCounter = 0;

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

      if (startIndex == 255) {
        if (delayTimer == 0) {
          delayTimer = paletteConfigs[currentPaletteConfig].loopDelay;
        } else {
          delayTimer--;
          FastLED.delay(1000 / UPDATES_PER_SECOND / paletteConfigs[currentPaletteConfig].speed);
        }
        if (delayTimer == 0) {
          startIndex++;
          loopCounter++;

          if (loopCounter == paletteConfigs[currentPaletteConfig].loopCount) {
            reset();
          }
        }
      }
      
      if (startIndex != 255 && isRunning) {
        startIndex++;
        fillLEDsFromPaletteColors(startIndex);
      
        FastLED.show();
        FastLED.delay(1000 / UPDATES_PER_SECOND / paletteConfigs[currentPaletteConfig].speed);
      }
    }
  }

  void start() {
    isRunning = true;
    stopTimer = -1;
    loopCounter = 0;
    FastLED.setBrightness(BRIGHTNESS);
  }

  void stop() {
    stopTimer = FADE_DURATION;
  }

  void changePaletteConfig(int configIndex) {
    currentPaletteConfig = configIndex;
  }

  void reset() {
    isRunning = false;
    stopTimer = -1;
    startIndex = 0;
    delayTimer = 0;
    loopCounter = 0;
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  }

private:
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
    setupClickPalette();
    setupBeatPalette();
    setupBreathePalette();
  }

  void setupBeatPalette() {
    const int LENGTH_PULSE_SHORT = 12;
    const int LENGTH_PULSE_LONG = 22;
    const int PULSE_OFFSET = 6;
    const int START_OFFSET = 4;
    CRGB gradient[32];
    fill_solid( gradient, 32, CRGB::Black);
  
    for (int i = 0; i < PULSE_OFFSET; i++) {
      gradient[i + START_OFFSET] = CHSV(0, 255, (LENGTH_PULSE_SHORT - i) * 255/LENGTH_PULSE_SHORT);
    }
  
    for (int i = 0; i < LENGTH_PULSE_LONG; i++) {
      gradient[i + PULSE_OFFSET + START_OFFSET] = CHSV(0, 255, (LENGTH_PULSE_LONG - i) * 255/LENGTH_PULSE_LONG);
    }
  
    paletteConfigs[1] = {CRGBPalette32(gradient), 0, 1, -1};
  }

  void setupBreathePalette() {
    CRGB gradient[32];
    const int MAX_BRIGHTNESS = 70;
    const int FADE_DURATION = 32;

    fill_solid( gradient, 32, CRGB::Black);
    
    for (int i = 0; i < FADE_DURATION; i++) {
      int value = (cos((2 * PI / FADE_DURATION) * i - PI) + 1) / 2 * MAX_BRIGHTNESS;
      if (value <= 3) {
        value = 0;
      }
      gradient[i] = CHSV(0, 255, value);
    }
    

    paletteConfigs[2] = {CRGBPalette32(gradient), 120, 0.5, -1};
  }

  void setupClickPalette() {
    const int PADDING = 4;
    CRGB gradient[32];

    fill_solid( gradient, 32, CRGB::Black);

    for (int i = 0; i < 3; i++) {
      gradient[i + PADDING] = CHSV(0, 255, 100);
    }
    
    paletteConfigs[0] = {CRGBPalette32(gradient), 0, 1, 1};
  }
};


extern LEDController LEDController;
