#include "config.h"
#include "sensors.h"

float temperatureSensor = 0.0;
float humiditySensor = 0.0;
float pressureSensor = 0.0;

float battPercent = 0.0;
float battVoltage = 0.0;

Adafruit_BME280 bme; // I2C

/**
 * Read from sensor BME280
 *
 */
void readBME280Sensor()
{
   if (!bme.begin(0x76)) {
    Serial.println(F("BME280 not found"));
    //while (1);
    return;
  }

  delay(200); 
  
  temperatureSensor = bme.readTemperature() + TEMPERATURE_OFFSET;
  humiditySensor = bme.readHumidity() + HUMIDITY_OFFSET;
  pressureSensor = bme.readPressure() / 100.0F;
}

/**
 *
 *
 */
void readVoltageSensor()
{
  Serial.print("Measuring battery");
  // Battery voltage reading
  pinMode(BATTERY_ADC_PIN, INPUT); 
  delay(50);
  
  uint32_t Vbatt = 0;
  for(int i = 0; i < 16; i++) {
    Vbatt = Vbatt + analogReadMilliVolts(BATTERY_ADC_PIN); // ADC with correction   
  }
  
  battVoltage = 2 * Vbatt / 16 / 1000.0; 
  float voltage = battVoltage;
  
  if (voltage < 0) voltage = 0; // Prevent negative voltage

  if (voltage < 2.75) {
    battVoltage = 0.0;
    battPercent = 0.0;
    Serial.printf(", voltage: %.2fV (too low or not connected)\n", voltage);
  } else {
    // Calculate battery percentage from emaVoltage
    // Lookup table algorithm: precise mapping of lithium battery discharge curve
    float localBatteryPercentage;
    if (battVoltage >= 4.15f) localBatteryPercentage = 100.0f;
    else if (battVoltage >= 4.0f) localBatteryPercentage = 96.3f + (battVoltage - 4.0f) * 24.5f;
    else if (battVoltage >= 3.7f) localBatteryPercentage = 53.3f + (battVoltage - 3.7f) * 143.5f;
    else if (battVoltage >= 3.5f) localBatteryPercentage = 6.1f + (battVoltage - 3.5f) * 235.7f;
    else if (battVoltage >= 3.0f) localBatteryPercentage = (battVoltage - 3.0f) * 12.3f;
    else localBatteryPercentage = 0.0f;

    // Clamp to 0-100 range
    battPercent = constrain(localBatteryPercentage, 0.0f, 100.0f);
    
    Serial.printf(", voltage: %.2fv (%.2f%%)\n", battVoltage, battPercent);
  }
}