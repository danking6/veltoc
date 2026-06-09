#pragma once


enum TemperatureUnit {
  TEMP_F,
  TEMP_C
};

enum FullscreenNotifType {
  NONE,
  BATTERY,
  WELCOME
};

enum ScreenView {
  VIEW_TEMPERATURE,
  VIEW_HUMIDITY,
  VIEW_TIME,
  VIEW_SETTINGS,
  VIEW_FULLSCREEN_NOTIF,
  VIEW_COUNT
};

struct AppSettings {

  TemperatureUnit tempUnit;

  ScreenView currentView;
  

  int temperatureUnits;
  int activityLed;
  int wifiEnable;
  int timeFormat;
};

struct AppState {

  ScreenView currentView;
  FullscreenNotifType fullscreenNotif;

  bool fullRedrawNeeded;
  bool viewChanged;
  bool partialViewChanged;


  int selectedMenuIndex;
  bool inSettingsMenu;
  bool inEditMode;
};

// Shared global settings object
extern AppSettings settings;
extern AppState appState;

// Functions
void loadSettings();
void saveSettings();