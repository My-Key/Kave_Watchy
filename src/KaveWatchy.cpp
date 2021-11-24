#include "KaveWatchy.h"

const int posBatteryX = 15;
const int posBatteryY = 15;

const int posBatteryFillX = posBatteryX + 2;
const int posBatteryFillY = posBatteryY + 14;

const int posTimeCenterX = 120;
const int posTimeY = 120;

const int posAMPMTimeX = 160;
const int posAMPMTimeY = 140;

const int posDateX = 25;
const int posDate1Y = 150;

const int posDate2Y = 175;

const int posStepsIconX = 137;
const int posStepsIconY = 15;

const int posStepsBGX = 51;
const int posStepsBGY = 25;

const int posStepsOffsetY = 22;

const float VOLTAGE_MIN = 3.2;
const float VOLTAGE_MAX = 4.1;
const float VOLTAGE_RANGE = 0.9;

KaveWatchy::KaveWatchy()
{
  // Serial.begin(115200);
}

void KaveWatchy::drawWatchFace()
{
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, epd_bitmap_BG, 200, 200, GxEPD_BLACK);
  display.setTextColor(GxEPD_BLACK);

  drawTime();
  drawDate();
  drawSteps();
  drawBattery();
}

void KaveWatchy::drawTime()
{
  display.setFont(&NIOBRG__30pt7b);

  bool am = currentTime.Hour < 12;
  int hour = currentTime.Hour;

  if (HOUR_12_24 == 12)
    hour = ((hour+11)%12)+1;
    
  int16_t  x1, y1;
  uint16_t w, h;

  display.getTextBounds(String(":"), 0, 0, &x1, &y1, &w, &h);

  int colonXStart = posTimeCenterX - w / 2;
  int colonXEnd = posTimeCenterX + w / 2 + 4;

  String hourString = "";
  
  if (hour < 10)
    hourString += "0";
  
  hourString += String(hour);

  display.getTextBounds(hourString, 0, 0, &x1, &y1, &w, &h);

  display.setCursor(colonXStart - w - 4, posTimeY);
  display.print(hourString);
  
  display.setCursor(colonXStart, posTimeY);
  display.print(":");

  String minuteString = "";

  if (currentTime.Minute < 10)
    minuteString += "0";

  minuteString += String(currentTime.Minute);

  display.setCursor(colonXEnd, posTimeY);
  display.print(minuteString);

  if (HOUR_12_24 != 12)
    return;

  display.setFont(&NIOBRG__10pt7b);
  display.setCursor(posAMPMTimeX, posAMPMTimeY);
  display.print(am ? "AM" : "PM");
}

const char* KaveWatchy::Ordinal(uint8_t num)
{
  switch(num % 100)
  {
      case 11:
      case 12:
      case 13:
        return "th";
  }

  switch(num % 10)
  {
      case 1:
        return "st";
      case 2:
        return "nd";
      case 3:
        return "rd";
      default:
        return "th";
  }
}

void KaveWatchy::drawDate()
{
  display.setFont(&NIOBRG__10pt7b);

  String dayOfWeek = dayStr(currentTime.Wday);
  String month = monthStr(currentTime.Month);

  display.setCursor(posDateX, posDate1Y);
  display.println(dayOfWeek);

  display.setCursor(posDateX, posDate2Y);
  display.print(month);
  display.print(" ");

  display.print(currentTime.Day);
  
  display.print(Ordinal(currentTime.Day));
}

void KaveWatchy::drawSteps(){
    // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0){
      sensor.resetStepCounter();
    }
    
    uint32_t stepCount = sensor.getCounter();

    display.drawBitmap(posStepsIconX, posStepsIconY, epd_bitmap_Shell, 48, 50, GxEPD_BLACK);
    
    display.drawBitmap(posStepsBGX, posStepsBGY, epd_bitmap_Steps_BG, 85, 30, GxEPD_BLACK);

    int16_t  x1, y1;
    uint16_t w, h;
    display.getTextBounds(String(stepCount), 0, 0, &x1, &y1, &w, &h);

    display.setFont(&NIOBRG__10pt7b);
    display.setCursor(posStepsIconX - 10 - w, posStepsBGY + posStepsOffsetY);
    display.println(stepCount);
}

void KaveWatchy::drawBattery()
{
  float VBAT = getBatteryVoltage();

  // 12 battery states
  int batState = int(((VBAT - VOLTAGE_MIN) / VOLTAGE_RANGE) * 4);
  if (batState > 4)
    batState = 4;
  if (batState < 0)
    batState = 0;

  display.drawBitmap(posBatteryX, posBatteryY, epd_bitmap_Battery_BG, 29, 101, GxEPD_BLACK);

  switch(batState)
  {
    case 4:
      display.drawBitmap(posBatteryFillX, posBatteryFillY, epd_bitmap_Battery_Fill_100, 25, 60, GxEPD_WHITE);
      break;
    case 3:
      display.drawBitmap(posBatteryFillX, posBatteryFillY, epd_bitmap_Battery_Fill_75, 25, 60, GxEPD_WHITE);
      break;
    case 2:
      display.drawBitmap(posBatteryFillX, posBatteryFillY, epd_bitmap_Battery_Fill_50, 25, 60, GxEPD_WHITE);
      break;
    case 1:
      display.drawBitmap(posBatteryFillX, posBatteryFillY, epd_bitmap_Battery_Fill_25, 25, 60, GxEPD_WHITE);
      break;
    default:
      display.drawBitmap(posBatteryFillX, posBatteryFillY, epd_bitmap_Battery_Fill_0, 25, 60, GxEPD_WHITE);
      break;
  }
}
