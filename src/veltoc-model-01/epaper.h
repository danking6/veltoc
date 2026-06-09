#pragma once

#include <GxEPD2_BW.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
//#include <Fonts/FreeMonoBold9pt7b.h>
//#include <Fonts/FreeMonoBold12pt7b.h>
// u8g2 fonts list: https://github.com/olikraus/u8g2/wiki/fntlist99
// inlc. fonts: Sans, Serif, Mono, 9, 12, 18, 24

// Declare only (don't define)
extern float temperatureSensor;
extern float humiditySensor;
extern float pressureSensor;

extern float battPercent;
extern float battVoltage;

extern time_t storedEpoch;
extern uint16_t timeSyncFailures;


void initEpaper();

void drawCurrentView();
void drawCurrentPartial();
void drawTemperatureView();
void drawHumidityView();
void drawTimeView();
void drawSettingsMenu();
void drawMenuRow(MenuItem item, int y, bool selected);

void drawOptionButton(
    int x,
    int y,
    const char* label,
    bool selected);

void drawSettingValue(int index, int x, int y);
void drawEditDialog();
void drawSettingEditValue(int index);

void drawFullscreenNotif();

void updateTemperature(float temp);
void updateHumidity(float humidity);
void updateBattery(float temp);
void updateTime();

void drawSmallCard(int x, int y, String label, String description);
void drawLargeCard(String value, String units);

void drawBattery(int x, int y, int percent, float voltage);
void drawEmptyBattery(int x, int y);