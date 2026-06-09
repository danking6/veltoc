#include "config.h"
#include "menu.h"
#include "settings.h"
#include "epaper.h"



// Init epaper
GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(
  GxEPD2_290_T94_V2(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)
);
/*
GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(
  GxEPD2_290_C90c(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)
);
*/
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

/**
 * Set attributes for e-paper display
 *
 */
void initEpaper()
{
  //SPI.begin();
  SPI.begin(D8, -1, D10, D1);
  display.init(115200, false, 2, false);

  

  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);

  u8g2Fonts.begin(display);

  u8g2Fonts.setFontMode(0); // solid
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
}


void drawCurrentView() {

  switch (appState.currentView) {

    case VIEW_TEMPERATURE:
      drawTemperatureView();
      break;

    case VIEW_HUMIDITY:
      drawHumidityView();
      break;

    case VIEW_TIME:
      drawTimeView();
      break;

    case VIEW_SETTINGS:
      if (appState.inEditMode) {
        drawEditDialog();
      } else { 
        drawSettingsMenu();
      }
      break;
    
    case VIEW_FULLSCREEN_NOTIF:
      drawFullscreenNotif();
      break;
  }
}

void drawCurrentPartial() {

  switch (appState.currentView) {

    case VIEW_TEMPERATURE:
      updateTemperature(temperatureSensor);
      break;

    case VIEW_HUMIDITY:
      updateHumidity(humiditySensor);
      break;

    case VIEW_TIME:
      updateTime();
      break;

  }
}


/**
 * Draw the dashboard/UI elements on epaper
 *
 */
void drawTemperatureView()
{
  if (appState.fullRedrawNeeded) {
    display.setFullWindow();
    appState.fullRedrawNeeded = false;
  } else {
    display.setPartialWindow(0, 0, display.width(), display.height());
  }
  
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    if (settings.temperatureUnits == 0) {
      String tempRnd = String(temperatureSensor, 1);
      if (temperatureSensor < 10 && temperatureSensor > -10) {
        tempRnd = " " + tempRnd;
      }
      drawLargeCard(tempRnd, "°C");
    }
    else {
      // Conver to deg. F, and round
      String tempRnd = String((temperatureSensor * 1.8) + 32, 1);
      if ((temperatureSensor * 1.8) + 32 < 10) {
        tempRnd = " " + tempRnd;
      }
      drawLargeCard(tempRnd, "°F");
    }

    // Small cards
    int humRnd = round(humiditySensor);
    drawSmallCard(200, 15, String(humRnd) + "%", "HUMIDITY");
    

    int pressRnd = round(pressureSensor);
    drawSmallCard(200, 70, String(pressRnd), "PRESSURE");
    
    //drawBattery(10, 110, 20, 12, battPercent, 0); //battVoltage
    drawBattery(10, 5, battPercent, 0); //battVoltage
    

    // Signal
    //if (Zigbee.connected()) {
    //  drawSignal(275, 120);
    //}

  } while (display.nextPage());
}

/**
* Partial update for temperature
*
*/
void updateTemperature(float temp) 
{
  // Define partial update area
  display.setPartialWindow(0, 25, 190, 90); //left (x), top (y), width, height
  display.firstPage();

  do {
    // Clear only this region
    display.fillRect(0, 10, 180, 90, GxEPD_WHITE);

     if (settings.temperatureUnits == 0) {
      String tempRnd = String(temperatureSensor, 1);
      if (temperatureSensor < 10 && temperatureSensor > -10) {
        tempRnd = " " + tempRnd;
      }
      drawLargeCard(tempRnd, "°C");
    }
    else {
      // Conver to deg. F, and round
      String tempRnd = String((temperatureSensor * 1.8) + 32, 1);
      if ((temperatureSensor * 1.8) + 32 < 10) {
        tempRnd = " " + tempRnd;
      }
      drawLargeCard(tempRnd, "°F");
    }

  } while (display.nextPage());
}

