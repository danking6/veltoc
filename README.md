# Veltoc
Veltoc Model 01 is a ESP32 powered, retro-inspired environmental monitor with an E-Ink display. It can monitor temperature, humidity and air pressure. Connect to wifi to also display the current time.

<img width="4032" height="3265" alt="Veltoc-model-01 copy" src="https://github.com/user-attachments/assets/abb05084-31d4-4e59-87b8-da9ddf925d28" />

# Hardware Used

- <a href="https://www.seeedstudio.com/ePaper-breakout-Board-for-XIAO-V2-p-6374.html">ePaper Driver Board for Seeed Studio XIAO</a>
- <a href="https://www.seeedstudio.com/2-9-Monochrome-ePaper-Display-with-296x128-Pixels-p-5782.html">2.9" Monochrome eInk / ePaper Display</a>
- <a href="https://www.amazon.com/dp/B08728K3YB">Rotary Encoder</a>
- 3mm 3.7v LED
- BME 280 Temperature/Humidity/Pressure sensor
- 2x 100kOhm resistors (for battery level reading)
- <a href="https://www.amazon.com/dp/B095W4HS75">EEMB LiPo Battery 3.7V 1400mAh 112945 Rechargeable Battery</a>

# Wiring For Xiao ESP32 C6
These are all configurable in the config.h file if using a different ESP32 or pins.
BME280 Sensor
SCL -> D5
SDA -> D4

LED -> D6

Rotary Encoder
Button -> GPIO6 (MTCK pin on bottom) (wake)
Rotation -> D7, D9

Battery voltage sensor
A5 (MTDI pin on bottom)

Note: The battery was soldered directly to the underside battery +/- pins on the Xiao ESP32, as when using the JST connector on the epaper driver board, the battery wouldn't charge.

# Setup

Arduino Studio was used. Please download the necessary libraries. If using wifi (needed for accurate time), add wifi credentials in voltoc-model-01.ino. Adjust other pins as needed in config.h
