#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "analogWrite.h"
#include "time.h"

const char* ssid     = "";
const char* password = "";

char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = -21600;
int daylightOffset_sec = 3600;

int digits[4] = {26, 25, 32, 33};
int segments[7] = {15, 16, 17, 19, 21, 22, 23};

int digit1 = 0,
    digit2 = 0;

int b = 15,
    a = 16,
    f = 17,
    dp = 18,
    g = 19,
    c = 21,
    d = 22,
    e = 23;

int switchIn = 12;

void displayNum(int n) {
  switch (n) {
    case 0:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      break;
    case 1:
      digitalWrite(f, HIGH);
      digitalWrite(e, HIGH);
      break;
    case 2:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(d, HIGH);
      break;
    case 3:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      break;
    case 4:
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      break;
    case 5:
      digitalWrite(a, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      break;
    case 6:
      digitalWrite(a, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      break;
    case 7:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      break;
    case 8:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    case 9:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(c, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
  }
}

void displayChar(int charId) {
  switch(charId) {
    case 0:
      digitalWrite(a, HIGH);
      digitalWrite(b, HIGH);
      digitalWrite(g, HIGH);
      digitalWrite(f, HIGH);
    break;
    case 1:
      digitalWrite(a, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(d, HIGH);
    break;
  }
}

void clearAll() {
  for (int i = 0; i != 7; i++) {
    digitalWrite(segments[i], LOW);
  }
  for (int i = 0; i != 4; i++) {
    digitalWrite(digits[i], LOW);
  }
  digitalWrite(dp, LOW);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i != 4; i++) {
    pinMode(digits[i], OUTPUT);
  }
  for (int i = 0; i != 7; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(switchIn, INPUT);
  pinMode(dp, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(dp, HIGH);
    for (int i = 0; i != 4; i++) {
      digitalWrite(digits[i], HIGH);
      delay(500);
    }
    clearAll();
  }
  clearAll();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  HTTPClient http;
  http.begin("http://api.weatherapi.com/v1/current.json?key=[API_KEY]&q=[CITY]");

  int statusCode = http.GET();

  if (statusCode > 0) {
    String payload = http.getString();
    DynamicJsonDocument res(1024);
    deserializeJson(res, payload);
    String temp = res["current"]["temp_c"];
    digit1 = temp[0] - '0';
    digit2 = temp[1] - '0';
  }
}

void printWeather() {
  digitalWrite(digits[0], HIGH);
  displayNum(digit1);
  digitalWrite(digits[0], LOW);
  clearAll();

  digitalWrite(digits[1], HIGH);
  displayNum(digit2);
  digitalWrite(digits[1], LOW);
  clearAll();

  digitalWrite(digits[2], HIGH);
  displayChar(0);
  digitalWrite(digits[2], LOW);
  clearAll();

  digitalWrite(digits[3], HIGH);
  displayChar(1);
  digitalWrite(digits[3], LOW);
  clearAll();
}

void printTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char hour[3];
  char minutes[3];
  strftime(hour, 3, "%I", &timeinfo);
  strftime(minutes, 3, "%M", &timeinfo);

  int hourInt1 = hour[0] - '0',
      hourInt2 = hour[1] - '0';
  int minutesInt1 = minutes[0] - '0',
      minutesInt2 = minutes[1] - '0';

  digitalWrite(digits[0], HIGH);
  displayNum(hourInt1);
  digitalWrite(digits[0], LOW);
  clearAll();

  digitalWrite(digits[1], HIGH);
  displayNum(hourInt2);
  digitalWrite(dp, HIGH);
  digitalWrite(digits[1], LOW);
  clearAll();

  digitalWrite(digits[2], HIGH);
  displayNum(minutesInt1);
  digitalWrite(digits[2], LOW);
  clearAll();

  digitalWrite(digits[3], HIGH);
  displayNum(minutesInt2);
  digitalWrite(digits[3], LOW);
  clearAll();
}

void loop() {
  switch (digitalRead(switchIn)) {
    case HIGH:
      return printTime();
      break;
    default:
      return printWeather();
      break;
  }
}
