#include <NewPing.h>            // Library for ultrasonic sensor
#include <LiquidCrystal_I2C.h>  // Library for I2C LCD display
#include <ESP32Servo.h>         // Library for controlling servos on ESP32
#include <WiFiClientSecure.h>   // Library for secure Wi-Fi connection
#include <esp_crt_bundle.h>     // ESP32 certificate bundle
#include <WiFi.h>               // Library for Wi-Fi functions
#include <PubSubClient.h>       // Library for MQTT communication

// Pin Definitions
#define trig_pin 19       // Ultrasonic sensor trigger pin
#define echo_pin 18       // Ultrasonic sensor echo pin
#define button_pin 26     // Button input pin
#define mic_pin 27        // Microphone input pin

// Wi-Fi credentials
const char* ssid = "WE2020";            // Wi-Fi SSID
const char* password = "epic2023";      // Wi-Fi password

// MQTT Broker details
const char* mqtt_broker = "bae6e1004af84917878e457c24d59cce.s1.eu.hivemq.cloud";
const char* mqtt_username = "esp32";   // MQTT username
const char* mqtt_password = "esppass"; // MQTT password
const int mqtt_port = 8883;            // MQTT port for secure connection
const char* topic_publish = "esp32/send";       // MQTT topic for publishing messages
const char* topic_distance = "esp32/distance";  // MQTT topic for distance sensor data
const char* topic_mic = "esp32/mic";            // MQTT topic for microphone data
const char* topic_temp = "esp32/temp";          // MQTT topic for temperature readings
const char* topic_subscribe = "esp32/receive";  // MQTT topic for receiving messages

// Variables for timing and control
long mil = 0;           // Current time in milliseconds
long lastmil = 0;       // Last recorded time in milliseconds
int pos = 0;            // Position of the servo motor
int i = 0;              // Index for the mic data being sent
bool moveto180 = true;  // Flag to control servo direction
bool mic_read = false;  // Flag to control microphone data reading
long lastmic = 0;       // Last time microphone data was read
long micstart = 0;      // Start time for microphone data reading

// Variables to store sensor readings
float currentDistance = 0;  // Current distance reading
int currentTemp = 0;        // Current temperature reading

// Initialize Wi-Fi and MQTT client objects
WiFiClientSecure espClient;   // Secure Wi-Fi client
PubSubClient client(espClient);  // MQTT client using secure Wi-Fi client
Servo serv;                      // Servo motor control object
LiquidCrystal_I2C lcd(0x27,16,2); // LCD display object (I2C address: 0x27, 16x2 characters)
NewPing sensor(trig_pin, echo_pin, 40); // Ultrasonic sensor object (max distance: 40cm)
#define LED_PIN 2  // LED pin (GPIO 2)

// Callback function to handle messages received on subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
    
    // Convert payload to a string
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    // Check the content of the message and display the appropriate data
    if (message == "0") {
        // Print temperature data on the LCD
        lcd.clear();
        lcd.home();
        lcd.print("Fetching Temp...");
        // Request temperature data from the cloud
        client.publish(topic_temp, "get", true); // Set QoS level 1
    } else if (message == "1") {
        // Print distance data on the LCD
        lcd.clear();
        lcd.home();
        lcd.print("Fetching Dist...");
        // Request distance data from the cloud
        client.publish(topic_distance, "get", true); // Set QoS level 1
    } else {
        // Display the message on the LCD (if it doesn't match "0" or "1")
        lcd.clear();
        lcd.home();
        lcd.print("Message: ");
        lcd.setCursor(0, 1);
        lcd.print(message);
    }
}

