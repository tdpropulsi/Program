#include <WiFi.h>
#include <FirebaseESP32.h>
#include <time.h>

#define WIFI_SSID "wifi bayar"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_HOST "https://esp32-gilang-data-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyB3AhEYnuLIqsTAn3O3GaYn_9OaQtx6rdM"

const int analogPin = 34; // Replace with your analog input pin
const char* firebasePath = "/analog_values"; // Firebase database path
const char* rtcPath = "/timestamps"; // RTC timestamps path

FirebaseData firebaseData;
time_t currentTime;
struct tm timeInfo;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  configTime(0, 0, "pool.ntp.org"); // Configure RTC with NTP server
  while (!getLocalTime(&timeInfo)) {
    delay(1000);
    Serial.println("Failed to obtain time");
  }
}

void loop() {
  int analogValue = analogRead(analogPin);

  String timestamp = getCurrentTimestamp(); // Get RTC timestamp in ISO 8601 format

  // Prepare the data to be uploaded to Firebase
  FirebaseJson json;
  json.set("analog_value", analogValue);
  json.set("timestamp", timestamp);

  if (Firebase.pushJSON(firebaseData, firebasePath, json)) {
    Serial.println("Data sent to Firebase");
  } else {
    Serial.println("Failed to send data to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  delay(5000); // Delay for 5 seconds before taking the next reading
}

String getCurrentTimestamp() {
  if (!getLocalTime(&timeInfo)) {
    return "error";
  }

  char buffer[50];
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02dZ",
           timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
           timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

  return String(buffer);
}
