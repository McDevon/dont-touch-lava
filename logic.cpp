#include "FastLED.h"
#include "game_main.h"
#include "us_sensor.h"

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 32
#define DEFAULT_BRIGHTNESS 50

#define LED_DATA_PIN 9
#define US_PING_PIN 3
#define US_ECHO_PIN 2

CRGB leds[NUM_LEDS];

void setup() {
  delay(1000);
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<LED_TYPE,LED_DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);

  initialize_us_sensor(US_PING_PIN, US_ECHO_PIN);
}

GameState game_state = CREATE_GAME_STATE(leds, NUM_LEDS);

void loop() {
   long duration = activate_us_sensor(US_PING_PIN, US_ECHO_PIN);
   long cm = us_microseconds_to_cm(duration);
   Serial.print(cm);
   Serial.print("cm, ");
   Serial.print(duration);
   Serial.print(" us");
   Serial.println();

   int delay_time = step_game(&game_state, duration);
   FastLED.show();
   
   delay(delay_time);
}
