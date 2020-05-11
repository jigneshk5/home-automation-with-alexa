#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "ssid";
const char pass[] = "science@78";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

const int red= D5;
const int green= D6;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("mqtt_client","iot_guy","aio_LXOw37WyVgExbkpB5UTgU1A2cxsk")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("iot_guy/feeds/red-light");
  client.subscribe("iot_guy/feeds/green-light");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if(topic=="iot_guy/feeds/red-light"){
    if(payload=="1")
      digitalWrite(red,HIGH);
    else
      digitalWrite(red,LOW);
  }
  if(topic=="iot_guy/feeds/green-light"){
    if(payload=="1")
      digitalWrite(green,HIGH);
    else
      digitalWrite(green,LOW);
  }

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("io.adafruit.com", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(5000);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
}
