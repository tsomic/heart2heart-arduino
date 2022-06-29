#include <FastLED.h>

#define LED_PIN     4
#define NUM_LEDS    64
#define BRIGHTNESS  64
#define UPDATES_PER_SECOND 100

const int FADE_DURATION = 255;

CRGB leds[NUM_LEDS];

uint8_t startIndex = 0;
int stopTimer = -1;
bool isRunning = false;
CRGBPalette16 currentPalette;
CRGBPalette16 beatPalette;

class LEDController {
public:
  LEDController();
  
  void init() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    reset();
    
    setupBeatPalette();
    currentPalette = beatPalette;
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
  
      //if (stopTimer > 1) {
        //stopTimer--;
        //FastLED.setBrightness(stopTimer);
      //}
      
      startIndex++;
      fillLEDsFromPaletteColors(startIndex);
    
      FastLED.show();
      FastLED.delay(1000 / UPDATES_PER_SECOND);

      //if (stopTimer == 1) {
        //reset();
      //}
    }
  }

  void start() {
    isRunning = true;
    FastLED.setBrightness(BRIGHTNESS);
  }

  void stop() {
    stopTimer = FADE_DURATION;
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
      leds[i + QUARTER] = ColorFromPalette( currentPalette, colorIndex);
      leds[NUM_LEDS / 2 - i - 1 - QUARTER] = ColorFromPalette( currentPalette, colorIndex);
      leds[NUM_LEDS - i - 1 - QUARTER] = ColorFromPalette( currentPalette, colorIndex);
      leds[NUM_LEDS / 2 + i + QUARTER] = ColorFromPalette( currentPalette, colorIndex);
      colorIndex++;
    }
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
  
    beatPalette = CRGBPalette16(gradient[0],  gradient[1],  gradient[2],  gradient[3],
                                gradient[4],  gradient[5],  gradient[6],  gradient[7],
                                gradient[8],  gradient[9],  gradient[10],  gradient[11],
                                gradient[12],  gradient[13],  gradient[14],  gradient[15]);
  }
};


extern LEDController LEDController;
