#include "game_main.h"
#include "us_sensor.h"
#include "colors.h"

#define min_us_distance 50
#define max_us_distance 200
#define max_us_move_per_step 3
#define us_outlier_threshold 30

const long min_distance = us_cm_to_microseconds(min_us_distance);
const long max_distance = us_cm_to_microseconds(max_us_distance);

const long start_min_distance = us_cm_to_microseconds(min_us_distance + (max_us_distance - min_us_distance) / 3);
const long start_max_distance = us_cm_to_microseconds(min_us_distance + (max_us_distance - min_us_distance) / 3 * 2);

const long max_move_speed = us_cm_to_microseconds(max_us_move_per_step);
const long move_outlier_threshold = us_cm_to_microseconds(us_outlier_threshold);

#define LAVA_BURST_WARNING_START 3
#define LAVA_BURST_LAVA_STEP_LENGTH 3
#define LAVA_BURST_LAVA_START 9
#define LAVA_BURST_LAVA_END 19

void initialize_game(GameState *state)
{
  randomSeed(analogRead(0));

  state->current_step = 0;
  state->current_substep = 0;
  state->animation = 1;
  state->animation_step = 0;
  state->phase_step = 50 + random(100);
  state->last_duration = us_cm_to_microseconds((min_us_distance + max_us_distance) / 2);
  state->step_duration = 10;
  state->player_position = state->led_count / 2;
  state->area_height = state->led_count - 10;
  state->area_top = 5;
  state->lava_burst_position = -1;
  state->lava_burst_step = -200 - random(100);
}

CHSV lava_color(GameState *state, int index)
{
  return lava_colors[(state->color_offset + index) % lava_colors_count];
}

CHSV warning_color(GameState *state, int index)
{
  return warning_colors[(state->color_offset + index) % warning_colors_count];
}

void draw_state(GameState *state)
{
  int warningRadius = 0;
  int outerRadius = 0;
  int innerRadius = -1;
  if (state->lava_burst_position > LAVA_BURST_WARNING_START) {
    if (state->lava_burst_step > LAVA_BURST_LAVA_START && state->lava_burst_step <= LAVA_BURST_LAVA_END) {
      outerRadius = (1 + state->lava_burst_step - LAVA_BURST_LAVA_START) / LAVA_BURST_LAVA_STEP_LENGTH;
      warningRadius = (1 + state->lava_burst_step - LAVA_BURST_WARNING_START) / LAVA_BURST_LAVA_STEP_LENGTH;
    } else if (state->lava_burst_step > LAVA_BURST_LAVA_END) {
      outerRadius = (1 + LAVA_BURST_LAVA_END - LAVA_BURST_LAVA_START) / LAVA_BURST_LAVA_STEP_LENGTH;
      innerRadius = (1 + LAVA_BURST_LAVA_END - state->lava_burst_step) / LAVA_BURST_LAVA_STEP_LENGTH;
      warningRadius = (1 + LAVA_BURST_LAVA_END - LAVA_BURST_WARNING_START) / LAVA_BURST_LAVA_STEP_LENGTH;
    }
  }

  for (int i = 0; i < state->led_count; ++i) {
    int distance = -1;
    if (state->lava_burst_position > LAVA_BURST_WARNING_START) {
      distance = abs(i - state->lava_burst_position);
    }
    if (i == state->player_position) {
      state->leds[i] = CRGB::Blue;
    } else if (i < state->area_top) {
      state->leds[i] = lava_color(state, i);
    } else if (i > state->area_top + state->area_height) {
      state->leds[i] = lava_color(state, i);
    } else if (distance < outerRadius && distance > innerRadius) {
      state->leds[i] = lava_color(state, i);
    } else if (distance < warningRadius) {
      state->leds[i] = warning_color(state, i);
    } else {
      state->leds[i] = CRGB::Black;
    }
  }
}

void set_player_position(GameState *state, long distance)
{
  state->player_position = (distance - min_distance) * state->led_count / (max_distance - min_distance);
}

void wait_for_player_to_be_ready(GameState *state, long distance)
{
  set_player_position(state, distance);
  
  if (distance > start_min_distance && distance < start_max_distance) {
    state->current_step++;
    if (state->current_step > 150) {
      state->current_step = 0;
      state->animation = 0;
    }
  } else {
    state->current_step = 0;
  }
}

