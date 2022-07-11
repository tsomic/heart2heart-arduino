#include <FastLED.h>

#define LED_PIN 4
#define NUM_LEDS 64
#define BRIGHTNESS 64
#define UPDATES_PER_SECOND 100
#define FADE_DURATION 128

struct PaletteConfig {
  CRGBPalette32 palette;
  uint8_t loopDelay;
  float speed;
  bool repeat;
};

PaletteConfig paletteConfigs[5];
int16_t stopTimer;
bool isRunning;
uint8_t delayTimer;
uint8_t currentPaletteConfig;
CRGB leds[NUM_LEDS];
uint8_t startIndex;

class LEDController {
  public:
    LEDController();

    void init(uint8_t defaultPaletteConfig) {
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
          FastLED.setBrightness(min(stopTimer, (int16_t)BRIGHTNESS));
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

            if (!paletteConfigs[currentPaletteConfig].repeat) {
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
      FastLED.setBrightness(BRIGHTNESS);
    }

    void stop() {
      stopTimer = FADE_DURATION;
    }

    void changePaletteConfig(uint8_t configIndex) {
      currentPaletteConfig = configIndex;
    }

    void reset() {
      isRunning = false;
      stopTimer = -1;
      startIndex = 0;
      delayTimer = 0;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    }

  private:
    void fillLEDsFromPaletteColors( uint8_t colorIndex) {
      const uint8_t QUARTER = NUM_LEDS / 4;
      for ( int i = 0; i < QUARTER; i++) {
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
      setupMode1ChangePalette();
      setupMode2ChangePalette();
    }

    void setupBeatPalette() {
      const uint8_t LENGTH_PULSE_SHORT = 12;
      const uint8_t LENGTH_PULSE_LONG = 22;
      const uint8_t PULSE_OFFSET = 6;
      const uint8_t START_OFFSET = 4;
      CRGB gradient[32];
      fill_solid( gradient, 32, CRGB::Black);

      for (uint8_t i = 0; i < PULSE_OFFSET; i++) {
        gradient[i + START_OFFSET] = CHSV(0, 255, (LENGTH_PULSE_SHORT - i) * 255 / LENGTH_PULSE_SHORT);
      }

      for (uint8_t i = 0; i < LENGTH_PULSE_LONG; i++) {
        gradient[i + PULSE_OFFSET + START_OFFSET] = CHSV(0, 255, (LENGTH_PULSE_LONG - i) * 255 / LENGTH_PULSE_LONG);
      }

      paletteConfigs[0] = {CRGBPalette32(gradient), 0, 1, true};
    }

    void setupBreathePalette() {
      CRGB gradient[32];
      const uint8_t MAX_BRIGHTNESS = 70;
      const uint8_t DURATION = 32;

      fill_solid( gradient, 32, CRGB::Black);

      for (uint8_t i = 0; i < DURATION; i++) {
        uint8_t value = (cos(2 * PI / DURATION * i - PI) + 1) / 2 * MAX_BRIGHTNESS;
        if (value <= 10) {
          value = 0;
        }
        gradient[i] = CHSV(0, 255, value);
      }


      paletteConfigs[1] = {CRGBPalette32(gradient), 120, 0.4, true};
    }

    void setupClickPalette() {
      const uint8_t PADDING = 4;
      CRGB gradient[32];

      fill_solid( gradient, 32, CRGB::Black);

      for (uint8_t i = 0; i < 2; i++) {
        gradient[i + PADDING] = CHSV(0, 255, 100);
      }

      paletteConfigs[2] = {CRGBPalette32(gradient), 0, 1, false};
    }

    void setupMode1ChangePalette() {
      paletteConfigs[3] = {paletteConfigs[0].palette, 0, 3, false};
    }
    
    void setupMode2ChangePalette() {
      paletteConfigs[4] = {paletteConfigs[1].palette, 0, 3, false};
    }
};


extern LEDController LEDController;
