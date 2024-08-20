#include "WiFi.h"
#include <LiquidCrystal_I2C.h>

const int buttonPin = 23;   
// GPIO pin connected to the push button
int buttonState = 0;       
// Variable to hold the button state
int previousButtonState = HIGH; 
// Variable to hold the previous button state
int currentIndex = 0;      
// Index to track the current network being displayed
const int maxNetworks = 20; 
// Maximum number of networks to store

// Arrays to store WiFi scan results
String ssid[maxNetworks];
long rssi[maxNetworks];
int channel[maxNetworks];
int encryption[maxNetworks];
int networkCount = 0;      
// Number of networks found

LiquidCrystal_I2C lcd(0x27, 16, 2);  
// Initialize the LCD with I2C address 0x27 and 16x2 size

void setup() {
  Serial.begin(115200);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Initialize the button pin as an input with an internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");

  // Initial WiFi scan
  scanNetworks();
}

void loop() {
  // Read the state of the push button
  buttonState = digitalRead(buttonPin);

  // Check if the button is pressed (LOW state because of pull-up)
  if (buttonState == LOW && previousButtonState == HIGH) {
    Serial.println("Button pressed");

    // Display the next network's information
    displayNetwork(currentIndex);

    // Move to the next network, wrap around if necessary
    currentIndex = (currentIndex + 1) % networkCount;

    // Debounce the button
    delay(200);
  }

  previousButtonState = buttonState;
}

void scanNetworks() {
  Serial.println("Scanning for networks...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scanning for ");
    lcd.setCursor(0, 1);
    lcd.print("    networks...");

  // Perform the WiFi scan
  networkCount = WiFi.scanNetworks();
  if (networkCount == 0) {
    Serial.println("No networks found");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No networks found");
  } else {
    Serial.print(networkCount);
    Serial.println(" networks found");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("networks found");
    lcd.setCursor(7, 1);
    lcd.print(networkCount);


    // Limit the number of networks to maxNetworks
    if (networkCount > maxNetworks) {
      networkCount = maxNetworks;
    }

    // Store the scan results
    for (int i = 0; i < networkCount; ++i) {
      ssid[i] = WiFi.SSID(i);
      rssi[i] = WiFi.RSSI(i);
      channel[i] = WiFi.channel(i);
      encryption[i] = WiFi.encryptionType(i);
    }
  }

  // Delete the scan result to free memory
  WiFi.scanDelete();
}

void displayNetwork(int index) {
  if (index < networkCount) {
    // Display on Serial Monitor
    Serial.print(index + 1); 
     // Print network number
    Serial.print("- ");
    Serial.print(ssid[index]);  
    // Print SSID (WiFi name)
    Serial.print(" (Ch: ");
    Serial.print(channel[index]);  
    // Print Channel number
    Serial.println(")");
    
    Serial.print(" RSSI: ");
    Serial.print(rssi[index]);  
    // Print RSSI value
    
    Serial.print(" | ");
    Serial.print("Encryption: ");
    String encryptionType;
    switch (encryption[index]) {
      case WIFI_AUTH_OPEN:            encryptionType = "Open"; break;
      case WIFI_AUTH_WEP:             encryptionType = "WEP"; break;
      case WIFI_AUTH_WPA_PSK:         encryptionType = "WPA"; break;
      case WIFI_AUTH_WPA2_PSK:        encryptionType = "WPA2"; break;
      case WIFI_AUTH_WPA_WPA2_PSK:    encryptionType = "WPA+WPA2"; break;
      case WIFI_AUTH_WPA2_ENTERPRISE: encryptionType = "WPA2-EAP"; break;
      case WIFI_AUTH_WPA3_PSK:        encryptionType = "WPA3"; break;
      case WIFI_AUTH_WPA2_WPA3_PSK:   encryptionType = "WPA2+WPA3"; break;
      case WIFI_AUTH_WAPI_PSK:        encryptionType = "WAPI"; break;
      default:                        encryptionType = "Unknown";
    }
    Serial.println(encryptionType);
    Serial.println();  // Blank line for spacing between networks

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ssid[index].substring(0, 8)); 
     // Print the SSID (trimmed to fit)
    lcd.setCursor(9, 0);
    lcd.print("Ch:");
    lcd.print(channel[index]);  
    // Print the channel number

    lcd.setCursor(0, 1);
    lcd.print("RSSI:");
    lcd.print(rssi[index]);  
    // Print the RSSI value

    lcd.setCursor(9, 1);
    lcd.print(encryptionType.substring(0, 7));  
    // Print the encryption type (trimmed to fit)
  }
}
