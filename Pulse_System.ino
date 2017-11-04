#include <ESP8266WiFi.h>

// Setup wifi connection
const char* ssid = "AndroidAPM";
const char* password = "1234567890";

// Global variables
int pulse = 0;

void setup_wifi() {
  delay(10);
  
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(D5, OUTPUT);
  //setup_wifi();
}

void loop() {
  pulse = analogRead(A0);
  if(pulse >= 530)
    digitalWrite(D5,HIGH);
  else
    digitalWrite(D5,LOW);
  delay(50);
  Serial.println(pulse);
}
