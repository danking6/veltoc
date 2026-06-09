#include "settings.h"
#include <Arduino.h>

RTC_DATA_ATTR AppSettings settings = {
  TEMP_F,
  VIEW_TEMPERATURE,
  1, //temperatureUnits (0=C, 1=F)
  1, //activityLight
  0, //wifiEnable
  0 //timeFormat
};

RTC_DATA_ATTR AppState appState = {
  VIEW_TEMPERATURE,
  NONE,
  false, //fullRedrawNeeded
  false, //viewChanged
  false, //partialViewChanged
  
  0, //selectedMenuIndex
  false, // inSettingsMenu
  false //inEditMode
};

void loadSettings() {

  // later:
  // read from Preferences/NVS
}

void saveSettings() 
{
  
}