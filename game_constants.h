#ifndef game_constants_h
#define game_constants_h

#define INVERT_CONTROL_DIRECTION
//#undef INVERT_CONTROL_DIRECTION

#define min_us_distance 20
#define max_us_distance 150
#define max_us_move_per_step 3
#define us_outlier_threshold 30

extern const long min_distance;
extern const long max_distance;

extern const long start_min_distance;
extern const long start_max_distance;

extern const long max_move_speed;
extern const long move_outlier_threshold;

#define speedup_interval 100
#define narrow_interval 70

#define LAVA_BURST_WARNING_START 5
#define LAVA_BURST_LAVA_STEP_LENGTH 5
#define LAVA_BURST_LAVA_START 15
#define LAVA_BURST_LAVA_END 30

#define ANIMATION_WAIT 1
#define ANIMATION_PLAY 0
#define ANIMATION_ENDING 2
#define ANIMATION_SCORE 3

#endif