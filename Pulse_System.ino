#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Setup wifi connection
const char* ssid = "AndroidAPM";
const char* password = "1234567890";

// MQTT Server
const char* server = "192.168.43.186";
const int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Global variables
int pulse = 0;

// Constants
const int blinkLed = D5;
const int peak = 550;

void setup_wifi() {
  delay(10);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),"disconnection",1,false,"")) {
      client.subscribe("id-esp/pulse-sensor");
    } else {
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char rateStr[length];
  for(int i = 0;i < length;i++)
    rateStr[i] = (char)payload[i];
  int rate = atoi(rateStr);
  Serial.println(rate);
  delay(10);
}

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(blinkLed, OUTPUT);
  setup_wifi();
  client.setServer(server, port);
  client.setCallback(callback);
}

void loop() {
  /*pulse = analogRead(A0);
  Serial.println(pulse);
  if(pulse >= peak){
    digitalWrite(blinkLed,HIGH);
  }else
    digitalWrite(blinkLed,LOW);
  delay(10);*/
  if (!client.connected()) {
    reconnect();
  } else {
    pulse = analogRead(A0);
    char str[4];
    sprintf(str,"%d",pulse);
    client.publish("id-esp/pulse-sensor",str);
  }
  client.loop();
}
