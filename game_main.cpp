#include "game_main.h"
#include "game_constants.h"
#include "us_sensor.h"
#include "colors.h"
#include <EEPROM.h>

void initialize_game(GameState *state)
{
  if (state->player_position < 0) {
    randomSeed(analogRead(0));
    state->high_score = EEPROM.get(0, state->high_score);
    Serial.print(" Load high score: ");
    Serial.println(state->high_score);
  }

  state->current_step = 0;
  state->current_substep = 0;
  state->animation = ANIMATION_WAIT;
  state->animation_step = 0;
  state->phase_step = 50 + random(100);
  state->last_duration = us_cm_to_microseconds((min_us_distance + max_us_distance) / 2);
  state->step_duration = 15;
  state->player_position = state->led_count / 2;
  state->area_height = state->led_count - 10;
  state->area_top = 5;
  state->lava_burst_position = -1;
  state->collectible_position = -1;
  state->collectible_timer = 10;
  state->lava_burst_step = -60 - random(10);

  state->score = 0;

  Serial.println(" -- Start new game -- ");
}

CHSV color_animation(GameState *state, const int index, const CHSV *colors, const int count)
{
  return colors[(state->color_offset + index) % count];
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
      innerRadius = (1 + state->lava_burst_step - LAVA_BURST_LAVA_END) / LAVA_BURST_LAVA_STEP_LENGTH;
      warningRadius = (1 + LAVA_BURST_LAVA_END - LAVA_BURST_WARNING_START) / LAVA_BURST_LAVA_STEP_LENGTH;
    }
  }

  for (int i = 0; i < state->led_count; ++i) {
    int distance = INT16_MAX;
    if (state->lava_burst_position > LAVA_BURST_WARNING_START) {
      distance = abs(i - state->lava_burst_position);
    }
    if (i == state->player_position) {
      state->leds[i] = CRGB::Blue;
    } else if (i == state->collectible_position) {
      state->leds[i] = color_animation(state, i, gem_colors, gem_colors_count);
    } else if (i < state->area_top) {
      state->leds[i] = color_animation(state, i, lava_colors, lava_colors_count);
    } else if (i > state->area_top + state->area_height) {
      state->leds[i] = color_animation(state, i, lava_colors, lava_colors_count);
    } else if (distance < outerRadius && distance > innerRadius) {
      state->leds[i] = color_animation(state, i, lava_colors, lava_colors_count);
    } else if (distance < warningRadius && distance > innerRadius) {
      state->leds[i] = color_animation(state, i, warning_colors, warning_colors_count);
    } else {
      state->leds[i] = CRGB::Black;
    }
  }
}

void set_player_position(GameState *state, long distance)
{
  const long area_size = max_distance - min_distance;
#ifdef INVERT_CONTROL_DIRECTION
  state->player_position = (area_size - distance + min_distance) * state->led_count / (area_size);
#else 
  state->player_position = (distance - min_distance) * state->led_count / (area_size);
#endif
}

void wait_for_player_to_be_ready(GameState *state, long distance)
{
  set_player_position(state, distance);
  
  if (distance > start_min_distance && distance < start_max_distance) {
    state->current_step++;
    if (state->current_step > 150) {
      state->current_step = 0;
      state->animation = ANIMATION_PLAY;
    }
  } else {
    state->current_step = 0;
  }
}

void move_area(GameState *state)
{
  state->current_step++;
  state->animation_step++;

  if (state->current_step % speedup_interval == 0 && state->step_duration > 4) {
    state->step_duration -= 1;
  }
  if (state->current_step % narrow_interval == 0 && state->area_height > 10) {
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
      state->lava_burst_step = -40 - random(10);
      state->lava_burst_position = -1;
    }
  }
}

void lose_game(GameState *state)
{
  state->animation = ANIMATION_ENDING;
  state->current_substep = 0;
  state->animation_step = 0;

  Serial.print("GAME OVER, Score ");
  Serial.print(state->score);
  Serial.println("");
}

void reset_collectible(GameState *state)
{
  state->collectible_timer = 25 + random(25);
  state->collectible_position = -1;
}

void test_area(GameState *state)
{
  if (state->player_position < state->area_top
      || state->player_position > state->area_top + state->area_height) {
    lose_game(state);
  }
  if (state->collectible_position >= 0 && (state->collectible_position < state->area_top
      || state->collectible_position > state->area_top + state->area_height)) {
    reset_collectible(state);
  }

  if (state->lava_burst_step > LAVA_BURST_LAVA_START && state->lava_burst_step <= LAVA_BURST_LAVA_END) {
    const int radius = (1 + state->lava_burst_step - LAVA_BURST_LAVA_START) / LAVA_BURST_LAVA_STEP_LENGTH;
    if (abs(state->player_position - state->lava_burst_position) < radius) {
      lose_game(state);
    }
    if (state->collectible_position >= 0 && abs(state->collectible_position - state->lava_burst_position) < radius) {
      reset_collectible(state);
    }
  } else if (state->lava_burst_step > LAVA_BURST_LAVA_END) {
    const int outerRadius = (1 + LAVA_BURST_LAVA_END - LAVA_BURST_LAVA_START) / LAVA_BURST_LAVA_STEP_LENGTH;
    const int innerRadius = (1 + state->lava_burst_step - LAVA_BURST_LAVA_END) / LAVA_BURST_LAVA_STEP_LENGTH;
    const int distance = abs(state->player_position - state->lava_burst_position);
    const int collectible_distance = abs(state->collectible_position - state->lava_burst_position);
    if (distance < outerRadius && distance > innerRadius) {
      lose_game(state);
    }
    if (state->collectible_position >= 0 && collectible_distance < outerRadius && collectible_distance > innerRadius) {
      reset_collectible(state);
    }
  }
}

