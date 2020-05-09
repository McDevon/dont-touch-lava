#include "game_constants.h"
#include "us_sensor.h"

const long min_distance = us_cm_to_microseconds(min_us_distance);
const long max_distance = us_cm_to_microseconds(max_us_distance);

const long start_min_distance = us_cm_to_microseconds(min_us_distance + (max_us_distance - min_us_distance) / 3);
const long start_max_distance = us_cm_to_microseconds(min_us_distance + (max_us_distance - min_us_distance) / 3 * 2);

const long max_move_speed = us_cm_to_microseconds(max_us_move_per_step);
const long move_outlier_threshold = us_cm_to_microseconds(us_outlier_threshold);