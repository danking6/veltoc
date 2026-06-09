
// Local project files
#include "config.h"
#include "settings.h"
#include "menu.h"
#include "sensors.h"
#include "epaper.h"

// Other config files and packages
#include <ESP32RotaryEncoder.h> // https://github.com/MaffooClock/ESP32RotaryEncoder/tree/main
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "driver/rtc_io.h"
#include <WiFi.h>
#include <time.h>


RotaryEncoder rotaryEncoder( ROT_ENC_A, ROT_ENC_B );

// Vars for button press detection
bool lastReading = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;   // ms
unsigned long pressStartTime = 0;
long lastEncoderValue = 0;

unsigned long lastActivityTime = 0;

bool debug = 1;

int wakeTimer = 300; // Seconds to sleep for until next wake
int sleepTimeout = 30; // Seconds before entering sleep. Can change depeneding on the selected view


RTC_DATA_ATTR uint32_t bootCount = 0;
RTC_DATA_ATTR uint16_t timeSyncFailures = 100;

const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";
const char* ntpServer = "pool.ntp.org";
const char* timezoneString = "EST5EDT,M3.2.0,M11.1.0"; // POSIX timezone rule



/**
 * ESP32 setup function
 *
 */
void setup()
{
  pinMode(ACTIVITY_LED_PIN, OUTPUT);
  analogWrite(ACTIVITY_LED_PIN, settings.activityLed ? 25 : 0);
  
  Serial.begin(115200);
  
  // Start activity timer
  lastActivityTime = millis();

  bootCount++;

 
  pinMode(ROT_ENC_BTN, INPUT_PULLUP);
  pinMode(ROT_ENC_A, INPUT_PULLUP);
  pinMode(ROT_ENC_B, INPUT_PULLUP);

  rotaryEncoder.setEncoderType( EncoderType::FLOATING ); 
  rotaryEncoder.setBoundaries( -1000, 1000, false );
  rotaryEncoder.onTurned( &knobCallback );
  rotaryEncoder.begin();
  
  // reset states
  appState.inEditMode = false;
  appState.inSettingsMenu = false;

  //ESP_SLEEP_WAKEUP_TIMER or ESP_SLEEP_WAKEUP_UNDEFINED
  esp_sleep_wakeup_cause_t wakeReason = esp_sleep_get_wakeup_cause();
  bool manualWake = false;
  if (wakeReason == ESP_SLEEP_WAKEUP_UNDEFINED || wakeReason == ESP_SLEEP_WAKEUP_EXT1) {
    manualWake = true;
    sleepTimeout = 30;
    Serial.println("Manual wake");
  }
  else {
    sleepTimeout = 0;
    Serial.println("Auto/timed wake");
  }

  
  // Sync time over wifi every 60 startups
  if (bootCount == 1 || (bootCount % 60 == 0)) {
    connectWiFi();
    if (WiFi.status() == WL_CONNECTED){
      syncTime();
      disconnectWiFi();
    }
  } 
  setenv("TZ", timezoneString, 1);
  tzset();

  // read sensors
  readBME280Sensor();
  readVoltageSensor();
  
  initEpaper();
  

  // Battery low/dead notification
  if (battPercent <= 5) { 
    appState.currentView = VIEW_FULLSCREEN_NOTIF;
    appState.fullscreenNotif = BATTERY;
    appState.viewChanged = true;
  }

  if (bootCount == 1 || (bootCount % 30 == 0)) {
    appState.fullRedrawNeeded = true;
    appState.viewChanged = true;
  }
  else {
    appState.partialViewChanged = true;
  }
} //End setup()


/**
 * Main loop 
 *
 */
void loop() 
{
  checkButton();

  //digitalWrite(ACTIVITY_LED_PIN, settings.activityLed ? HIGH : LOW);
  //analogWrite(ACTIVITY_LED_PIN, settings.activityLed ? 25 : 0);

  // Whole screen changed, ie. rotary encoder knob turned
  if (appState.viewChanged) {
    drawCurrentView();
    appState.viewChanged = false;
  }
 
  // Partial screen changed
  if (appState.partialViewChanged) {
    drawCurrentPartial();
    appState.partialViewChanged = false;
  }

  // Check inactivity timeout and sleep
  if ((millis() - lastActivityTime) > (sleepTimeout * 1000)) {
    goToSleep(0);
  }
}


/**
 * Rotary encoder knob rotated
 *
 * 
 */
void knobCallback( long value )
{
  //Serial.printf( "Raw value: %i\n", value );
  lastActivityTime = millis();
  Serial.printf( "last activity time: %i\n", lastActivityTime );
  if(lastEncoderValue < value) {
    if (appState.currentView == VIEW_SETTINGS && appState.inSettingsMenu) {
      encoderCW();
    } else {
      nextView();
    }
  }
  else {
    if (appState.currentView == VIEW_SETTINGS && appState.inSettingsMenu) {
      encoderCCW();
    } else {
      previousView();
    }
  }

  appState.viewChanged = true;
  lastEncoderValue = value;
}

