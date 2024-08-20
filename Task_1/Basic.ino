#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>


const char* ssid     = "TEdata7D2C42";
const char* password = "15909078";

// Define NTP Client 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Time offset in seconds UTC+1 means 3600 which means 1 hour
const long utcOffsetInSeconds = 7200 + 3600;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  // Wait half a second before retrying
  }

  Serial.println("Connected to WiFi");

  // Initialize NTPClient with a time offset
  timeClient.begin();
  timeClient.setTimeOffset(utcOffsetInSeconds);
}

void loop() {
  timeClient.update();  // Update time
  Serial.println(timeClient.getFormattedTime());  // Print the time
  delay(1000);  // Wait for a second before printing again
}
