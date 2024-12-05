# Step-by-Step Guide to Set Up ESP32 with ThingSpeak and Traffic Sensor

This guide will walk you through the entire process of setting up your ESP32 with a TC51 sensor, connecting to ThingSpeak, and displaying data on a webpage.

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Installing Required Software](#installing-required-software)
3. [Setting Up ESP32 in Visual Studio Code](#setting-up-esp32-in-visual-studio-code)
4. [Wiring the ESP32 and TC51 Sensors](#wiring-the-esp32-and-tc51-sensors)
5. [ThingSpeak Setup](#thingspeak-setup)
6. [Uploading Code to ESP32](#uploading-code-to-esp32)
7. [Creating the Web Interface](#creating-the-web-interface)
8. [Testing the System](#testing-the-system)
9. [Troubleshooting](#troubleshooting)

## Prerequisites
- **ESP32 board** (e.g., ESP32 DevKit)
- **TC51 traffic sensors** (two sensors, one for detecting "IN" and one for detecting "OUT")
- **Wi-Fi network credentials** (SSID and password)
- **ThingSpeak account** (to store and visualize data)
- **Visual Studio Code** (with PlatformIO extension)

## Installing Required Software

1. **Install Visual Studio Code**:
    - Download from [here](https://code.visualstudio.com/).
    - Install and launch Visual Studio Code.

2. **Install PlatformIO**:
    - Open Visual Studio Code.
    - Go to Extensions (on the left sidebar).
    - Search for **PlatformIO** and click **Install**.

3. **Install the ESP32 Platform**:
    - Open PlatformIO in Visual Studio Code.
    - Go to **PlatformIO Home** and click on **Platforms**.
    - Search for **Espressif 32** and click **Install**.

## Setting Up ESP32 in Visual Studio Code

1. **Create a new PlatformIO project**:
    - Open PlatformIO Home, then click **New Project**.
    - Name your project (e.g., `TrafficCounter`), select **ESP32 DevKit** as the board, and choose **Arduino** as the framework.
    - Click **Finish** to create the project.

2. **Add the ThingSpeak Library**:
    - In the `platformio.ini` file of your project, add the following line under `[env:esp32]`:
    ```ini
    lib_deps = mathworks/ThingSpeak@^2.0.0
    ```

## Wiring the ESP32 and TC51 Sensors

1. **ESP32 Pinout**:
    - **Sensor 1 (IN):** Connect one wire from GPIO 25 on ESP32 to the **OUT** pin of the first TC51 sensor.
    - **Sensor 2 (OUT):** Connect one wire from GPIO 23 on ESP32 to the **OUT** pin of the second TC51 sensor.
    - **VCC:** Connect the **VCC** pin of both sensors to the **3V3** pin of the ESP32.
    - **GND:** Connect the **GND** pin of both sensors to the **GND** pin of the ESP32.

2. **Connections Summary**:
    - **GPIO 25 → Sensor 1 (IN)**
    - **GPIO 23 → Sensor 2 (OUT)**
    - **3V3 → VCC for both sensors**
    - **GND → GND for both sensors**

## ThingSpeak Setup

1. **Create a ThingSpeak account**:
    - Go to [ThingSpeak](https://thingspeak.com/) and create an account if you don’t have one.

2. **Create a new channel**:
    - After logging in, click **Channels** on the top menu, then click **New Channel**.
    - Fill out the channel name and description (e.g., `Room Counter`).
    - Add four fields: `field1`, `field2`, `field3`, and `field4`. These will store the sensor data for people entering, exiting, total visits, and current room occupancy.

3. **Get the API keys**:
    - Once the channel is created, you’ll see the channel details. Note down the **Channel ID** and **Write API Key**. You will use these in your code.

## Uploading Code to ESP32

1. **Main Code (Arduino Sketch)**:
    - Create a new file `main.cpp` inside your PlatformIO project `src` folder and paste the following code:

    ```cpp
    #include <WiFi.h>
    #include <ThingSpeak.h>
    #include <Arduino.h>

    const int sensor1Pin = 25;  // Sensor 1 for "IN"
    const int sensor2Pin = 23;  // Sensor 2 for "OUT"

    const char* ssid = "E308";           // Replace with your Wi-Fi SSID
    const char* password = "98806829";   // Replace with your Wi-Fi password

    unsigned long channelID = 2771658;          // Channel ID
    const char* writeAPIKey = "762I0WX8SZUMHMI4";  // Write API Key
    WiFiClient client;

    unsigned long lastUpdate = 0;
    int counterIn = 0;
    int counterOut = 0;
    int finalCount = 0;
    int currentInRoom = 0;

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
    }

    void loop() {
      static int lastSensor1Value = 0;
      static int lastSensor2Value = 1;

      int sensor1Value = digitalRead(sensor1Pin);
      int sensor2Value = digitalRead(sensor2Pin);

      unsigned long currentMillis = millis();

      if (lastSensor1Value == 0 && sensor1Value == 1) {
        counterIn++;
        Serial.print("CounterIn: ");
        Serial.println(counterIn);
      }

      if (lastSensor2Value == 1 && sensor2Value == 0) {
        counterOut++;
        Serial.print("CounterOut: ");
        Serial.println(counterOut);
      }

      currentInRoom = counterIn - counterOut;
      finalCount = counterIn;

      if (currentMillis - lastUpdate >= 18000) {  // 18 seconds
        lastUpdate = currentMillis;

        ThingSpeak.setField(1, counterIn);
        ThingSpeak.setField(2, counterOut);
        ThingSpeak.setField(3, finalCount);
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
    ```

2. **Upload the code to your ESP32**:
    - Connect your ESP32 to your computer via USB.
    - In PlatformIO, click **Upload** to compile and upload the code to the ESP32.

## Creating the Web Interface

1. **Create an HTML file**:
    - In the root folder of your project, create an `index.html` file and paste the following code:
    ```html
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Room Counter</title>
      <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #beb9b9; }
        .container { max-width: 1200px; margin: 0 auto; padding: 20px; text-align: center; }
        .counter { font-size: 30px; margin: 10px 0; }
        .counter span { font-weight: bold; }
      </style>
    </head>
    <body>
      <div class="container">
        <h1>People Counter</h1>
        <div class="counter">
          <h2>People Entered: <span id="counterIn">0</span></h2>
        </div>
        <div class="counter">
          <h2>People Exited: <span id="counterOut">0</span></h2>
        </div>
        <div class="counter">
          <h2>Current People in Room: <span id="currentInRoom">0</span></h2>
        </div>
        <div class="counter">
          <h2>Total Visits Today: <span id="finalCount">0</span></h2>
        </div>
        <button onclick="saveData()">Save Data</button>
      </div>
      <script src="script.js"></script>
    </body>
    </html>
    ```

2. **Create the JavaScript file**:
    - Create a `script.js` file to fetch and update data:
    ```javascript
    const updateData = () => {
      fetch('https://api.thingspeak.com/channels/YOUR_CHANNEL_ID/fields.json?api_key=YOUR_API_KEY')
      .then(response => response.json())
      .then(data => {
        document.getElementById('counterIn').innerText = data.field1;
        document.getElementById('counterOut').innerText = data.field2;
        document.getElementById('currentInRoom').innerText = data.field3;
        document.getElementById('finalCount').innerText = data.field4;
      })
      .catch(error => console.log('Error fetching data: ', error));
    };
    setInterval(updateData, 10000);  // Update every 10 seconds
    ```

    Replace `YOUR_CHANNEL_ID` and `YOUR_API_KEY` with your ThingSpeak channel ID and API key.

## Testing the System

1. **Connect the ESP32 to your Wi-Fi**.
2. **Monitor the data**:
    - After running the system, you can check the ThingSpeak channel for live data updates.
3. **Test the sensor behavior**:
    - The system should record the number of people entering and exiting based on the sensor signals.
    
## Troubleshooting

- **No data on ThingSpeak**:
    - Ensure that your ESP32 is connected to Wi-Fi.
    - Check that the write API key and channel ID are correct.
    - Make sure the sensor wires are properly connected to the ESP32 pins.
