
#define BLYNK_TEMPLATE_ID "TMPL3tSsnk9jR"
#define BLYNK_TEMPLATE_NAME "Hummidity"
#define BLYNK_AUTH_TOKEN "uIR_EgTkTN561qr2xeX9sr9O1hd7a-JF"// Include Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Blynk Credentials

char ssid[] = "Barath";
char pass[] = "n6dss8p7";

// Sensor and Pin Definitions
#define TRIG_PIN 4
#define ECHO_PIN 5
#define DHTPIN 14
#define DHTTYPE DHT11
#define ONE_WIRE_BUS 21
#define LED1_PIN 12
#define LED2_PIN 13
#define FLEX1_PIN 34
#define FLEX2_PIN 35
#define FLEX3_PIN 32
#define DF_TX 17
#define DF_RX 16
#define BUZZER1_PIN 18
#define BUZZER2_PIN 19

const int X_PIN = 36;
const int Y_PIN = 39;
const int Z_PIN = 37;

// Objects
OneWire oneWire(ONE_WIRE_BUS);
DHT dht(DHTPIN, DHTTYPE);
DallasTemperature sensors(&oneWire);
SoftwareSerial mySerial(DF_RX, DF_TX);
DFRobotDFPlayerMini myDFPlayer;

int readAnalog(int pin, int samples = 100) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delayMicroseconds(100);
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  analogReadResolution(12);
  
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER1_PIN, OUTPUT);
  pinMode(BUZZER2_PIN, OUTPUT);

  digitalWrite(BUZZER1_PIN, LOW);
  digitalWrite(BUZZER2_PIN, LOW);

  dht.begin();
  sensors.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini not found.");
    while (true);
  }
  myDFPlayer.volume(30);
  myDFPlayer.play(1);
}

void loop() {
  Blynk.run();
  

  int flexValue1 = analogRead(FLEX1_PIN);
  int flexValue2 = analogRead(FLEX2_PIN);
  int flexValue3 = analogRead(FLEX3_PIN);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance_cm = duration * 0.034 / 2;

  int x = readAnalog(X_PIN);
  int y = readAnalog(Y_PIN);
  int z = readAnalog(Z_PIN);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, temperatureC);

  if (flexValue1 < 700 && myDFPlayer.available() && myDFPlayer.readType() == DFPlayerPlayFinished) {
    myDFPlayer.play(3);
  }
  if (flexValue2 < 700 && myDFPlayer.available() && myDFPlayer.readType() == DFPlayerPlayFinished) {
    myDFPlayer.play(2);
  }
  if (flexValue3 < 700 && myDFPlayer.available() && myDFPlayer.readType() == DFPlayerPlayFinished) {
    myDFPlayer.play(4);
  }

  if (y < 1800) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(BUZZER1_PIN, HIGH);
    Blynk.virtualWrite(V3, "Movement Detected");
  } else {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(BUZZER1_PIN, LOW);
    Blynk.virtualWrite(V3, "No movement");
  }

  if (distance_cm < 4) {
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(BUZZER2_PIN, HIGH);
    Blynk.virtualWrite(V4, "Urine bag is full");
  } else {
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(BUZZER2_PIN, LOW);
    Blynk.virtualWrite(V4, "Normal level");
  }

  Serial.printf("Distance: %.2f cm\n", distance_cm);
  Serial.printf("Temperature (Room): %.2f C\n", temperature);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("Body Temperature: %.2f C\n", temperatureC);
  Serial.printf("X: %d, Y: %d, Z: %d\n", x, y, z);
  Serial.printf("Flex value 1: %d\n", flexValue1);
  Serial.printf("Flex value 2: %d\n", flexValue2);
  Serial.printf("Flex value 3: %d\n", flexValue3);

  delay(2000);
}