/**
* Partial update for temperature
*
*/
void updateHumidity(float humidity) 
{
  // Define partial update area
  display.setPartialWindow(0, 10, 180, 90); //left (x), top (y), width, height
  display.firstPage();

  do {
    // Clear only this region
    display.fillRect(0, 10, 180, 90, GxEPD_WHITE);

    int humRnd = round(humidity);
    drawLargeCard(" " + String(humRnd), "%");

  } while (display.nextPage());
}


void updateBattery(float temp) 
{
}

/**
 * Draw the large/main card-style element on the left
 *
 */
void drawLargeCard(String value, String units)
{
  u8g2Fonts.setFont(u8g2_font_logisoso58_tn); // digital font, 62, 46, 38
  u8g2Fonts.setCursor(18, 95); 
  u8g2Fonts.print(value);
  
  u8g2Fonts.setFont(u8g2_font_logisoso24_tf);
  u8g2Fonts.setCursor(155, 63);
  u8g2Fonts.print(units);
}

/**
 * Draw the small card-style element on the right side
 *
 */
void drawSmallCard(int x, int y, String label, String description) 
{
  display.drawLine(x, y, x, y+42, GxEPD_BLACK);
  display.drawLine(x + 1, y, x + 1, y+42, GxEPD_BLACK);

  u8g2Fonts.setFont(u8g2_font_logisoso26_tf);
  u8g2Fonts.setCursor(x+18, y+28);
  u8g2Fonts.print(label);

  //display.setTextColor(GxEPD_BLACK);
  u8g2Fonts.setFont(u8g2_font_nerhoe_tf);//u8g2_font_6x10_tf, u8g2_font_t0_11_tf, u8g2_font_oskool_tf, 
  u8g2Fonts.setCursor(x+18, y+41);
  u8g2Fonts.print(description);
}




/**
 * Draw the dashboard/UI elements on epaper
 *
 */
void drawHumidityView()
{
  if (appState.fullRedrawNeeded) {
    display.setFullWindow();
    appState.fullRedrawNeeded = false;
  } else {
    display.setPartialWindow(0, 0, display.width(), display.height());
  }

  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);

    int humRnd = round(humiditySensor);
    drawLargeCard(" " + String(humRnd), "%");

    // Small cards
    String tempRnd = String((temperatureSensor * 1.8) + 32, 1);
    drawSmallCard(200, 15, tempRnd, "TEMP");

    int pressRnd = round(pressureSensor);
    drawSmallCard(200, 70, String(pressRnd), "PRESSURE");
    
    drawBattery(10, 110, battPercent, 0); //battVoltage

    // Signal
    //if (Zigbee.connected()) {
    //  drawSignal(275, 120);
    //}

  } while (display.nextPage());
}


/**
 * Draw the dashboard/UI elements on epaper
 *
 */
