#include "game_main.h"
#include "us_sensor.h"

const long min_us_distance = 50;
const long max_us_distance = 200;

const long min_distance = us_cm_to_microseconds(min_us_distance);
const long max_distance = us_cm_to_microseconds(max_us_distance);

const long start_min_distance = us_cm_to_microseconds(min_us_distance + (max_us_distance - min_us_distance) / 3);
const long start_max_distance = us_cm_to_microseconds(min_us_distance + (max_us_distance - min_us_distance) / 3 * 2);

void initialize_game(GameState *state)
{
  state->current_step = 0;
  state->current_substep = 0;
  state->animation = 1;
  state->animation_step = 0;
  state->last_duration = us_cm_to_microseconds((min_us_distance + max_us_distance) / 2);
  state->step_duration = 10;
  state->player_position = state->led_count / 2;
  state->area_height = state->led_count - 10;
  state->area_top = 5;

  randomSeed(analogRead(0));
}

void draw_state(GameState *state)
{
  for (int i = 0; i < state->led_count; ++i) {
    if (i < state->area_top) {
      state->leds[i] = CRGB::Red;
    } else if (i == state->player_position) {
      state->leds[i] = CRGB::Blue;
    } else if (i > state->area_top + state->area_height) {
      state->leds[i] = CRGB::Red;
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

  const long random_length = 150;

  if (state->current_step % speedup_interval == 0 && state->step_duration > 4) {
    state->step_duration -= 2;
  }
  if (state->current_step % narrow_interval == 0 && state->area_height > 5) {
    state->area_height -= 1;
  }

  if (state->animation_step < random_length && state->animation_step >= 0) {
    const int change = random(3);
    if (change == 0 && state->area_top > 2) {
      state->area_top -= 1;
    } else if (change == 1 && state->area_top < state->led_count - state->area_height - 2) {
      state->area_top += 1;
    }
  } else {
    if (state->animation_step > 0) {
      state->animation_step = -1;
    }

    if (state->animation_step == -1 || state->animation_step == -3) {
      if (state->area_top > 2) {
        state->area_top -= 1;
      } else {
        state->animation_step--;
      }
    } else if (state->animation_step == -2) {
      if (state->area_top < state->led_count - state->area_height - 2) {
        state->area_top += 1;
      } else {
        state->animation_step--;
      }
    } else {
      state->animation_step = 0;
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
  CRGB color = CRGB::Black;
  state->current_substep++;
  if (state->current_substep > 20) {
    state->current_substep = 0;
    state->animation_step++;
  }
  if (state->current_substep > 10) {
    color = CRGB::Red;
  }

  for (int i = 0; i < state->led_count; ++i) {
    state->leds[i] = color;
  }
  if (state->animation_step > 5) {
    initialize_game(state);
  }
}

int step_game(GameState *state, long sensor_duration)
{
  if (state->player_position < 0) {
    initialize_game(state);
  }

  const long distance = (sensor_duration > min_distance && sensor_duration < max_distance) ? sensor_duration : state->last_duration;

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
