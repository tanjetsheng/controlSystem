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

#define WAIT 1000

byte board;
byte mod;
byte size;
byte act;
byte send[4];

byte R_board;
byte R_mod;
byte R_size;
byte R_act;

byte r_board;
byte r_mod;
byte r_size;
byte r_act;

int BoardTotal = 0;
int brokeBoard =0;
int brokeBoardNum[10];
int checkNum = 0;
char buff[50];
char buff1[20];



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


void print(){
  sprintf(buff ,"%x , %x ,%x, %x",R_board,R_mod,R_size,R_act);
}

void print1(){
  sprintf(buff1 ,"%x , %x ,%x, %x",r_board,r_mod,r_size,r_act);
}

void read(){
  if (Serial.available() > 0) {
    R_board = Serial.read();
  }
  if (Serial.available() > 0) {
    R_mod = Serial.read();
  }
  if (Serial.available() > 0) {
    R_size = Serial.read();
  }
  if (Serial.available() > 0) {
    R_act = Serial.read();
  }
}

void read1(){
  if (Serial.available() > 0) {
    r_board = Serial.read();
  }
  if (Serial.available() > 0) {
    r_mod = Serial.read();
  }
  if (Serial.available() > 0) {
    r_size = Serial.read();
  }
  if (Serial.available() > 0) {
    r_act = Serial.read();
  }
}

void sendAndRead(){
  R_act = 0;
  putInArray();
  delay(WAIT);
  read1();
  delay(WAIT);
  putInArray();
  delay(WAIT);
  read();
  delay(WAIT);
}

void validate(){
  board = 1;
  mod = 0;
  act = 0xc;
  BoardTotal = 0;
  brokeBoard = 0;
  int num = 0;
  memset(brokeBoardNum, 0 , 10);
  for(int i=1; i<=3 ;i++){
    read();
    sendAndRead();
    if(R_act == 1){
      BoardTotal++;
    }
    else if(R_act == 0){
      brokeBoard++;
      brokeBoardNum[num] = board;
      num++;
    }
    board++;
  }
}

void rearrange(){
  int a = 0;
  for(int i = 0;i<brokeBoard;i++){
    if(brokeBoardNum[a] <= board){
      board = board + 1;
      a++;
    }
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
        sendAndRead();
        print();
        print1();
        client.println(buff1);
        client.println(buff);
         if(R_act == 0x0A)
             client.println("on");
         else
             client.println("off");

    }
    if (ClientRequest == "off") {
        act = 0xb;
        sendAndRead();
        print();
        print1();
        client.println(buff1);
        client.println(buff);
        if(R_act == 0x0A)
            client.println("on");
        else
          client.println("off");
    }
    if (ClientRequest == "Read") {
        act = 0xc;
        sendAndRead();
        if(R_act == 0x0A)
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
        validate();
        print();
        //client.println(buff);
      //  client.println(BoardTotal);
    //  client.println(brokeBoardNum[1]);
        client.println(2);
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
      board = 1;
//      rearrange();
      DEBUG_PRINT(size);
    }
    if (ClientRequest == "2"){
      board = 2;
//      rearrange();
      DEBUG_PRINT(size);
    }
    if (ClientRequest == "3"){
      board = 3;
//    rearrange();
      DEBUG_PRINT(size);
    }
    //-----------------------------------//



    //client.println(2);
    client.stop();
    delay(1);
    client.flush();

}
