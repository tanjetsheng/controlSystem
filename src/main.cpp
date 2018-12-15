#include <FS.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <RingBufCPP.h>
#include "config.h"
//#define DEBUG
#include "DebugUtils.h"

byte board;
byte mod;
byte size;
byte act;
byte send[4];

byte R_board;
byte R_mod;
byte R_size;
byte R_act;

enum receiveData {
  BOARD,
  MOD,
  SIZE,
  ACT
};
receiveData receiveState = BOARD;

String  ClientRequest;
WiFiServer server(80);

WiFiClient client;

String myresultat;

String ReadIncomingRequest(){
while(client.available()) {
ClientRequest = (client.readStringUntil('\r'));
 if ((ClientRequest.indexOf("HTTP/1.1")>0)&&(ClientRequest.indexOf("/favicon.ico")<0)){
myresultat = ClientRequest;
}
}
return myresultat;
}

void setup()
{
  board = 0;
  mod = 0;
  size = 1;
  act = 0;

  ClientRequest = "";

  Serial.begin(9600);

  WiFi.disconnect();
  delay(3000);
  DEBUG_PRINT("START");
  WiFi.begin("DMC2","pv121717");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");

  }
  DEBUG_PRINT("Connected");
  DEBUG_PRINT("Your IP is");
  DEBUG_PRINT((WiFi.localIP()));
  server.begin();

}

void putInArray(){
  send[0] = board;
  send[1] = mod;
  send[2] = size;
  send[3] = act;
  DEBUG_PRINT(send[0]);
  DEBUG_PRINT(send[1]);
  DEBUG_PRINT(send[2]);
  DEBUG_PRINT(send[3]);
  Serial.write(send,4);
}

void getData(){
  receiveState = BOARD;
  switch(receiveState){
    case BOARD:
    if (Serial.available() > 0) {
      R_board = Serial.read();
      receiveState = MOD;
    }
    break;
    case MOD:
    if (Serial.available() > 0) {
      R_mod = Serial.read();
      receiveState = SIZE;
    }
    break;
    case SIZE:
    if (Serial.available() > 0) {
      R_size = Serial.read();
      receiveState = ACT;
    }
    break;
    case ACT:
    if (Serial.available() > 0) {
      R_act = Serial.read();
    }
    break;
  }
}
void loop()
{

    client = server.available();
    if (!client) { return; }
    while(!client.available()){  delay(1); }
    ClientRequest = (ReadIncomingRequest());
    ClientRequest.remove(0, 5);
    ClientRequest.remove(ClientRequest.length()-9,9);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    //---------------action--------------------//
    if (ClientRequest == "on") {
        act = 0xa;
        putInArray();
        getData();
        if(R_act == 1)
            client.println("on");
        else
          client.println("off");

    }
    if (ClientRequest == "off") {
        act = 0xb;
        putInArray();
        getData();
        if(R_act == 1)
            client.println("on");
        else
          client.println("off");
    }
    if (ClientRequest == "Read") {
        act = 0x02;
        putInArray();
        getData();
        if(R_act == 1)
            client.println("on");
        else
          client.println("off");
    }
    //-----------------------------------------//
    //-------------mode-----------------------//
    if (ClientRequest == "check") {
        mod = 0x0;
        board = 0;
        act = 0;
        DEBUG_PRINT("check");
        putInArray();
        delay(250);
        putInArray();
        getData();
      //  client.println(2);
        client.println(R_board);
    }
    if (ClientRequest == "Buzz") {
        mod = 0x01;
        DEBUG_PRINT("BUzz");
    }
    if (ClientRequest == "Lock") {
        mod = 0x02;
        DEBUG_PRINT("Lock");
    }
    if (ClientRequest == "Alarm") {
        mod = 0x03;
        DEBUG_PRINT("Alarm");
    }
    //-------------------------------------//
    //-------------size----------------------//
    if (ClientRequest == "1"){
      board = 0x01;
      DEBUG_PRINT(size);
    }
    if (ClientRequest == "2"){
      board = 0x02;
      DEBUG_PRINT(size);
    }
    if (ClientRequest == "3"){
      board = 0x03;
      DEBUG_PRINT(size);
    }
    //-----------------------------------//



    //client.println(2);
    client.stop();
    delay(1);
    client.flush();

}
