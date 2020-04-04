#import <Arduino.h>
#include "us_sensor.h"

void initialize_us_sensor(int ping_pin, int echo_pin)
{
  pinMode(ping_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
}

long activate_us_sensor(int ping_pin, int echo_pin)
{
  digitalWrite(ping_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(ping_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ping_pin, LOW);
  long microseconds = pulseIn(echo_pin, HIGH);
  
  return microseconds;
}

long us_microseconds_to_inches(long microseconds)
{
   return microseconds / 74 / 2;
}

long us_microseconds_to_cm(long microseconds)
{
   return microseconds / 29 / 2;
}

long us_cm_to_microseconds(long cm)
{
   return cm * 2 * 29;
}
