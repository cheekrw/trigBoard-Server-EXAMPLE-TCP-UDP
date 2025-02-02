// demo server for TCP/UDP Messages

#include <WiFi.h>
// STUFF NEEDED FOR TCP ***********
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
//got these from here
//https://github.com/me-no-dev/ESPAsyncWebServer
//https://github.com/me-no-dev/AsyncTCP
//*********************************

//STUFF NEEDED FOR UDP ************
#include "AsyncUDP.h"
AsyncUDP udp;
//*********************************

// make some text on ili9341*******
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeSansBold24pt7b.h>

#define TFT_DC 15
#define TFT_CS 14

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//*******************************



void setup() {
  Serial.begin(115200);
  // you don't have to be the access point
  WiFi.enableAP(true);
  Serial.println(WiFi.softAP("someSSID", "somePassword", 1, 1, 8)); //ssid,pw,ch,hid,conn  I like to keep hidden
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(ILI9341_WHITE);

  //TCP SETUP AND CALL BACKS ******************
  server.on("/trigBoard", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String message;
    char newPacket[100];
    if (request->hasParam("message")) {// we send data in URL /trigBoard?message=blah blah
      message = request->getParam("message")->value();
      message.toCharArray(newPacket, message.length());
      request->send(200, "text/plain", "OK");
      Serial.printf("TCP: %s\n", newPacket);
    }
  });
  server.begin();
  //*******************************************
  //UDP SETUP AND CALL BACKS ******************
  if (udp.listen(1234)) {// whatever port you want here
    udp.onPacket([](AsyncUDPPacket packet) {
      //      Serial.print("UDP Packet Type: ");
      //      Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      //      Serial.print(", From: ");
      //      Serial.print(packet.remoteIP());
      //      Serial.print(":");
      //      Serial.print(packet.remotePort());
      //      Serial.print(", To: ");
      //      Serial.print(packet.localIP());
      //      Serial.print(":");
      //      Serial.print(packet.localPort());
      //      Serial.print(", Length: ");
      //      Serial.print(packet.length());
      //      Serial.print(", Data: ");
      //Serial.print("UDP: ");
      Serial.write(packet.data(), packet.length());
      Serial.println("");
      String T_data = (char*)(packet.data());
      int comma_location = T_data.indexOf(',');
      tft.fillScreen(ILI9341_BLACK);
      tft.setCursor(90,220);
      tft.setFont(&FreeSansBold24pt7b);
      tft.setTextSize(2);
      tft.print(T_data.substring(0, comma_location));
      tft.setFont();
      tft.setCursor(0,0);
      tft.print(T_data.substring(comma_location + 1));
  
      //and could send back as well
      WiFiUDP udpTX;
      IPAddress broadcastIP(255, 255, 255, 255);
      if (udpTX.beginPacket(broadcastIP , 1234) == 1) {
        udpTX.print("hello world");
        udpTX.endPacket();
      }
      
    });
  }
  //*******************************************
}

void loop() {

}

//unsigned long testText(String temp, String batt) {
  unsigned long testText(String T) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(25,100);

  tft.println(T);
//  tft.setFont();
//  tft.println(batt);
  }