/**
 * Next view
 * 
 */
void nextView() 
{
  appState.currentView = (ScreenView)((appState.currentView + 1) % (VIEW_COUNT-1));
}

/**
 * Previous view
 * 
 */
void previousView() 
{
  int view = appState.currentView - 1;

  if (view < 0) {
    view = VIEW_COUNT - 2;
  }

  appState.currentView = (ScreenView)view;
}

/**
* Encoder rotation in settings menu
*/
void encoderCW() {

  if (!appState.inEditMode) {

    appState.selectedMenuIndex++;

    if (appState.selectedMenuIndex >= settingsMenuCount) {
      appState.selectedMenuIndex = 0;
    }

  } else {

    // modify setting value
    (*editingItem->value)++;
    
    if(*editingItem->value >= editingItem->optionCount)
      *editingItem->value = 0;
    
    //Serial.println("editnig item: " + String(*editingItem->value) + ", option count:" + String(editingItem->optionCount));
  }
}

/**
* Encoder rotation in settings menu
*/
void encoderCCW() {

  if (!appState.inEditMode) {

    appState.selectedMenuIndex--;

    if (appState.selectedMenuIndex < 0) {
      appState.selectedMenuIndex =
        settingsMenuCount - 1;
    }

  } else {

    // modify setting value
    (*editingItem->value)--;
    //Serial.println("editnig item: " + String(*editingItem->value) + ", option count:" + String(editingItem->optionCount));

  if(*editingItem->value < 0)
    *editingItem->value = editingItem->optionCount - 1;

  }
}


/**
 * Rotary encoder button pushed
 *
 * 
 */
void buttonCallback( unsigned long duration )
{
   Serial.printf( "Button pressed for %u ms\n", duration );
   lastActivityTime = millis();

  if (appState.currentView == VIEW_SETTINGS && ! appState.inSettingsMenu) {
    appState.inSettingsMenu = true;
    appState.viewChanged = true;
    return;
  }

  if (appState.currentView == VIEW_SETTINGS) {
    if (!appState.inEditMode) {
      // Enter edit mode
      appState.inEditMode = true;
      
      editingItem = &settingsMenu[appState.selectedMenuIndex];
    }
    else {
      appState.inEditMode = false;
    }
  }

  appState.viewChanged = true;
}

/**
 * Custom rotary encoder button check. (The ESP32RotaryEncoder.h library was
 * missing button presses)
 */
void checkButton() 
{
  bool reading = digitalRead(ROT_ENC_BTN);

  // Reset debounce timer if state changed
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  // If stable longer than debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      // Button pressed
      if (buttonState == LOW) {
        pressStartTime = millis();
      }

      // Button released
      if (buttonState == HIGH) {
        buttonCallback(millis() - pressStartTime);
      }
    }
  }

  lastReading = reading;
}


/**
 * Deep sleep after delay seconds
 * @param int delay seconds
 *
 */
void goToSleep(int timeout)
{
  timeout = timeout * 1000;
  if (timeout > 0) {
    Serial.println("Staying awake for a bit...");
    // Stay awake for x seconds
    delay(timeout);
  }

  // Time view, wake every minute, on :00
  if (appState.currentView == VIEW_TIME) {
    time_t epoch = time(nullptr);
    wakeTimer = 60 - (epoch % 60);
    if (wakeTimer > 65) { wakeTimer = 60; }
  }
  // Ssettings view, reset to temperature view
  if (appState.currentView == VIEW_SETTINGS) {
    appState.currentView = VIEW_TEMPERATURE;
    drawCurrentView();
  }

  Serial.println("Entering deep sleep now.");

  esp_sleep_enable_timer_wakeup(wakeTimer * 1000000); 

  // Wake when ANY selected pin goes LOW. (ESP32 C6 doesn't have Ext0 functionality)
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_LOW);
  rtc_gpio_isolate(ROT_ENC_BTN);

  //delay(100); // allow serial to flush
  esp_deep_sleep_start();
}



void connectWiFi()
{
  Serial.print("Connecting to Wifi ");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
  }

  if (retries <= 20) {
    Serial.println(" Wifi connected");
  } else {
    Serial.println(" Unable to connect to Wifi");
  }
}


void disconnectWiFi()
{
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}


void syncTime()
{
  Serial.print("Synchronizing time from server. ");
  time_t before;
  time(&before);
  Serial.print("Before sync: ");
  Serial.print(before);

  configTime(0, 0, ntpServer);

  //delay(4000);

  uint8_t retries = 0;
  while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (retries == 20) {
    Serial.println("Time sync failed!");
    timeSyncFailures++;
  } else {
    timeSyncFailures = 0;
    time_t after;
    time(&after);
    Serial.print("After sync: ");
    Serial.println(after);
  }
}
