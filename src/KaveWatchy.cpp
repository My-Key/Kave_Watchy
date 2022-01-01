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

const float VOLTAGE_MIN = 3.4;
const float VOLTAGE_MAX = 4.2;
const float VOLTAGE_RANGE = VOLTAGE_MAX - VOLTAGE_MIN;

const int epd_bitmap_Battery_Top_LEN = 6;
const unsigned char* epd_bitmap_Battery_Top[epd_bitmap_Battery_Top_LEN] = {
	epd_bitmap_Battery_Top_1, epd_bitmap_Battery_Top_2, epd_bitmap_Battery_Top_3,
	epd_bitmap_Battery_Top_4, epd_bitmap_Battery_Top_5, epd_bitmap_Battery_Top_6
};

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
  int batState = int(((VBAT - VOLTAGE_MIN) / VOLTAGE_RANGE) * 57);
  if (batState > 57)
    batState = 57;
  if (batState < 0)
    batState = 0;

  int top = random(0, epd_bitmap_Battery_Top_LEN);

  display.drawBitmap(posBatteryX, posBatteryY, epd_bitmap_Battery_BG, 29, 101, GxEPD_BLACK);

  int fillPosY = posBatteryFillY + (57 - batState);

  int size = batState;

  if (size > 20)
    size = 20;

  display.drawBitmap(posBatteryFillX, fillPosY, epd_bitmap_Battery_Top[top], 25, size, GxEPD_WHITE);

  drawFillBitmap(posBatteryFillX, fillPosY + 2, 25, batState - 2, random(0, 25), random(0, 25), epd_bitmap_Bubbles, 25, 25, GxEPD_WHITE);
}


void KaveWatchy::drawFillBitmap(int16_t x, int16_t y, int16_t fw, int16_t fh, int16_t ox, int16_t oy, 
const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color)
{
  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  display.startWrite();
  for (int16_t fy = 0; fy < fh; fy++, y++) {
    for (int16_t fx = 0; fx < fw; fx++) {

      int16_t bx = (fx + ox) % w;
      int16_t by = (fy + oy) % h;

      if (bx & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(&bitmap[by * byteWidth + bx / 8]);
      if (byte & 0x80)
        display.writePixel(x + fx, y, color);
    }
  }
  display.endWrite();
}