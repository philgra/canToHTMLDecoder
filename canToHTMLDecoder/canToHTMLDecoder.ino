/*
  Arduinocode zur Darstellung von CAN-Nachrichten auf einer HTML-Seite mit einem ESP8266.
  Von philgra 2022
  https://github.com/philgra/canToHTMLDecoder

*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <mcp_can.h>
#include <SPI.h>
#include <DNSServer.h>
#include "credentials.h"
#include "htmlCode.h"

//#define DEBUG

//Hier kann zwischen Generierung eines AP oder Verbindung mit einem bestehenden WLAN umgeschaltet werden
#define GEN_AP


#define UPDATE_DELAY_MS 1000
#define MAX_NUMB_OF_IDS 40



#define CAN0_INT 2  // INT in pin 2
MCP_CAN CAN0(15);   // CS in pin 15


ESP8266WebServer server(80); //instantiate server at port 80 (http port)

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

//Soft AP network parameters
IPAddress apIP(172, 20, 0, 1);
IPAddress netMsk(255, 255, 255, 0);


String g_text = "";
String g_html_data;
int g_htmlTestCntr = 0;
int g_numbOfIdsReceived = 0;
bool g_resetRequested = false;


struct canMessage {
  String id = "";
  String dlc = "";
  String data = "";
  long period = 0;
  long count = 0;
  long lastReceivedMs = 0;
};


struct carData {
  String coolantTemp = "";
  bool coolantTempRec = false;

  String engineRpm = "";
  bool engineRpmRec = false;

  String velocity = "";
  bool velocityRec = false;

  String voltage = "";
  bool voltageRec = false;
};



void setup(void) {
  Serial.begin(115200);
  initCan();
  setupNetwork();
}

void loop(void) {
  long lastWebHandling = 0;
  long lastCanRequestSend = 0;
  int numbOfLastCanRequestSend = 0;
  canMessage incomingCanMessages[MAX_NUMB_OF_IDS];
  carData myCarData;
  g_numbOfIdsReceived = 0;
  g_resetRequested = false;


  while (!g_resetRequested)
  {
    if (!digitalRead(CAN0_INT))
    {
      //Neue CAN-Bus Nachricht
      long unsigned int rxId;
      unsigned char len = 0;
      unsigned char rxBuf[8];

      String id_str, dlc_str, data_str = "";


      CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)

      if ((rxId & 0x80000000) == 0x80000000)    // Determine if ID is standard (11 bits) or extended (29 bits)
      {
        id_str = String((rxId & 0x1FFFFFFF), HEX);
      }
      else
      {
        id_str = String(rxId, HEX);
      }
      dlc_str = String(len);
      if ((rxId & 0x40000000) == 0x40000000) {  // Determine if message is a remote request frame.
        data_str = "REMOTE REQUEST FRAME";
      }
      else
      {
        for (byte i = 0; i < len; i++)
        {
          data_str += String(rxBuf[i], HEX) + " ";
        }
      }
      //Serial.println(id_str + " : " + data_str);

      //Wenn CAN-ID = 0x7e8 (ECU) und SID = 41 (Antwort auf Messwertabfrage)

      if (((rxId == 0x7e8)) && (rxBuf[1] == 0x41))
      {
        //PID filtern
        if (rxBuf[2] == 0x05)
        {
          //Kühlwassertemperatur
          //Bereich -40°C bis 215°C
          float A = -40;
          float B = 215;
          myCarData.coolantTemp = String((float)((((float)rxBuf[3] / 256) * (B - A)) + A));
          myCarData.coolantTempRec = true;
          //Serial.println("Kühlwassertemperatur: " + String(myCarData.coolantTemp ));

        }
        if (rxBuf[2] == 0x0C)
        {
          //Motordrehzahl
          //Bereich 0 bis 16383,75 U/min
          myCarData.engineRpm = String((float)((float)(256 * rxBuf[3]) + rxBuf[4]) / 4);
          myCarData.engineRpmRec = true;
          //Serial.println("Motordrehzahl: " + String(myCarData.engineRpm ));
        }
        if (rxBuf[2] == 0x0D)
        {
          //Geschwindigkeit
          //Bereich 0 bis 255 km/h
          float A = 0;
          float B = 255;
          //myCarData.velocity = String((float)((((float)rxBuf[3] / 256) * (B - A)) + A));
          myCarData.velocity = String(rxBuf[3]);
          myCarData.velocityRec = true;
          //Serial.println("Geschwindigkeit: " + String(myCarData.velocity ));
        }
        if (rxBuf[2] == 0x42)
        {
          //Steuermodulspannung
          //Bereich 0 bis 65,535V
          myCarData.voltage = String((float)((float)(256 * rxBuf[3]) + rxBuf[4]) / 1000);
          myCarData.voltageRec = true;
          //Serial.println("Steuermodulspannung: " + String(myCarData.voltage ));
        }
      }


      g_numbOfIdsReceived = canMessageReceived(incomingCanMessages, g_numbOfIdsReceived, setCanMessage(id_str, dlc_str, data_str));



    }
    if ((millis() - lastCanRequestSend) > (UPDATE_DELAY_MS / 5))
    {
      lastCanRequestSend = millis();
      if (numbOfLastCanRequestSend == 0)
      {
        sendRequestToCar(0x05);
        numbOfLastCanRequestSend++;
      }
      else if (numbOfLastCanRequestSend == 1)
      {
        sendRequestToCar(0x0C);
        numbOfLastCanRequestSend++;
      }
      else if (numbOfLastCanRequestSend == 2)
      {
        sendRequestToCar(0x0D);
        numbOfLastCanRequestSend++;
      }
      else if (numbOfLastCanRequestSend == 3)
      {
        sendRequestToCar(0x42);
        numbOfLastCanRequestSend = 0;
      }
    }

    if ((millis() - lastWebHandling ) > UPDATE_DELAY_MS)
    {
      //Web Handling
#ifdef DEBUG
      Serial.print(messagesToString(incomingCanMessages, g_numbOfIdsReceived));
#endif
      g_html_data = canMessagesToHTMLString(incomingCanMessages, g_numbOfIdsReceived);
      g_html_data += carDataToHTMLString(myCarData);
      server.handleClient();
      lastWebHandling = millis();

    }
    server.handleClient();

#ifdef GEN_AP
    dnsServer.processNextRequest();
#endif
  }
}