void drawTimeView()
{
  if (appState.fullRedrawNeeded) {
    display.setFullWindow();
    appState.fullRedrawNeeded = false;
  } else {
    display.setPartialWindow(0, 0, display.width(), display.height());
  }
  
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    //drawLargeCard("10:25", "");

    //https://github.com/olikraus/u8g2/wiki/fntgrpiconic?
    //u8g2Fonts.setFont(u8g2_font_open_iconic_all_2x_t);
    
    /*
    u8g2Fonts.setFont(u8g2_font_open_iconic_www_4x_t);
    u8g2Fonts.drawGlyph(85, 70, 81); // battery
    
    u8g2Fonts.setFont(u8g2_font_nerhoe_tf);
    u8g2Fonts.setCursor(40, 90);
    u8g2Fonts.print("Please connect to wifi to"); 
    u8g2Fonts.setCursor(60, 105);
    u8g2Fonts.print("display the time.");
    */
   
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); // "%A, %B %d %Y %H:%M:%S" -> Monday, May 25 2026 01:50:54
        char timeString[6];

        strftime(timeString, sizeof(timeString), (settings.timeFormat == 1 ? "%H:%M" : "%I:%M"), &timeinfo);
        
        u8g2Fonts.setFont(u8g2_font_logisoso58_tn); 

        // Hide leading zero for 12-hour time
        if (timeString[0] == '0' && settings.timeFormat == 0) {
          u8g2Fonts.setCursor(35, 93);
          u8g2Fonts.print(&timeString[1]);
        }
        else {
          u8g2Fonts.setCursor(10, 93);
          u8g2Fonts.print(&timeString[0]);
        }

        if (timeSyncFailures >= 8) {
          u8g2Fonts.setFont(u8g2_font_open_iconic_embedded_2x_t);
          u8g2Fonts.drawGlyph(5, 110, 71); // warning icon
          u8g2Fonts.setFont(u8g2_font_nerhoe_tf);
          u8g2Fonts.setCursor(25, 110);
          u8g2Fonts.print("Time may be inaccurate, sync failed"); 
        }
        

    } else {
      u8g2Fonts.setFont(u8g2_font_open_iconic_embedded_4x_t);
      u8g2Fonts.drawGlyph(85, 60, 71); // warning icon
      u8g2Fonts.setFont(u8g2_font_nerhoe_tf);
      u8g2Fonts.setCursor(35, 80);
      u8g2Fonts.print("Unable to sync time, check wifi"); 
      u8g2Fonts.setCursor(70, 95);
      u8g2Fonts.print("or time server."); 
    }

    // Small cards
    String tempRnd = String((temperatureSensor * 1.8) + 32, 1);
    drawSmallCard(200, 15, tempRnd, "TEMPERATURE");

    // Small cards
    int humRnd = round(humiditySensor);
    drawSmallCard(200, 70, String(humRnd) + "%", "HUMIDITY");
    
    drawBattery(10, 5, battPercent, 0); //battVoltage

  } while (display.nextPage());
}


/**
* Partial update for time
*
*/
void updateTime() 
{
  // Define partial update area
  display.setPartialWindow(0, 25, 190, 80); //left (x), top (y), width, height
  display.firstPage();

  do {
    // Clear only this region
    display.fillRect(0, 25, 190, 90, GxEPD_WHITE);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); // "%A, %B %d %Y %H:%M:%S" -> Monday, May 25 2026 01:50:54
      char timeString[6];

      strftime(timeString, sizeof(timeString), (settings.timeFormat == 1 ? "%H:%M" : "%r:%M"), &timeinfo);
      
      u8g2Fonts.setFont(u8g2_font_logisoso58_tn); 
      
      // Hide leading zero for 12-hour time
      if (timeString[0] == '0' && settings.timeFormat == 0) {
        u8g2Fonts.setCursor(35, 93);
        u8g2Fonts.print(&timeString[1]);
      }
      else {
        u8g2Fonts.setCursor(10, 93);
        u8g2Fonts.print(&timeString[0]);
      }

      if (timeSyncFailures >= 8) {
        u8g2Fonts.setFont(u8g2_font_open_iconic_embedded_2x_t);
        u8g2Fonts.drawGlyph(5, 110, 71); // warning icon
        u8g2Fonts.setFont(u8g2_font_nerhoe_tf);
        u8g2Fonts.setCursor(25, 110);
        u8g2Fonts.print("Time may be inaccurate, sync failed"); 
      }

    } else {
        Serial.println("Failed to obtain time");
    }

  } while (display.nextPage());
}



/**
 * Draw the dashboard/UI elements on epaper
 *
 */
