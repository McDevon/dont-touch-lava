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

void setup()
{
  delay(1000);
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);

  initialize_us_sensor(US_PING_PIN, US_ECHO_PIN);
}

GameState game_state = CREATE_GAME_STATE(leds, NUM_LEDS);
int delay_accumulated = INT16_MAX;
long duration = 0;
const int ultrasonic_min_interval = 50;

void loop()
{
  if (delay_accumulated >= ultrasonic_min_interval) {
    delay_accumulated = 0;
    duration = activate_us_sensor(US_PING_PIN, US_ECHO_PIN);
  } 

  int delay_time = step_game(&game_state, duration);
  delay_accumulated += delay_time;

  FastLED.show();

  delay(delay_time);
}
