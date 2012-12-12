
#include <WiFly.h>
#include "Config.h"

/*
 * this version, the arduino uses a normally open magnetic switch,
 * and only turns on when the switch is opened (the door is open)
 * and sends alert after MSG_DOOR_OPEN_ALERT millis
 */

String MSG_WIFI_ERROR = "wifi_error";
String MSG_DOOR_OPEN_ALERT = "door_opened_alert";
String MSG_DOOR_OPEN = "door_opened";
unsigned long lastAlive = 0;
unsigned long DOOR_OPEN_ALERT_INTERVAL = 60000; // 5 minutes 300000
unsigned long DOOR_OPEN_RESEND_INTERVAL = 300000;
unsigned long lastSentAlert = 0;
unsigned int numTimesAlertSent = 0;

Client client(host, port);

void setup() {
  
  delay(1000);
  Serial.begin(9600);
  
  Serial.println("Starting....");
  
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
  sendMessage(MSG_DOOR_OPEN, NULL);
  lastSentAlert = millis();
}



void loop() {

  long now = millis();
  if (now >= DOOR_OPEN_ALERT_INTERVAL && now >= lastSentAlert+(DOOR_OPEN_RESEND_INTERVAL*numTimesAlertSent)) {
    lastSentAlert = now;
    numTimesAlertSent++;
    sendMessage(MSG_DOOR_OPEN_ALERT, NULL);
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

