// Make sure to install those libraries.
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace these with your actual WiFi credentials
const char* ssid = "Your-WiFi-Network";
const char* password = "Your-WiFi-Password";

// Replace this with the latitude and longitude of your location
const float latitude = 0.00;
const float longitude = 0.000000;
// Make sure to replace "0" with latiude and longitude.

// Replace this with your LCD I2C address
int lcdColumns = 20;
int lcdRows = 4;
int lcdAddr = 0x27;  // Change this to your I2C address

LiquidCrystal_I2C lcd(lcdAddr, lcdColumns, lcdRows);

// Flag to indicate which weather information to display
bool displayTemp = true;

void setup() {
  Serial.begin(115200);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lcd.print("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();

  // Print IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Fetch weather data
  String weatherData = getWeatherData();
  
  // Parse weather data
  String weather;
  if (displayTemp) {
    weather = parseTemperature(weatherData);
  } else {
    weather = parseWindSpeed(weatherData);
  }
  
  // Display weather on LCD
  displayOnLCD(weather);

  // Toggle display flag every 3 seconds
  delay(3000);
  displayTemp = !displayTemp;
}

String getWeatherData() {
  HTTPClient http;
  String weatherData;
  // Open Meteo API. Does not requires API key.
  String url = "https://api.open-meteo.com/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + "&current=temperature_2m,wind_speed_10m&hourly=temperature_2m,relative_humidity_2m,wind_speed_10m";

  http.begin(url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      weatherData = http.getString();
    }
  } else {
    Serial.println("Error getting weather data");
  }

  http.end();
  return weatherData;
}

String parseTemperature(String weatherData) {
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, weatherData);

  float currentTemp = doc["current"]["temperature_2m"];

  String result = "Temp: " + String(currentTemp) + "C";
  
  return result;
}

String parseWindSpeed(String weatherData) {
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, weatherData);

  float currentWindSpeed = doc["current"]["wind_speed_10m"];

  String result = "Wind: " + String(currentWindSpeed) + "km/h";
  
  return result;
}

void displayOnLCD(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}
