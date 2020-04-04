#ifndef us_sensor_h
#define us_sensor_h

void initialize_us_sensor(int ping_pin, int echo_pin);
long activate_us_sensor(int ping_pin, int echo_pin);

long us_microseconds_to_inches(long microseconds);
long us_microseconds_to_cm(long microseconds);
long us_cm_to_microseconds(long cm);

#endif
