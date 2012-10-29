
#include <WiFly.h>
#include "Config.h"
#define LED 12
#define REED 7

String MSG_STARTED = "started";
String MSG_ALIVE = "alive";
String MSG_WIFI_ERROR = "wifi_error";
String MSG_DOOR_OPENED = "door_opened";
String MSG_DOOR_CLOSED = "door_closed";
unsigned long lastAlive;

Client client(host, port);

void setup() {
  
  delay(5000);
  Serial.begin(9600);
  
  pinMode(LED, OUTPUT);      
  digitalWrite(LED, LOW);
  
  pinMode(REED, INPUT);
  
  Serial.println("Starting....");
  Serial.println(LED);
  Serial.println(REED);
  
  WiFly.begin();
  
  Serial.println("wifly begun....");
  
  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    sendMessage(MSG_WIFI_ERROR, NULL);
    while (1) {
      // Hang on failure.
      Serial.println("errored out"); 
      delay(1000);
    }
  }  

  sendMessage(MSG_STARTED, NULL);
 
  lastAlive = millis();
}

int count = 0;
int currentState = 0;

void loop() {

  Serial.print("current state: ");
  Serial.println(currentState);
  
  int doorOpen = digitalRead(REED);
  Serial.print("door open: ");
  Serial.println(doorOpen);
  
  if(doorOpen != currentState) {
    Serial.print("change state from ");
    Serial.print(currentState);
    Serial.print(" to ");
    Serial.println(doorOpen);
    
    if(doorOpen == LOW) {
       sendMessage(MSG_DOOR_CLOSED, NULL); 
       digitalWrite(LED, HIGH);
    } else {
       sendMessage(MSG_DOOR_OPENED, NULL);
       digitalWrite(LED, LOW);
    }
    currentState = doorOpen;
  }
  
  
  
  /* not sure if i need to read this or not */

  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    count++;
    if (count > 80) {
      count = 0;
      Serial.println();
    }
  }
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
  
  delay(500);
}

void sendMessage(String msgType, String msg) {
  String data = "msg="+msg;
  Serial.println("sending message type: "+msgType);
  
  if( client.connect() ) 
  {
    Serial.println( "connected.." );
 
    client.println( "POST /service/"+msgType+" HTTP/1.1" );
    client.print( "Host: ");
    client.println(host);
    client.println( "Content-Type: application/x-www-form-urlencoded" );
    client.println( "Connection: close" );
    client.print( "Content-Length: " );
    client.println( data.length() );
    client.println();
    client.print( data );
    client.println();
  } else {
    Serial.println( "error. not connected.." ); 
  }
  
}

