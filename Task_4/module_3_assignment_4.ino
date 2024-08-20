// necessary libraries
#include <WiFiClientSecure.h>  // Library for secure Wi-Fi connection
#include <esp_crt_bundle.h>    // ESP32 certificate bundle
#include <WiFi.h>              // Library for Wi-Fi functions
#include <PubSubClient.h>      // library for MQTT client implementation using HiveMQ
#include <ESP32Servo.h>        // library for servo control on ESP32

// Wi-Fi credentials
const char* ssid = "WE2020";        // Wi-Fi network SSID
const char* password = "epic2023";  // Wi-Fi network password

// MQTT broker configuration
const char* mqtt_broker = "bae6e1004af84917878e457c24d59cce.s1.eu.hivemq.cloud";  // MQTT broker
const char* mqtt_username = "esp32";                                              // MQTT username
const char* mqtt_password = "esppass";                                            // MQTT password
const char* mqttServer = "a7be40337d754d4ebe031ec532ceb81f.s1.eu.hivemq.cloud";   // MQTT alternative address
const int mqttPort = 8883;                                                        // MQTT port

// MQTT topics
const char* IR_sensor_topic = "IR/distance";  // topic for publishing the IR sensor's distance readings
const char* servo_topic = "servo/angle";      // topic to subscribe for determining the servo's angle

// instance for Wi-Fi client, MQTT client and servo motor
WiFiClientSecure espClient;      // secure Wi-Fi client instance
PubSubClient client(espClient);  // MQTT client instance using the Wi-Fi client
Servo servo;                     // servo motor instance

// pin configuration
int servo_pin = 33;      // setting GPIO pin for servo at 33
int IR_sensor_pin = 32;  // setting GPIO pin for IR sensor at 32

// declaring the callback function for handling MQTT messages
void callback(char* topic, byte* payload, unsigned int length);

void setup() {

// declaring serial communication
Serial.begin(115200);  // setting serial communication at baud 115200
delay(10);

// connecting to Wi-Fi
WiFi.begin(ssid, password);              // begin Wi-Fi connection
while (WiFi.status() != WL_CONNECTED) {  // if it is still not connected
  delay(500);
  Serial.print(".");                     // print "." to indicate its status as connecting
}
// setting Wi-Fi client into insecure
espClient.setInsecure();       // setting insecure for SSL
Serial.println("Connected.");  // confirming the success of the connection

// setting MQTT configuration
client.setServer(mqtt_broker, mqttPort);  // setting MQTT client server
client.setCallback(callback);  // setting MQTT client callback function

// connecting to MQTT broker
while (!client.connected()) {
  Serial.print("Connecting to MQTT Broker..."); // printing that we will attempt to connect to MQTT broker
  String client_id = "esp32-client-" + String(WiFi.macAddress());  // create client ID
  if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {  // attempt connection
    Serial.println("Connected to MQTT Broker");  // connection complete
    client.subscribe("servo/angle");  // subscribing to servo angle topic
  } else {
    Serial.print("Failed with state ");  // connection failed
    Serial.print(client.state());  // print failure state
    delay(2000);
  }
}

// setting pin configuration
pinMode(IR_sensor_pin, INPUT);  // setting IR sensor's pin as input
servo.attach(servo_pin);        // attaching servo to its pin
servo.write(0);                 // setting servo's initial position
delay(1000);
}

void loop() {

// maintaining MQTT connection
client.loop();

// reading and publishing the IR sensor's readings
int distance = analogRead(IR_sensor_pin);                   // read analog value for distance from IR sensor
Serial.println(distance);                                   // printing read distance on serial monitor
client.publish(IR_sensor_topic, String(distance).c_str());  // publishing distance to MQTT IR sensor topic
delay(1000);
}

// setting callback function for handling MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;                              // declaring temporary string
  for(int i=0; i<length; i+=1) {                   // looping through recieved payload
    messageTemp += (char)payload[i];               // turning each character to string message
  }
  if (String(topic) == "servo/angle") {            // if the message is for the servo angle topic
    int position = (payload[0] == '0') ? 0 : 180;  // get the servo's position to either 0 or 180 based on the recieved message
    servo.write(position);                         // apply the recieved position
  }
}