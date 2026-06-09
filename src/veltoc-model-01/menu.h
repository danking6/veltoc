#pragma once

/*struct MenuItem {
  const char* label;
};
*/

enum SettingType {
    SETTING_BOOL,
    SETTING_INT
};

struct MenuItem {
  const char* label;
  const char** options;
  int optionCount;
  int* value;
  //SettingType type;
 // void* settingPtr;
};

// Shared global settings object
extern MenuItem settingsMenu[];
extern const int settingsMenuCount;
extern MenuItem* editingItem;