// Callback function to handle incoming sensor data
void dataCallback(char* topic, byte* payload, unsigned int length) {
    // Convert payload to a string
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    if (String(topic) == topic_temp) {
        // Update and display temperature data
        currentTemp = message.toInt();
        lcd.clear();
        lcd.home();
        lcd.print("Temp: ");
        lcd.print(currentTemp);
        lcd.print(" C");
    } else if (String(topic) == topic_distance) {
        // Update and display distance data
        currentDistance = message.toFloat();
        lcd.clear();
        lcd.home();
        lcd.print("Dist: ");
        lcd.print(currentDistance);
        lcd.print(" cm");
    }
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Set pin modes
    pinMode(32, INPUT);    // Set GPIO 35 as input (analog pin for temperature sensor)
    pinMode(LED_PIN, OUTPUT); // Set LED pin as output
    digitalWrite(LED_PIN, LOW); // Initially turn off the LED
    
    // Initialize Servo
    serv.attach(33);      // Attach servo to GPIO 33
    serv.write(0);        // Set initial servo position to 0 degrees
    
    // Initialize LCD
    lcd.init();           // Initialize LCD
    lcd.backlight();      // Turn on LCD backlight
    
    // Connect to Wi-Fi
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");

    // Set up secure Wi-Fi client (insecure for this example)
    espClient.setInsecure();
    
    // Connect to MQTT Broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback); // Set the MQTT callback function
    
    while (!client.connected()) {
        Serial.print("Connecting to MQTT Broker...");
        String client_id = "esp32-client-" + String(WiFi.macAddress());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT Broker");
            client.subscribe(topic_subscribe, 1); // Subscribe with QoS level 1
            client.subscribe(topic_temp, 1);      // Subscribe with QoS level 1
            client.subscribe(topic_distance, 1);  // Subscribe with QoS level 1
        } else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    
    // Publish an initial message to the MQTT topic with QoS level 1
    client.publish(topic_publish, "Hello from ESP32", true); // Set QoS level 1 and retain flag

    // Record the current time
    mil = millis();
    lastmil = mil;
}

void loop() {
    // Keep the MQTT client connected and process incoming messages
    client.loop();
    
    // Update the current time
    mil = millis();
    
    // Check if button is pressed and start reading microphone data
    if (!mic_read && analogRead(34) > 2000) {
        client.publish(topic_mic, String(analogRead(35)).c_str(), true); // Set QoS level 1
        lastmic = mil; 
        mic_read = true;   // Start microphone data reading
        micstart = mil;    // Record start time of microphone data reading
    }
    
    // Continue reading microphone data every 10ms for 4 seconds
    if (mic_read && mil - lastmic > 10) {
        client.publish(topic_mic, String(analogRead(35)).c_str(), true); // Set QoS level 1
        lastmic = mil;
        if (mil - micstart > 4000) {
            client.publish(topic_mic, "Done", true); // Set QoS level 1
            mic_read = false;  // Stop microphone data reading after 4 seconds
        }
    }
    
    // Control servo movement and publish sensor data every 100ms
    if (mil - lastmil > 100) {
        // Move servo motor to the next position
        if (moveto180) {
            pos += 10;
            if (pos == 180) moveto180 = false;  // Change direction at 180 degrees
        } else {
            pos -= 10;
            if (pos == 0) moveto180 = true;    // Change direction at 0 degrees
        }
        
        // Read temperature from analog pin (connected to a temperature sensor)
        int temp = analogRead(32);
        currentTemp = map(temp, 0, 4095, -55, 125);   // Map the analog reading to temperature range
        
        // Measure distance using ultrasonic sensor
        unsigned int uS = sensor.ping();
        currentDistance = uS / US_ROUNDTRIP_CM; // Convert time to distance in cm
        
        // Publish temperature and distance data to MQTT topics with QoS level 1
        client.publish(topic_temp, String(currentTemp).c_str(), true); // Set QoS level 1
        client.publish(topic_distance, String(currentDistance).c_str(), true); // Set QoS level 1
        
        // Update servo position
        serv.write(pos);
        
        // Record the last time the loop was executed
        lastmil = mil;
    }
}
