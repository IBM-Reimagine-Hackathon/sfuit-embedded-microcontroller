# SfUIT - ESP32

![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white) ![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white) ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)



### Before getting started, you need to set up your Arduino IDE with ESP32 environment.

## Rough Circuit Diagram
![image](https://drive.google.com/uc?export=view&id=1YMcwOw9ATWgJct5lqMdza1ZCHyepUTv2)

The repositories we have used are:
* SfUIT_IBM_Connect_IOT.ino
* HTML.h
* WifiManager.h

## WiFi Manager for ESP32

### Usage:
* Connect to Access Point with SSID: ESP32 WiFi Manager, Key: 12345678
* Go to 192.168.4.1 on preferred browser, enter router's WiFi SSID and Key
* Click Save, device will restart and store WiFi creds to EEPROM

## Modules
* setWifiCredentialsInEEPROM()
* onBeatDetected()
* setPulseOximeter()
* getTempFunc()
* printPulseOxiValues()
* loadCredFromEEPROM()
* publishPayload()
* wipeEEPROM()

## Workflow
* Dynamic Wifi and Pulse Oximeter connections are established in `setup()`.
* Wifi is instantiated and values are retrieved, updated in `loop()`.
### In setup()
* `setWifiCredentialsInEEPROM()` checks if there exists any Wifi Credentials in EEPROM and establishes the Wifi connectivity.
* `setPulseOximeter()` is used for setting up the conenction of ESP32 with Pulse-Oximeter `(MAX30100/30102)`
### In loop()
* `onBeatDetected()` retrieves analog values from Pulse-Oximeter.
* `getTempFunc()` retrieves analog values from Waterproof Temperature sensor `(DS18B20)`.
* `printPulseOxiValues()` prints the Pulse-Oximeter values.
* `loadCredFromEEPROM()` loads the Wifi Credentials from EEPROM.
* `publishPayload()` publishes the values from the Payload to the `IBM-Watson Cloud`.
* `wipeEEPROM()` wipes the details stored in the EEPROM.


## Dependencies
* Wifi.h
* Wire.h
* MAX30100_PulseOximeter.h
* PubSubClient.h
* WiFiClient.h
* EEPROM.h
* HTML.h
* WebServer.h
* WiFiManager.h

## Technology Stack
* Arduino
* C , C++
