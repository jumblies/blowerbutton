/*
  MQTT Button for activating Sonoff 3 relay.
  Geoffrey Lamke,MD
  20171126



*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"  //Connection Info stored in a separate file to make it GIT friendly.  

//Pushbutton and LED indicator definitions
const int btnPin = D4; //Input Pin for Pushbutton
const int ledPin =  LED_BUILTIN;      // the number of the LED pin
int buttonState = 0;
int lastButtonState = 0;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(btnPin, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("BlowerButton")) {
//      Serial.println(
      Serial.println("connected");
      // Once connected, publish an announcement...
      //      client.publish(stateTopic, "hello world I'm connected");  //This is super buggy...it loops and publishes a crapload of messages.
      // ... and resubscribe
      client.subscribe(commandTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  buttonState = digitalRead(btnPin);
  long now = millis();
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      // turn LED on:
      digitalWrite(ledPin, LOW);
      Serial.println("LOW"); //Debug
      client.publish(commandTopic, "ON");
      //      client.publish(stateTopic, "ON");
      lastButtonState = buttonState;
    }
    else {
      digitalWrite(ledPin, HIGH);
      Serial.println("HIGH"); //Debug
      client.publish(commandTopic, "OFF");
      //      client.publish(stateTopic, "OFF");
      lastButtonState = HIGH;

    }
  }
}
