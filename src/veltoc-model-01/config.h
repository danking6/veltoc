// Pins, constants, settings
#pragma once



// Rotary encoder
#define ROT_ENC_BTN GPIO_NUM_6
#define ROT_ENC_B GPIO_NUM_20
#define ROT_ENC_A GPIO_NUM_17

#define BUTTON_PIN_BITMASK (1ULL << GPIO_NUM_6) // GPIO bitmask for ext1 wakeup

// Pins for epaper
#define EPD_CS   D1
#define EPD_DC   D3
#define EPD_RST  D0
#define EPD_BUSY D2
// Pins need to be connected as follows for epaper
//#define EPD_SCK  D8 //CLK
//#define EPD_MOSI D10 //DIN

// Battery
#define BATTERY_ADC_PIN 5

// LED
#define ACTIVITY_LED_PIN D6

//#define TIME_TO_SLEEP 30      //Seconds of inactivity before sleep

// Adjust as needed for temp/hum accuracy
#define TEMPERATURE_OFFSET 0
#define HUMIDITY_OFFSET 0



