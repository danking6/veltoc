#pragma once

// Temperature/humidity sensor BME680
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"

// Declare only (don't define)
extern float temperatureSensor;
extern float humiditySensor;
extern float pressureSensor;

extern float battPercent;
extern float battVoltage;

void readBME280Sensor();
void readVoltageSensor();