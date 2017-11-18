#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>

#define UpperThreshold 550
#define LowerThreshold 500

#define OLED_Address 0x3C

// Setup wifi connection
const char* ssid = "AndroidAPM";
const char* password = "1234567890";

// MQTT Server
const char* server = "192.168.43.186";
const char* token = "WUUYXIGsRChZP5SzFeFN";
const int port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_SSD1306 oled(3);

// Global variables
int x = 0;
int lastX = 0;
int lastY = 0;
int lastTime = 0;
bool BPMTiming = false;
bool beatComplete = false;
int BPM = 0;

// Constants
const int pinBuzzer = 13;

void setup_wifi() {
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  oled.clearDisplay();
  digitalWrite(LED_BUILTIN, LOW);
}

void reconnect() {
  while (!client.connected()) {
    if ( WiFi.status() != WL_CONNECTED) {
      oled.clearDisplay();
      digitalWrite(LED_BUILTIN, HIGH);
      setup_wifi();
    }
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(),token,NULL)) {
      Serial.println("Connected to MQTT Server");
      break;
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
  delay(100);
  pinMode(A0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  oled.setTextSize(2);
  
  setup_wifi();
  client.setServer(server, port);
  //client.setCallback(callback);
}

void loop() {
  // Graph signal
  if(x > 127){
    oled.clearDisplay();
    x = 0;
    lastX = 0;
  }

  int pulse = analogRead(A0);
  int now = millis();
  int y = 60 - (pulse/16);
  
  oled.setTextColor(WHITE);
  oled.writeLine(lastX,lastY,x,y,WHITE);
  lastY=y;
  lastX=x;
  
  if(pulse > UpperThreshold){
    if(beatComplete){
      BPM = now - lastTime;
      BPM = int(60/(float(BPM)/1000));
      BPMTiming = false;
      beatComplete = false;

      digitalWrite(pinBuzzer,HIGH);
      delay(10);
      digitalWrite(pinBuzzer,LOW);
    }
    if(BPMTiming == false){
      lastTime = millis();
      BPMTiming = true;
    }
  }
  if((pulse < LowerThreshold) & (BPMTiming))
    beatComplete = true;

  // Graph BPM
  oled.writeFillRect(0,50,128,16,BLACK);
  oled.setCursor(0,50);
  oled.print(BPM);
  oled.print(" BPM");
  oled.display();
  x++;
  if (!client.connected()) {
    reconnect();
  } else {
    String payload = "{";
    payload += "\"signal\":"; payload += String(pulse); payload += ",";
    payload += "\"BPM\":"; payload += String(BPM);
    payload += "}";
    char attributes[100];
    payload.toCharArray( attributes, 100 );
    Serial.println(payload);
    client.publish("v1/devices/me/telemetry",attributes);
  }
  client.loop();
}
