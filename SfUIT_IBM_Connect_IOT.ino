
#include <WiFi.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include "EEPROM.h"
#include "HTML.h"
#include <WebServer.h>

WebServer server(80);
#include "WiFiManager.h"

#define REPORTING_PERIOD_MS    1000
#define ORG "*****" 
#define DEVICE_TYPE "ESP32" 
#define DEVICE_ID "105BFAA3C9C8" 
#define TOKEN "GwwWtTFismxcOk6sX&" 
#define DS18B20_Sensor 35
#define ADC_VREF_mV    3300.0 // in millivolt
#define ADC_RESOLUTION 4096.0

float BPM, SpO2, milliVolt, tempC, tempF;
long lastMsg = 0;
int adcVal;
String payload;

const char* ssid = "Prateek";  // Enter SSID here
const char* password = "nencheppa";  //Enter Password here
PulseOximeter pox;
uint32_t tsLastReport = 0;

char server_name[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/status1/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
WiFiClient wifiClient;
PubSubClient client(server_name, 1883, NULL, wifiClient);

//Setting Wifi Credentials in EEPROM
void setWifiCredentialsInEEPROM()
{
  //Checking if there exist any wifi credentials
  if(!CheckWIFICreds()){
    Serial.println("No WIFI credentials stored in memory. Loading form...");
    digitalWrite(2,HIGH);
    while(loadWIFICredsForm());
  }
  //If there are no existing credentials 
  else{
  SetWifiSSID().toCharArray(ssid, 50);
  SetWifiPassword().toCharArray(pass, 50);
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  if(WiFi.status() == WL_CONNECTED){
  Serial.println("Connected to WiFi");
  }
  else{
    Serial.println("Not Connected"); 
   }
  }
}

//Printing if pulse is detected
void onBeatDetected()
{
  Serial.println("Beat Detected!");
}

void setPulseOximeter()
{

  // Setting up the Pulse Oximeter
  Serial.print("Initializing pulse oximeter..");
  
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
    
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
  
   pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void getTempFunc()
{
          //Reading analog values from DS18B20 waterproof temperature sensor  
           adcVal = analogRead(DS18B20_Sensor);
          // convert the ADC value to voltage in millivolt
           milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
          // convert the voltage to the temperature in Celsius
           tempC = milliVolt / 10;
          // converting Temperature Celcius to Fahrenheit
           tempF = 32+((tempC*9)/5);
           Serial.print("Temparature (F): ");
           Serial.println(tempF);
}

void printPulseOxiValues()
{
    Serial.print("BPM: ");
    Serial.println(BPM);

    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");

    Serial.println("*********************************");
    Serial.println();
}

void loadCredFromEEPROM()
{
    if(digitalRead(15) == HIGH){
    Serial.println("Wiping WiFi credentials from memory...");
    wipeEEPROM();
    while(loadWIFICredsForm());
    }
    digitalWrite(2,HIGH);
    digitalWrite(2,LOW);
}

void publishPayload()
{
        payload = "{\"data\":{\"device_id\":\"" DEVICE_ID "\"";
        payload += ",\"Pulse\":";
        payload += BPM;
        payload += ",\"SpO2\":";
        payload += SpO2;
        payload += ",\"temperature\":";
        payload += tempF;
        payload += "}}";
}

void wipeEEPROM(){
  for(int i=0;i<400;i++){
    EEPROM.writeByte(i,0);
  }
  EEPROM.commit();
}

void setup() {

  //for resetting WiFi creds
  pinMode(2, OUTPUT);
  pinMode(15,INPUT); 
  EEPROM.begin(400);
  wipeEEPROM();     //Clearing the EEPROM before setting wifi credentials 
  
  //Setting the pinModes of sensors
  Serial.begin(115200);
  pinMode(DS18B20_Sensor, INPUT);
  pinMode(19, OUTPUT);

  //Establishing connection to Dynamic Wifi  
  setWifiCredentialsInEEPROM();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");


  //Establishing connection to IBM Watson Cloud
  if (!client.connected()) {
        Serial.print("Reconnecting client to ");
        Serial.println(server_name);
        while (!client.connect(clientId, authMethod, token)) {
            Serial.print(".");
        }
        Serial.println("Bluemix connected");
    }

  //Function call for Setting Pulse Oximeter
  setPulseOximeter();
}


void loop() {

  //Loading Wifi Credentials from EEPROM
  loadCredFromEEPROM();

  // Initializing/ Updating the values of BPM, SpO2 from sensor 
  
  long now = millis();
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();

  if ((millis() - tsLastReport > REPORTING_PERIOD_MS) && (now - lastMsg > 1000)) //Operating sensor values for every second
  {
    //Retrieving current time in millis
    lastMsg = now;

    //Printing Pulse and Oxygen Level values
    printPulseOxiValues();

    //Getting Body Temperature value
    getTempFunc();


    //Retrieving current time in millis
    tsLastReport = millis();

    //Publising Data to Cloud
    publishPayload();

    //Checking if Data is published 
    if (client.publish(pubTopic1, (char*) payload.c_str())) {
       Serial.println("Publish ok");
     } 
     else {
       Serial.println("Publish failed");
     }
  }

}
