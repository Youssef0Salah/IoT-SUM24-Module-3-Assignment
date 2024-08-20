#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "m";               // Your WiFi SSID
const char* password = "11111111";    // Your WiFi password

// LED pin
const int ledPin = 13; // Change to the pin number where your LED is connected

// Create a web server on port 80
WebServer server(80);

// Function to handle the root URL
void handleRoot() {
  // HTML content for the root URL
  String html = "<html><body>";
  html += "<h1>ESP32 LED Control</h1>";
  html += "<p><a href=\"/LED/on\"><button>Turn LED On</button></a></p>";
  html += "<p><a href=\"/LED/off\"><button>Turn LED Off</button></a></p>";
  html += "</body></html>";
  // Send the HTML content to the client
  server.send(200, "text/html", html);
}

// Function to handle turning the LED on
void handleLedOn() {
  digitalWrite(ledPin, HIGH); // Turn the LED on
  // Send response to the client
  server.send(200, "text/html", "<html><body><h1>LED is ON</h1><p><a href=\"/\"><button>Back</button></a></p></body></html>");
}

// Function to handle turning the LED off
void handleLedOff() {
  digitalWrite(ledPin, LOW); // Turn the LED off
  // Send response to the client
  server.send(200, "text/html", "<html><body><h1>LED is OFF</h1><p><a href=\"/\"><button>Back</button></a></p></body></html>");
}

void setup() {
  Serial.begin(115200); // Start the Serial communication

  pinMode(ledPin, OUTPUT);   // Set the LED pin as output
  digitalWrite(ledPin, LOW); // Initialize the LED as off

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Display IP address on the Serial Monitor
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Print the site URL in the Serial Monitor
  Serial.print("To control the LED, open the following URL in your browser: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Setup the web server routes
  server.on("/", handleRoot);       // Handle the root URL
  server.on("/LED/on", handleLedOn);  // Handle turning the LED on
  server.on("/LED/off", handleLedOff);// Handle turning the LED off
  server.begin();                   // Start the web server
  Serial.println("HTTP server started");
}

void loop() {
  // Handle client requests
  server.handleClient();
}
