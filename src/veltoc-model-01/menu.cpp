#include "menu.h"
#include "settings.h"
/*
MenuItem settingsMenu[] = {
  { "Temperature" },
  { "Activity Light" },
  { "Sleep Timeout" }
};

const int settingsMenuCount =
  sizeof(settingsMenu) / sizeof(MenuItem);
*/



//int temperatureUnit = 0; // 0 = F, 1 = C
//int refreshRate = 1;    // 0 = Slow, 1 = Medium, 2 = Fast
//int wifiEnabled = 0;    // 0 = Off, 1 = On

const char* tempOptions[] = { "C", "F" };

const char* wifiOptions[] = { "Off", "On" };

const char* activityLedOptions[] = { "Off", "On" };

const char* timeFormatOptions[] = { "12-hour", "24-hour" };
/*
struct MenuItem {
  const char* label;
  const char** options;
  int optionCount;
  int* value;
  SettingType type;
  void* settingPtr;
};
*/
MenuItem settingsMenu[] = {
  {
    "Temperature",
    tempOptions,
    2,
    &settings.temperatureUnits
  },

  {
    "Activity LED",
    activityLedOptions,
    2,
    &settings.activityLed
  },

  {
    "WiFi",
    wifiOptions,
    2,
    &settings.wifiEnable
  },

  {
    "Time Format",
    timeFormatOptions,
    2,
    &settings.timeFormat
  }
};

const int settingsMenuCount =
  sizeof(settingsMenu) / sizeof(MenuItem);


MenuItem* editingItem = nullptr;