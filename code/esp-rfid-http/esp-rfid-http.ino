#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h> // communication between ESP and RFID reader
#include "SparkFun_UHF_RFID_Reader.h" //Library for controlling the M6E Nano module
#include "util.h"
#include "WiFi_Config.h"
#include "RFID_Config.h"

#include <elapsedMillis.h> //https://playground.arduino.cc/Code/ElapsedMillis
uint16_t interval = 15000;

//To enable debugging for RFID, set to 1
#define RFID_DEBUGGING 0

//GE-100 default network
char ssid[] = "VU-Media";
char password[] = "";  // unused for unencrypted connection

// instances of the WiFi and network drivers
WiFiClient client;
HTTPClient http;


void setup()
{
  board_init();  // for the functions in util.h

  Serial.begin(115200);  // Serial monitor must match this rate

  while (!Serial);  //wait until open

  //Initialize RFID

  //Initialize WiFi
  wifiInit(ssid); //Since VU-Media is unencrypted, a password is not needed
  
  status(GOOD);
}


void loop()
{
  //Serial.println(F("Press a key to scan for a tag"));
  //while (!Serial.available()); //Wait for user to send a character
  //Serial.read(); //Throw away the user's character

  byte myEPC[12]; //Most EPCs are 12 bytes
  byte myEPClength;
  byte responseType = 0;
  byte count = 0;

  while (responseType != RESPONSE_SUCCESS)//RESPONSE_IS_TAGFOUND
  {
    myEPClength = sizeof(myEPC); //Length of EPC is modified each time .readTagEPC is called

    responseType = nano.readTagEPC(myEPC, myEPClength, 500); //Scan for a new tag up to 500ms
    
    if (count == 0){
      Serial.println(F("Searching for tag"));
      count++;
    }
    
    //Serial.println(count); //For debugging purposes

    delay(500);
  }

  count = 0; //Reset count for RESPONSE while loop
  
  //Beep! Piano keys to frequencies: http://www.sengpielaudio.com/KeyboardAndFrequencies.gif
  tone(BUZZER1, 2093, 150); //C
  delay(150);
  tone(BUZZER1, 2349, 150); //D
  delay(150);
  tone(BUZZER1, 2637, 150); //E
  delay(150);

  //Print EPC
  Serial.print(F(" epc["));
  for (byte x = 0 ; x < myEPClength ; x++)
  {
    if (myEPC[x] < 0x10) Serial.print(F("0"));
    Serial.print(myEPC[x], HEX);
    Serial.print(F(" "));
  }
  Serial.println(F("]"));


  const char* host = "api.thingspeak.com";
  const char* streamID = "ZGUH9PMYN82B6TY4";
  // Example: GET https://api.thingspeak.com/update?api_key=8X26QMKV55Q1LTDK&field1=

  String url = "http://";
  url += host;
  url += "/update?api_key=";
  url += streamID;
  url += "&field1=";
  for (byte x = 0 ; x < myEPClength; x++){
    if (myEPC[x] < 0x10){
      url += "0";
      //Serial.print(F("0"));
    }
    url += String(myEPC[x], HEX);
  }

  Serial.println(url);
  
  http.begin(url);
  //TODO: Test if EPC is a registered code by viewing the HTTP response code https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
  //  1XX Informational Response
  //  2XX Success
  //  3XX Redirect
  //  4XX Client Errors
  //  5XX Server Errors
  int httpCode = http.GET();
  String response = http.getString();
  http.end();
  Serial.print("HTTP response code: ");
  Serial.println(httpCode);
  Serial.println(response); 

  responseType = 0; //Reset responseType
  elapsedMillis timeElapsed;
  while (timeElapsed < interval) { //While time elapsed is less than 15 seconds
    responseType = nano.readTagEPC(myEPC, myEPClength, 500); //Scan for a new tag up to 500ms
    if (responseType == RESPONSE_SUCCESS) {
      
      //Set Status
      status(ERROR);
    
      //Fail Tone
      tone(BUZZER1, 660, 150); //E
      delay(150);
      tone(BUZZER1, 622, 150); //D#
     
      Serial.println(F("Data did not go through!"));
      Serial.println(F("Please wait 15 seconds between scans."));

      delay(500); //Give the user time to pull their tag away before scanning
      
      responseType = 0; //Reset Response
      timeElapsed = 0;
     }
  }
  //Ready to scan
  tone(BUZZER1, 1568, 150); //G
  delay(150);
  tone(BUZZER1, 1661, 150); //G#
  Serial.println("Ready to scan!");
  timeElapsed = 0; //Reset the elapsed time

  status(GOOD);

  
}


