#include <WiFi.h>
#include <ThingSpeak.h>
#include <Arduino.h>

// Define GPIO pins for sensors
const int sensor1Pin = 25;  // Sensor 1 for "IN"
const int sensor2Pin = 23;  // Sensor 2 for "OUT"

// Wi-Fi credentials
const char* ssid = "E308";           // Replace with your Wi-Fi SSID
const char* password = "98806829";   // Replace with your Wi-Fi password

// ThingSpeak settings
unsigned long channelID = 2771658;          // Channel ID
const char* writeAPIKey = "762I0WX8SZUMHMI4";  // Write API Key
const char* readAPIKey = "W9RF5JQJ6UKWZXVN";  // Read API Key
WiFiClient client;

// Variables
unsigned long lastUpdate = 0;  // Last update timestamp
int counterIn = 0;             // People entering
int counterOut = 0;            // People exiting
int finalCount = 0;            // Final visits today
int currentInRoom = 0;         // Current number in room
unsigned long sensor1Cooldown = 0;
unsigned long sensor2Cooldown = 0;

void setup() {
  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);

  // Fetch current values from ThingSpeak via HTTP
  String url = "http://api.thingspeak.com/channels/" + String(channelID) + "/feeds.json?api_key=" + String(readAPIKey) + "&results=1";
  Serial.println("Fetching current data from ThingSpeak...");

  if (client.connect("api.thingspeak.com", 80)) {  // Use port 80 for HTTP
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();

    // Wait for the response from ThingSpeak
    String response = "";
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      response += line;
      // Debug the response
      Serial.println(line);
      if (line == "\r") {
        break;  // End of HTTP headers
      }
    }

    // Read the JSON content
    if (response.indexOf("feeds") >= 0) {
      int startIdx = response.indexOf("[{") + 2;
      int endIdx = response.indexOf("}]");
      String data = response.substring(startIdx, endIdx);

      // Extract values from the JSON response
      int field1Start = data.indexOf("\"field1\":\"") + 9;
      int field1End = data.indexOf("\"", field1Start);
      String field1Value = data.substring(field1Start, field1End);
      counterIn = field1Value.toInt();  // Set counterIn to the current value in ThingSpeak
      Serial.print("Current counterIn value: ");
      Serial.println(counterIn);

      int field2Start = data.indexOf("\"field2\":\"") + 9;
      int field2End = data.indexOf("\"", field2Start);
      String field2Value = data.substring(field2Start, field2End);
      counterOut = field2Value.toInt();  // Set counterOut to the current value in ThingSpeak
      Serial.print("Current counterOut value: ");
      Serial.println(counterOut);

      int field3Start = data.indexOf("\"field3\":\"") + 9;
      int field3End = data.indexOf("\"", field3Start);
      String field3Value = data.substring(field3Start, field3End);
      finalCount = field3Value.toInt();  // Set finalCount to the current value in ThingSpeak
      Serial.print("Current finalCount value: ");
      Serial.println(finalCount);

      int field4Start = data.indexOf("\"field4\":\"") + 9;
      int field4End = data.indexOf("\"", field4Start);
      String field4Value = data.substring(field4Start, field4End);
      String field4ValueStr = data.substring(field4Start, field4End);
      currentInRoom = field4ValueStr.toInt();  // Set currentInRoom to the current value in ThingSpeak
      Serial.print("Current currentInRoom value: ");
      Serial.println(currentInRoom);
    } else {
      Serial.println("No feeds data found in the response.");
    }
  } else {
    Serial.println("Failed to connect to ThingSpeak.");
  }
}

void loop() {
  static int lastSensor1Value = 0;
  static int lastSensor2Value = 1;

  int sensor1Value = digitalRead(sensor1Pin);
  int sensor2Value = digitalRead(sensor2Pin);

  unsigned long currentMillis = millis();
  if (sensor1Cooldown > currentMillis) sensor1Value = 0;
  if (sensor2Cooldown > currentMillis) sensor2Value = 1;

  if (lastSensor1Value == 0 && sensor1Value == 1) {
    counterIn++;
    Serial.print("CounterIn: ");
    Serial.println(counterIn);
    sensor2Cooldown = currentMillis + 3000;
  }
  lastSensor1Value = sensor1Value;

  if (lastSensor2Value == 1 && sensor2Value == 0) {
    counterOut++;
    Serial.print("CounterOut: ");
    Serial.println(counterOut);
    sensor1Cooldown = currentMillis + 3000;
  }
  lastSensor2Value = sensor2Value;

  // Calculate the current number of people in the room
  currentInRoom = counterIn - counterOut;

  // Update finalCount if necessary
  finalCount = counterIn;  // Set finalCount to the value of counterIn, or use another logic

  // Send the updated data to ThingSpeak via HTTP only if the counters have changed
  if (currentMillis - lastUpdate >= 18000) {  // 18 seconds
    lastUpdate = currentMillis;

    ThingSpeak.setField(1, counterIn);
    ThingSpeak.setField(2, counterOut);
    ThingSpeak.setField(3, finalCount);  // Ensure finalCount is sent with the update
    ThingSpeak.setField(4, currentInRoom);

    int result = ThingSpeak.writeFields(channelID, writeAPIKey);
    if (result == 200) {
      Serial.println("Data sent to ThingSpeak successfully");
    } else {
      Serial.println("Failed to send data to ThingSpeak");
    }
  }

  delay(100);  // Keep this delay for polling sensors
}