void drawFullscreenNotif()
{
  if (appState.fullRedrawNeeded) {
    display.setFullWindow();
    appState.fullRedrawNeeded = false;
  } else {
    display.setPartialWindow(0, 0, display.width(), display.height());
  }
  
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    //drawLargeCard("10:25", "");

    //https://github.com/olikraus/u8g2/wiki/fntgrpiconic?
    //u8g2Fonts.setFont(u8g2_font_open_iconic_all_2x_t);
    
    /*
    u8g2Fonts.setFont(u8g2_font_open_iconic_www_4x_t);
    u8g2Fonts.drawGlyph(85, 70, 81); // battery
    
    u8g2Fonts.setFont(u8g2_font_nerhoe_tf);
    u8g2Fonts.setCursor(40, 90);
    u8g2Fonts.print("Please connect to wifi to"); 
    u8g2Fonts.setCursor(60, 105);
    u8g2Fonts.print("display the time.");
    */
   
    if (appState.fullscreenNotif == BATTERY) {
      drawEmptyBattery(95, 25);
      u8g2Fonts.setFont(u8g2_font_nerhoe_tf);
      u8g2Fonts.setCursor(60, 110);
      u8g2Fonts.print("Connect USB-C power to begin charging."); 
    }
    

  } while (display.nextPage());
}



/**
 * Draw the dashboard/UI elements on epaper
 *
 */
void drawSettingsMenu() {

  //display.setFullWindow();
  display.setPartialWindow(0, 0, display.width(), display.height()); //left (x), top (y), width, height
  //Serial.println("drawSettingsMenu()");
  display.firstPage();

  do {

    display.fillScreen(GxEPD_WHITE);

    u8g2Fonts.setFont(u8g2_font_logisoso16_tf);
    u8g2Fonts.setCursor(10, 23);
    u8g2Fonts.print("Settings");
    //display.drawLine(10, 25, display.width()-10, 25, GxEPD_BLACK);
    drawBattery(display.width()-55, 10, battPercent, 0); //battVoltage
    
    for (int i = 0; i < settingsMenuCount; i++) {

      int y = 50 + (i * 20);

      bool selected = (i == appState.selectedMenuIndex && appState.inSettingsMenu);

      drawMenuRow(settingsMenu[i], y, selected);
    }

  } while (display.nextPage());
}



void drawMenuRow(MenuItem item, int y, bool selected) 
{
  
  if(selected) {
    display.fillRoundRect(5, y - 22, display.width()-15, 20, 2, GxEPD_BLACK);
    //display.setTextColor(GxEPD_WHITE);
     u8g2Fonts.setFontMode(0); // solid (draw background)
     u8g2Fonts.setForegroundColor(GxEPD_WHITE);
     u8g2Fonts.setBackgroundColor(GxEPD_BLACK);
  } else {
    //display.setTextColor(GxEPD_BLACK);
     u8g2Fonts.setFontMode(1); // transparent
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
     u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
  }

  u8g2Fonts.setFont(u8g2_font_t0_13_tr); //u8g2_font_luBS08_tf
  u8g2Fonts.setCursor(10, y-8);
  u8g2Fonts.print(item.label);
  //display.setFont(&FreeSansBold9pt7b);
  //display.setCursor(10, y-8);
  //display.print(item.label);

  u8g2Fonts.setFont(u8g2_font_t0_13_tr);
  u8g2Fonts.setCursor(180, y-8);
  u8g2Fonts.print(item.options[*item.value]);
}



void drawEditDialog()
{
  int x = 50;
  int y = 10;
  int w = display.width()-(x*2);
  int h = display.height()-(y*2);

  display.setPartialWindow(x, y, w, h); //left (x), top (y), width, height
  Serial.println("drawEditDialog()");
  display.firstPage();
  do {
    // Dialog box
    display.fillRect(x, y, w, h, GxEPD_WHITE);
    //display.drawRect(25, 10, 240, 100, GxEPD_BLACK);
    display.drawRoundRect(x, y, w, h, 4, GxEPD_BLACK);

    // Title
    u8g2Fonts.setFontMode(0);
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);
    u8g2Fonts.setFont(u8g2_font_t0_13b_tr);

    u8g2Fonts.setCursor(x+10, y+15);
    u8g2Fonts.print(editingItem->label);

    //u8g2Fonts.setFont(u8g2_font_t0_14_tf);
    
    for(int i = 0; i < editingItem->optionCount; i++) {

      bool selected = (i == *editingItem->value);

      drawOptionButton(x+10, (i*23)+30, editingItem->options[i], selected);
      //Serial.println("editnig item " + String(*editingItem->value));
    }
      
    //drawSettingEditValue(appState.selectedMenuIndex);
  } while (display.nextPage());
  
}


