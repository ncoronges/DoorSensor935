
#include <WiFly.h>
#include "Config.h"
#define LED 12
#define REED 7

String MSG_STARTED = "started";
String MSG_ALIVE = "alive";
String MSG_WIFI_ERROR = "wifi_error";
String MSG_DOOR_OPENED = "door_opened";
String MSG_DOOR_CLOSED = "door_closed";
String MSG_DOOR_OPEN_ALERT = "door_opened_alert";
unsigned long lastAlive = 0;
unsigned long ALIVE_MSG_INTERVAL = 30000; // 5 minutes 300000
unsigned long DOOR_OPEN_ALERT_INTERVAL = 30000; // 5 minutes 300000
unsigned long lastDoorOpened = 0;

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
 
}

int count = 0;
int currentState = 0;

void loop() {

  long now = millis();
  //Serial.print("current state: ");
  //Serial.println(currentState);
  
  int doorOpen = digitalRead(REED);
  //Serial.print("door open: ");
  //Serial.println(doorOpen);
  
  if(doorOpen != currentState) {
    Serial.print("change state from ");
    Serial.print(currentState);
    Serial.print(" to ");
    Serial.println(doorOpen);
    
    if(doorOpen == LOW) {
       sendMessage(MSG_DOOR_CLOSED, NULL); 
       digitalWrite(LED, HIGH);
       lastDoorOpened = 0;
    } else {
       sendMessage(MSG_DOOR_OPENED, NULL);
       digitalWrite(LED, LOW);
       lastDoorOpened = now;
    }
    currentState = doorOpen;
  }
  
  
  /*
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
  }*/
  
  // send alive every 5 minutes
  if (now >= lastAlive + ALIVE_MSG_INTERVAL) {
    sendMessage(MSG_ALIVE, NULL); 
    lastAlive = now;
  } else if (now < lastAlive) {
    // after 50 days arduino resets millis() to zero
    lastAlive = now;  
  }
  
  // send door opened alert 
  if (lastDoorOpened != 0) {
    if (now > lastDoorOpened + DOOR_OPEN_ALERT_INTERVAL) {
      sendMessage(MSG_DOOR_OPEN_ALERT, NULL); 
      lastDoorOpened = now;  
    }
  }
  
  delay(10);
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
  delay(500);
  // Read http client stuff
  while (client.available()) {
      // TODO verify success (HTTP/1.1 200 OK)
      Serial.write(client.read());  // display the result
  }
  Serial.println();
  delay(10);
  if (client.connected()) {
      Serial.println("disconnecting.");
      client.stop();
      Serial.println("disconnected.");
  }
  delay(500);
}

