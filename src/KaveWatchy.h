#ifndef BOTWATCHY_H
#define BOTWATCHY_H

#include <Watchy.h>
#include "../include/images.h"
#include "../include/NIOBRG__10pt7b.h"
#include "../include/NIOBRG__30pt7b.h"

#define DRAW_TEMPERATURE true

typedef struct weatherDataOneCall{
  boolean invalid;
  int8_t temperature;
  int16_t weatherConditionCode0;
  int16_t weatherConditionCode1;
  int16_t weatherConditionCode2;
}weatherDataOneCall;

class KaveWatchy : public Watchy
{
public:
  KaveWatchy();
  void drawWatchFace();
  void drawTime();
  void drawDate();
  void drawWeather();
  void drawWeatherIcon(int8_t iconPosX, int16_t iconWeatherConditionCode);
  void drawSteps();
  void drawBattery();
  void drawWifi();

  const char* Ordinal(uint8_t num);
  const unsigned char* HeartBitmap(int amount);

  weatherDataOneCall getWeatherData();
};

#endif
