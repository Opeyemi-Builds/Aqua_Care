/*
   PROJECT: AQUA CARE
   DEVELOPERS: ALEGBELEYE OPEYEMI & OYEBISI HAKEEM
*/
// Including all necessary libraries for WiFi, HTTP, Sensors, and Display
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// This part is for the WiFi connection and server endpoint
const char *ssid = "KIMZY";
const char *pass = "12345678";
const char *serverName = "https://defensibly-noninfallible-jamika.ngrok-free.dev/api/sensor-data";

// Declaration of the pins and sensor types
#define DHTPIN 4
#define DHTTYPE DHT11
#define SOIL_PIN 34
#define PUMP_PIN 16
#define TRIG 17
#define ECHO 18

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 4000;

float cached_t = 0.0;
float cached_h = 0.0;
int cached_moisture = 0;
float cached_water = 0.0;
bool cached_pumpOn = false;

void setup()
{
    Serial.begin(115200);

    dht.begin();
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    Wire.begin(21, 22);
    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(10, 20);
        display.println("AQUA CARE");
        display.setTextSize(1);
        display.setCursor(20, 45);
        display.println("SYSTEM BOOT...");
        display.display();
    }
    else
    {
        Serial.println("OLED allocation failed!");
    }

    WiFi.begin(ssid, pass);
    Serial.print("Connecting to KIMZY Hotspot");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nCONNECTED TO NETWORK!");
}

float getWaterLevel()
{
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    long duration = pulseIn(ECHO, HIGH);
    float distance = duration * 0.034 / 2;
    float pct = map(distance, 17, 5, 0, 100);
    return constrain(pct, 0, 100);
}
void sendDataToServer()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi Disconnected. Skipping send.");
        return;
    }

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    if (http.begin(client, serverName))
    {
        http.addHeader("Content-Type", "application/json");
        http.addHeader("ngrok-skip-browser-warning", "true");

        String json = "{\"moisture\":" + String(cached_moisture) +
                      ",\"temperature\":" + String(cached_t) +
                      ",\"humidity\":" + String(cached_h) +
                      ",\"water_level\":" + String(cached_water) +
                      ",\"pump_status\":" + (cached_pumpOn ? "true" : "false") + "}";

        Serial.println("Sending JSON: " + json);
        int httpResponseCode = http.POST(json);
        Serial.print("Server Response: ");
        Serial.println(httpResponseCode);
        http.end();
    }
    else
    {
        Serial.println("Error: Could not connect to Ngrok URL.");
    }
}

void loop()
{
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t))
    {
        t = 0.0;
    }
    if (isnan(h))
    {
        h = 0.0;
    }

    int rawSoil = analogRead(SOIL_PIN);
    int moisture = constrain(map(rawSoil, 3200, 1500, 0, 100), 0, 100);

    float water = getWaterLevel();

    // The pump control logic
    bool pumpOn = (moisture < 60 && water > 10);
    digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);

    // Cache values for the HTTP sender to use
    cached_t = t;
    cached_h = h;
    cached_moisture = moisture;
    cached_water = water;
    cached_pumpOn = pumpOn;

    // Updating of the OLED Display

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("AQUA CARE - LIVE");
    display.print("Soil: ");
    display.print(moisture);
    display.println("%");
    display.print("Tank: ");
    display.print(water, 0);
    display.println("%");
    display.print("Temp: ");
    display.print(t, 1);
    display.println("C");
    display.print("Humi: ");
    display.print(h, 1);
    display.println("%");
    display.print("Pump: ");
    display.println(pumpOn ? "ACTIVE" : "STANDBY");
    display.display();

    unsigned long now = millis();
    if (now - lastSendTime >= SEND_INTERVAL)
    {
        lastSendTime = now;
        sendDataToServer();
    }

    delay(200);
}