void drawOptionButton(int x, int y, const char* label, bool selected)
{
    const int w = 175;
    const int h = 18;
    
    // Selected state
    if(selected) {

        display.fillRoundRect(x, y, w, h, 2, GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);

    } else {
        display.drawRoundRect(x, y, w, h, 2, GxEPD_BLACK);
        display.setTextColor(GxEPD_BLACK);
    }

    // Optional font
    // display.setFont(&FreeMonoBold12pt7b);

    display.setCursor(x+5, y+5);
    display.print(label);
    //Serial.println(label);
    // Reset default color
    //display.setTextColor(GxEPD_BLACK);
}



/**
 * Draws a battery charge level icon
 * @param int x
 * @param int y
 * @param int w
 * @param int h
 * @param int percent - 0-100
 * @param float voltage - battery voltage. 0 will hide the voltage output.
 */
void drawBattery(int x, int y, int percent, float voltage)
{
  uint8_t w = 20;
  uint8_t h = 11;
  // battery outline
  display.drawRoundRect(x, y, w, h, 1, GxEPD_BLACK);
  // nub
  if (percent == 100) {
    display.fillRoundRect(x + w -1, (y + h/4) + 1, 3, h/2, 1, GxEPD_BLACK);
  }
  else {
    display.drawRoundRect(x + w -1, (y + h/4) + 1, 3, h/2, 1, GxEPD_BLACK);
  }

  int fill = (w - 2) * percent / 100;
  display.fillRect(x + 1, y + 1, fill, h - 2, GxEPD_BLACK);
  
  u8g2Fonts.setFont(u8g2_font_samim_12_t_all);
  u8g2Fonts.setCursor(x + w + 6, y + h - 1);
  u8g2Fonts.print(String(percent));
  
  // Display voltage reading if provided
  if (voltage > 0) {
    u8g2Fonts.print("("+String(voltage) + "v)");
  }
}

void drawEmptyBattery(int x, int y)
{
  //int battX = 30;
  //int battY = 40;

  // outline
  display.drawRoundRect(x, y, 100, 55, 5, GxEPD_BLACK);
  display.drawRoundRect(x + 1, y + 1, 98, 53, 4, GxEPD_BLACK);

  //display.drawRoundRect(battX, battY, 100, 55, 5, GxEPD_BLACK);
  //display.drawRoundRect(31, 41, 98, 53, 4, GxEPD_BLACK);
  
  // space between diagonal line
  display.fillRect(x + 50, y - 5, 15, 10, GxEPD_WHITE);
  display.fillRect(x + 32, y + 50, 15, 10, GxEPD_WHITE);

  //display.fillRect(80, 35, 15, 10, GxEPD_WHITE);
  //display.fillRect(62, 90, 15, 10, GxEPD_WHITE);

  // diagonal line
  display.drawLine(x + 60, y - 10, x + 35, y + 65, GxEPD_BLACK);
  display.drawLine(x + 61, y - 10, x + 36, y + 65, GxEPD_BLACK);

  //display.drawLine(90, 30, 65, 105, GxEPD_BLACK);
  //display.drawLine(91, 30, 66, 105, GxEPD_BLACK);

  // bump
  display.drawRoundRect(x + 98, y + 13, 12, 30, 2, GxEPD_BLACK);
  display.drawRoundRect(x + 98, y +14, 11, 28, 2, GxEPD_BLACK);
  
  //display.drawRoundRect(128, 53, 12, 30, 2, GxEPD_BLACK);
  //display.drawRoundRect(128, 54, 11, 28, 2, GxEPD_BLACK);
}