void update_collectible(GameState *state)
{
  if (state->collectible_timer > 0) {
    state->collectible_timer -= 1;
    if (state->collectible_timer == 0) {
      state->collectible_timer = -1;
      const int bottom_distance = (state->area_top + state->area_height) - state->player_position;
      const int top_distance = state->player_position - state->area_top;
      state->collectible_position = top_distance > bottom_distance ? state->area_top + 2 + random(3) : state->area_top + state->area_height - (2 + random(3));
      Serial.print("New collectible at ");
      Serial.print(state->collectible_position);
      Serial.println("");
    }
  }
}

void test_collectible(GameState *state)
{
  if (state->collectible_position >= 0 && state->player_position == state->collectible_position) {
    reset_collectible(state);
    state->score += 1;
    Serial.print("Gem collected, Score ");
    Serial.print(state->score);
    Serial.println("");
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

  update_collectible(state);
  test_collectible(state);
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
    state->current_substep = 0;
    state->animation_step = 0;
    state->player_position = 0;
    state->area_top = 0;
    state->step_duration = 15;
    state->animation = ANIMATION_SCORE;
  }
}

void draw_score_climb(GameState *state) {
  const int highest_point = state->led_count * 3 / 4;
  const int bottom_point = max(state->player_position - highest_point, 0);

  for (int i = 0; i < state->led_count; ++i) {
    const int score = bottom_point + i + 1;
    const CHSV score_color = (score / 10) % 2 == 0 ? CHSV(212, 255, 247) : CHSV(40, 255, 247);
    if (score <= state->player_position) {
      state->leds[i] = score_color;
    } else if (score == state->high_score) {
      state->leds[i] = CRGB::Green;
    } else {
      state->leds[i] = CRGB::Black;
    }
  }
}

void draw_high_score_blink(GameState *state) {
  if ((state->current_substep / 10) % 2 == 0) {
    for (int i = 0; i < state->led_count; ++i) {
      state->leds[i] = CRGB::Black;
    }
  } else {
    draw_score_climb(state);
  }
}

void score_animation(GameState *state) {
  if (state->player_position < state->score) {
    state->current_substep += 1;
    if (state->animation_step == 0) {
      const int slow_limit = state->score / 10 * 10;
      const int jump_after = state->player_position >= slow_limit ? 25 : 1;
      if (state->current_substep > jump_after) {
        state->current_substep = 0;
        state->player_position += 1;

        if ((state->player_position + 1) % 10 == 0) {
          state->animation_step = 1;
        }
      }
    } else if (state->animation_step == 1) {
      if (state->current_substep > 30) {
        state->current_substep = 0;
        state->animation_step = 0;
      }
    }

    if (state->player_position == state->score) {
      state->current_substep = 0;
      state->animation_step = state->score > state->high_score ? 1 : 0;
    }

    draw_score_climb(state);
  } else {
    state->current_substep += 1;
    if (state->animation_step == 0) {
      draw_score_climb(state);
      if (state->current_substep > 200) {
        state->animation_step = -1;
      }
    } else {
      draw_high_score_blink(state);
      if (state->current_substep > 300) {
        state->animation_step = -1;
      }
    }
  }
  
  if (state->animation_step < 0) {
    if (state->score > state->high_score) {
      state->high_score = state->score;
      Serial.print(" New high score: ");
      Serial.println(state->high_score);
      EEPROM.put(0, state->high_score);
    }
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

  state->color_counter += 1;
  if (state->color_counter > 5) {
    state->color_counter = 0;
    state->color_offset += 1;
    if (state->color_offset >= lava_colors_count) {
      state->color_offset = 0;
    }
  }

  long distance = (sensor_duration > min_distance && sensor_duration < max_distance) ? sensor_duration : state->last_duration;
  const long change = distance - state->last_duration;

  if (abs(change) > max_move_speed) {
    distance = sign(change) * max_move_speed + state->last_duration;
  }

  if (state->animation == ANIMATION_WAIT) {
    wait_for_player_to_be_ready(state, distance);    
    draw_state(state);
  } else if (state->animation == ANIMATION_PLAY) {
    play_game(state, distance);
    draw_state(state);
  } else if (state->animation == ANIMATION_ENDING) {
    ending_animation(state, distance);
  } else if (state->animation == ANIMATION_SCORE) {
    score_animation(state);
  }

  state->last_duration = distance;

  return state->step_duration;
}
