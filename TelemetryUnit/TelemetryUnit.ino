#include <LoRa.h>
#include "boards.h"
#include <DHT.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <UnixTime.h>

#define DHTPIN 15     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11


//REMEMBER: Define an unique token for each device on Gateway code
//#define BOARD_ID 1  //ID to distinguish multiple sensor boards
#define BOARD_ID 2  //ID to distinguish multiple sensor boards


UnixTime timestamp(3);
uint32_t unix;
String temp_time = "";

DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;

unsigned long counter = 0;
String umidade_str = "";
String temp_str = "";
double latitude = 0.0, longitude = 0.0;

boolean debug = false;
unsigned long lastSend = 0;

void displayInfoGPS()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid()) {
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid()) {
        Serial.print(gps.date.month());
        Serial.print(F("/"));
        Serial.print(gps.date.day());
        Serial.print(F("/"));
        Serial.print(gps.date.year());
        Serial.print("Unix string:");
        Serial.println(temp_time);
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (gps.time.isValid()) {
        if (gps.time.hour() < 10) Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10) Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10) Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10) Serial.print(F("0"));
        Serial.print(gps.time.centisecond());
    } else {
        Serial.print(F("INVALID"));
    }

    Serial.println();
}

String composeJson(){
  StaticJsonDocument<400> data;
  String string;
  //populate JsonFormat
  data["board_id"] = BOARD_ID;
  data["packet_id"] = counter++;
  data["ts"] = temp_time;
  data["temperature"] = temp_str;
  data["humidity"] = umidade_str;
  data["latitude"] = latitude;
  data["longitude"] = longitude;
  

  //copy JsonFormat to string
  serializeJson(data, string);
  if (true){
    Serial.println("INFO: The following string will be send...");
    Serial.println(string);
  }
  return string;
}

void LoRaSend(String packet){
  //turn on ESP32 onboard LED before LoRa send
  digitalWrite(BOARD_LED, HIGH);
  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();
  //turn off ESP32 onboard LED when finished
  digitalWrite(BOARD_LED, LOW);
}

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);
    
    //dht.begin();

    Serial.println("LoRa Sender");
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    LoRa.setSyncWord(0xF3);
    LoRa.enableCrc();
    //latitude = LAT_TEST;
    //longitude = LONG_TEST;
}

void loop()
{
    
    if(millis() - lastSend > TIME_INTERVAL){
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      //int h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      //int t = dht.readTemperature();
      int h = (int)random(37,41);
      int t = (int)random(27,31);
        
      if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      delay(1000);
      return;
      }
      
      while (Serial1.available() > 0){
          if (gps.encode(Serial1.read())){
              latitude = gps.location.lat();
              longitude = gps.location.lng();
              timestamp.setDateTime(gps.date.year(),gps.date.month(),gps.date.day(),gps.time.hour(),gps.time.minute(),gps.time.second());
              unix = timestamp.getUnix();
              temp_time = String(unix) + String(gps.time.centisecond());
              displayInfoGPS();
          }
      }
              
      umidade_str = String(h);
      temp_str = String(t);
      
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.print(F("°C "));
      Serial.print("Sending packet: ");
      Serial.println(counter);

      
      // send packet if data is valid
      //if(isnan(h) || isnan(t) || !gps.time.isValid())
      if(isnan(h) || isnan(t) || !gps.time.isValid() || !gps.location.isValid())
          Serial.println("There is invalid data... Data won't be transmitted");
      else
          LoRaSend(composeJson());

      
      if (u8g2) {
          char buf[256];
          u8g2->clearBuffer();
          //u8g2->drawStr(0, 9, "Transmitting: OK!");
          snprintf(buf, sizeof(buf), "Temp: %s", temp_str);
          u8g2->drawStr(0, 9, buf);
          snprintf(buf, sizeof(buf), "Humidity: %s", umidade_str);
          u8g2->drawStr(0, 20, buf);
          snprintf(buf, sizeof(buf), "Lat: %f", latitude);
          u8g2->drawStr(0, 31, buf);
          snprintf(buf, sizeof(buf), "Long: %f", longitude);
          u8g2->drawStr(0, 42, buf);
          u8g2->sendBuffer();
      }
      
      lastSend = millis();
    }
    delay(1);
}
