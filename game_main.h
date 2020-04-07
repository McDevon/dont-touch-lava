#ifndef game_main_h
#define game_main_h

#include "FastLED.h"

struct GameState {
  CRGB *leds;
  long current_step;
  int current_substep;
  int animation_step;
  int phase_step;
  int step_duration;
  int area_height;
  int area_top;
  int animation;
  int lava_burst_position;
  int lava_burst_step;
  int last_duration;
  int player_position;
  int led_count;
};

#define CREATE_GAME_STATE(leds, led_count) { leds, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, led_count }

int step_game(GameState *state, long sensor_duration);

#endif