void move_area(GameState *state)
{
  state->current_step++;
  state->animation_step++;

  const long speedup_interval = 100;
  const long narrow_interval = 200;

  if (state->current_step % speedup_interval == 0 && state->step_duration > 4) {
    state->step_duration -= 2;
  }
  if (state->current_step % narrow_interval == 0 && state->area_height > 5) {
    state->area_height -= 1;
  }

  if (state->phase_step >= 0) {
    const int change = random(3);
    if (change == 0 && state->area_top > 2) {
      state->area_top -= 1;
    } else if (change == 1 && state->area_top < state->led_count - state->area_height - 2) {
      state->area_top += 1;
    }
    state->phase_step -= 1;
  } else {
    if (state->phase_step == -1 || state->phase_step == -3) {
      if (state->area_top > 2) {
        state->area_top -= 1;
      } else {
        state->phase_step--;
      }
    } else if (state->phase_step == -2) {
      if (state->area_top < state->led_count - state->area_height - 2) {
        state->area_top += 1;
      } else {
        state->phase_step--;
      }
    } else {
      state->phase_step = 100 + random(100);
    }
  }

  state->lava_burst_step++;
  if (state->lava_burst_step >= 0) {
    if (state->lava_burst_step == 0) {
      state->lava_burst_position = state->player_position;
    } else if (state->lava_burst_step > LAVA_BURST_LAVA_END + (LAVA_BURST_LAVA_END - LAVA_BURST_LAVA_START)) {
      state->lava_burst_step = -50 - random(150);
      state->lava_burst_position = -1;
    }
  }
}

void lose_game(GameState *state)
{
  state->animation = 2;
  state->current_substep = 0;
  state->animation_step = 0;
}

void test_area(GameState *state)
{
  if (state->player_position < state->area_top
      || state->player_position > state->area_top + state->area_height) {
    lose_game(state);
  }

  if (state->lava_burst_step > LAVA_BURST_LAVA_START && state->lava_burst_step <= LAVA_BURST_LAVA_END) {
    const int radius = (1 + state->lava_burst_step - LAVA_BURST_LAVA_START) / LAVA_BURST_LAVA_STEP_LENGTH;
    if (abs(state->player_position - state->lava_burst_position) < radius) {
      lose_game(state);
    }
  } else if (state->lava_burst_step > LAVA_BURST_LAVA_END) {
    const int outerRadius = (1 + LAVA_BURST_LAVA_END - LAVA_BURST_LAVA_START) / LAVA_BURST_LAVA_STEP_LENGTH;
    const int innerRadius = (1 + LAVA_BURST_LAVA_END - state->lava_burst_step) / LAVA_BURST_LAVA_STEP_LENGTH;
    const int distance = abs(state->player_position - state->lava_burst_position);
    if (distance < outerRadius && distance > innerRadius) {
      lose_game(state);
    }
  }
}

void play_game(GameState *state, long distance)
{
  set_player_position(state, distance);

  state->current_substep++;
  if (state->current_substep > state->step_duration) {
    state->current_substep = 0;
    move_area(state);
  }

  test_area(state);
}

void ending_animation(GameState *state, long distance)
{
  state->current_substep++;
  if (state->current_substep > 20) {
    state->current_substep = 0;
    state->animation_step++;
  }
  if (state->current_substep > 10) {
    for (int i = 0; i < state->led_count; ++i) {
      state->leds[i] = CRGB::Red;
    }
  } else {
    draw_state(state);
  }

  if (state->animation_step > 5) {
    initialize_game(state);
  }
}

inline long sign(long value) {
  return value < 0 ? -1 : 1;
}

int step_game(GameState *state, long sensor_duration)
{
  if (state->player_position < 0) {
    initialize_game(state);
  }

  state->color_offset += 1;
  if (state->color_offset >= lava_colors_count) {
    state->color_offset = 0;
  }

  long distance = (sensor_duration > min_distance && sensor_duration < max_distance) ? sensor_duration : state->last_duration;
  const long change = distance - state->last_duration;

  if (abs(change) > max_move_speed) {
    distance = sign(change) * max_move_speed + state->last_duration;
  }

  if (state->animation == 1) {
    wait_for_player_to_be_ready(state, distance);    
    draw_state(state);
  } else if (state->animation == 0) {
    play_game(state, distance);
    draw_state(state);
  } else if (state->animation == 2) {
    ending_animation(state, distance);
  }

  return state->step_duration;
}
