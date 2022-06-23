#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>

// Update these with values suitable for your network.
#define sensorLDR A0
int nilaiSensor;
int readSoilMoisture = D5;
int resultSoil;
const int relay = D4;

const char *ssid = "JTI-POLINEMA";          // nama wifi diganti
const char *password = "jtifast!";          // passwod diganti
const char *mqtt_server = "192.168.72.113"; // ip diganti

WiFiClient espClient;
PubSubClient client(espClient);

int value = 0;

// connect wifi
void setup_wifi()
{

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// connect node red (wifi)
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // client.subscribe("node1/temperature");
      // client.subscribe("node1/humidity");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// connect wifi
void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(readSoilMoisture, INPUT);
  pinMode(relay, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

// sensor cahaya
void loop()
{

  /*
    Code dibawah untuk koneksi
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  */
  nilaiSensor = analogRead(sensorLDR);

  static char ldr[7];
  dtostrf(nilaiSensor, 6, 2, ldr);

  // client.publish("node1/ldr", ldr);
  Serial.print("Nilai Sensor : ");
  Serial.println(nilaiSensor);

  resultSoil = digitalRead(readSoilMoisture);
  Serial.print("Moisture : ");
  Serial.println(resultSoil);
  static char ldr_ldr[7];
  dtostrf(resultSoil, 6, 2, ldr_ldr);
  // client.publish("node1/soil", ldr_ldr);

  if (nilaiSensor < 150) // pagi
  {
    Serial.println("JAM PAGI");

    if (resultSoil == 1) // kering
    {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Pompa Menyala"); // pompa bergerak
      delay(20000);
      digitalWrite(relay, LOW);
    }
    if (resultSoil == 0) // basah
    {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(relay, HIGH);
      Serial.println("Pompa Mati"); // pompa tidak bergerak
      delay(5000);
    }
  }

  if (nilaiSensor > 150 && nilaiSensor < 100) // siang
  {
    Serial.println("JAM SIANG");

    if (resultSoil == 1) // kering
    {
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(relay, LOW);
      Serial.println("Pompa Menyala"); // pompa bergerak
      delay(5000);
    }
    if (resultSoil == 0) // basah
    {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(relay, HIGH);
      Serial.println("Pompa Mati"); // pompa tidak bergerak
      delay(5000);
    }
  }

  if (nilaiSensor > 150) // sore
  {
    Serial.println("JAM SORE");
    if (resultSoil == 1) // kering
    {
      digitalWrite(LED_BUILTIN, LOW);
            digitalWrite(relay, LOW);

      Serial.println("Pompa Menyala"); // pompa bergerak
      delay(10000);
      digitalWrite(relay, HIGH);
    }
    if (resultSoil == 0) // basah
    {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(relay, LOW);
      Serial.println("Pompa Mati"); // pompa tidak bergerak
      delay(10000);
      digitalWrite(relay, HIGH);
    }
  }
  delay(2000);
}