#include <AsyncEventSource.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>

/*
    ESP32Ping from:
    https://github.com/marian-craciunescu/ESP32Ping
    Other Libriaries from the library manager
*/

#include <SPIFFS.h>
#include <WiFiSettings.h>
#include <ESP32Ping.h>
#include <WiFi.h>
#include "time.h"
//#include <ESPmDNS.h>

AsyncWebServer server(81);

const char* ntpServer = "pool.ntp.org";
//const long  gmtOffset_sec = 0;
//const int   daylightOffset_sec = 3600;
#define ONBOARD_LED  2

time_t now;

void notFound(AsyncWebServerRequest *request) {
  //Handle 404
  request->send(404, "text/plain", "Not found");
}

String toStringAddZero(int data)
{
  String st = "";
  if (data < 10)
  {
    st = "0" + String(data);
  }
  else
  {
    st = String(data);
  }
  return st;
}

void setup() {
    Serial.begin(115200);
    //SPIFFS.format();
    SPIFFS.begin(true);  // Will format on the first run after failing to mount

    // Use stored credentials to connect to your WiFi access point.
    // If no credentials are stored or if the access point is out of reach,
    // an access point will be started with a captive portal to configure WiFi.
    WiFiSettings.connect();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //Send respones from SPIFFS
    request->send(SPIFFS, "/responses.csv");
  });
  server.onNotFound(notFound);

  server.begin();
  configTime(-5.00, 0, ntpServer);
    while (now < 1546300800)
  {
    now = time(nullptr);
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
//  if(!MDNS.begin("net-mon-32")) {
//    Serial.println("Error starting mDNS");
//     return;
//  }
  pinMode(ONBOARD_LED,OUTPUT);
  
}



void loop() {
  struct tm *timeinfo;

  time(&now);
  timeinfo = localtime(&now);

  int hour = timeinfo->tm_hour;
  int mins = timeinfo->tm_min;
  int sec = timeinfo->tm_sec;
 
  File file = SPIFFS.open("/responses.csv", FILE_APPEND);

  bool ret = Ping.ping(WiFi.gatewayIP());
  float avg_time_ms = Ping.averageTime();
  Serial.println("----------Local Gateway----------");
  Serial.println("Time = " + toStringAddZero(hour) + ":" + toStringAddZero(mins) + ":" + toStringAddZero(sec));
  Serial.print("Ping: ");
  Serial.println(ret);
  Serial.print("AVG Resp: ");
  Serial.println(avg_time_ms);  
  file.print(toStringAddZero(hour) + ":" + toStringAddZero(mins) + ":" + toStringAddZero(sec)+", ");
  file.print("Gateway, ");
  file.print(ret);
  file.print(", ");
  file.println(avg_time_ms);
  if(!ret){
    digitalWrite(ONBOARD_LED,LOW);
  }else{
    digitalWrite(ONBOARD_LED,HIGH);
  }
  time(&now);
  timeinfo = localtime(&now);

  hour = timeinfo->tm_hour;
  mins = timeinfo->tm_min;
  sec = timeinfo->tm_sec;

  IPAddress ip (75, 75, 75, 75); // The remote ip to ping
  ret = Ping.ping(WiFi.gatewayIP());
  avg_time_ms = Ping.averageTime();
  Serial.println("----------75.75.75.75----------");
  Serial.println("Time = " + toStringAddZero(hour) + ":" + toStringAddZero(mins) + ":" + toStringAddZero(sec));
  Serial.print("Ping: ");
  Serial.println(ret);
  Serial.print("AVG Resp: ");
  Serial.println(avg_time_ms);
  file.print(toStringAddZero(hour) + ":" + toStringAddZero(mins) + ":" + toStringAddZero(sec)+", ");
  file.print("75.75.75.75, ");
  file.print(ret);
  file.print(", ");
  file.println(avg_time_ms);
  file.close();
    if(!ret){
    digitalWrite(ONBOARD_LED,LOW);
  }else{
    digitalWrite(ONBOARD_LED,HIGH);
  }
}
