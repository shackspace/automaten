#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Adafruit_VL53L0X.h"

#define sensor_sr04

#ifdef sensor_vl53l0x
  #define sensor_vl53l0x
  Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#endif

const char ssid[] = "shack";
const char password[] = "welcome2shack";
const char* mqtt_user = "matemat";
const char* mqtt_pass = "nMSvqnH8kPem";
const char* mqtt_server = "appserv.tutschonwieder.net";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#ifdef sensor_sr04
  #include <HCSR04.h>
  const int trgPin = D4;
  const int echoPin = D5;
  UltraSonicDistanceSensor distanceSensor(trgPin, echoPin);
#endif

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

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
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()), mqtt_user, mqtt_pass) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("/sensor/matemat/M6", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  Serial.println("BOOT");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

#ifdef sensor_vl53l0x
    Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
#endif

#ifdef sensor_sr04
  pinMode(trgPin, OUTPUT);
  pinMode(echoPin, INPUT); 
#endif
}


void loop() {
#ifdef sensor_vl53l0x
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }

   client.loop();
#endif

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

  #ifdef sensor_vl53l0x
    int num = measure.RangeMilliMeter;
    char cstr[16];
    itoa(num, cstr, 10);

    client.publish("/sensor/matemat/M6", cstr);
  #endif

  #ifdef sensor_sr04
    //Serial.println(distanceSensor.measureDistanceCm());
    //Distance1 =   distanceSensor.measureDistanceCm();
    //Serial.println("Distanz: " + String(Distance1));
    client.publish("/sensor/matemat/M6", String(distanceSensor.measureDistanceCm()).c_str());
  #endif

  }

  delay(1000);
}