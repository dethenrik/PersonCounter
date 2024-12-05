
# Komplett Guide til at Bygge en Tæller med ESP32, ThingSpeak, HTML, JavaScript og PlatformIO

Denne guide forklarer, hvordan du bygger et system, der tæller mennesker, der går ind og ud af et rum, ved hjælp af en ESP32, ThingSpeak til cloudlagring, HTML til visning af data, og JavaScript til at hente og gemme data.

## Forudsætninger

Før du starter, skal du sørge for, at du har følgende installeret:
- **PlatformIO** i Visual Studio Code (VS Code)
- **ThingSpeak** konto (til at oprette en kanal)
- **Wi-Fi Netværk** med koderne til din router
- **Breadboard**, **ESP32** og nødvendige **sensorer** (2 digitale sensorer til at registrere folk, der går ind og ud)

## Tilslutning af ESP32 til Breadboardet

### 1. Sensorer
- **Sensor 1** til at registrere indkommende personer:
  - **Sensor Pin**: GPIO 25 på ESP32.
  - Forbind den ene ledning til **GPIO 25** og den anden til **GND** (jord).

- **Sensor 2** til at registrere udgående personer:
  - **Sensor Pin**: GPIO 23 på ESP32.
  - Forbind den ene ledning til **GPIO 23** og den anden til **GND** (jord).

### 2. Forbindelser til ESP32
- Forbind **3V** på ESP32 til **+** på breadboardet.
- Forbind **GND** på ESP32 til **-** på breadboardet.

## PlatformIO Opsætning

### 1. Installér PlatformIO
- Hvis du ikke allerede har PlatformIO installeret, kan du gøre det via [PlatformIO's hjemmeside](https://platformio.org/).
- Installér PlatformIO-pluginet i Visual Studio Code.

### 2. Opret et nyt projekt i PlatformIO
- Åbn **PlatformIO** i Visual Studio Code.
- Klik på **New Project**, vælg **ESP32** som platform, og vælg **Arduino** framework.
- Projektet oprettes automatisk i den relevante mappe.

### 3. Konfigurer PlatformIO.ini
- Åbn filen `platformio.ini` i dit projekt og erstat med følgende:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
lib_deps = mathworks/ThingSpeak@^2.0.0
```

### 4. Installér nødvendige biblioteker
- Når du åbner projektet, sørg for, at biblioteket **ThingSpeak** er installeret, som angivet i `platformio.ini`.

## Upload af Kode til ESP32

### 1. Opret en fil ved navn `main.cpp`
- Opret en fil kaldet `main.cpp` i din **src**-mappe.

### 2. Kopier og indsæt følgende kode i `main.cpp`

```cpp
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Arduino.h>

// Definer GPIO-pins til sensorer
const int sensor1Pin = 25;
const int sensor2Pin = 23;

// Wi-Fi oplysninger
const char* ssid = "E308";
const char* password = "98806829";

// ThingSpeak opsætning
unsigned long channelID = 2771658;
const char* writeAPIKey = "762I0WX8SZUMHMI4";
const char* readAPIKey = "W9RF5JQJ6UKWZXVN";
WiFiClient client;

// Variabler
unsigned long lastUpdate = 0;
int counterIn = 0;
int counterOut = 0;
int finalCount = 0;
int currentInRoom = 0;
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

  currentInRoom = counterIn - counterOut;

  // Upload data to ThingSpeak
  if (millis() - lastUpdate > 15000) {
    ThingSpeak.setField(1, counterIn);
    ThingSpeak.setField(2, counterOut);
    ThingSpeak.setField(3, finalCount);
    ThingSpeak.setField(4, currentInRoom);
    ThingSpeak.writeFields(channelID, writeAPIKey);
    lastUpdate = millis();
  }
}
```

## ThingSpeak Opsætning

### 1. Opret en Kanal
- Gå til [ThingSpeak](https://thingspeak.com/) og log ind på din konto.
- Opret en ny kanal, og noter **Channel ID** og **Write API Key**, da de skal bruges i koden.

### 2. Opret Felter
- I din kanal skal du oprette **fire felter**:
  - **Field 1**: Counter In (antal personer, der går ind)
  - **Field 2**: Counter Out (antal personer, der går ud)
  - **Field 3**: Final Count (totalt antal personer, der er kommet ind)
  - **Field 4**: Current In Room (antal personer i rummet)

## HTML og JavaScript til Visning af Data

### 1. HTML-fil
Opret en fil ved navn `index.html` og indsæt følgende kode:

```html
<!DOCTYPE html>
<html lang="da">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>People Counter</title>
</head>
<body>
    <h1>People Counter</h1>
    <p>Number of people in room: <span id="currentInRoom">0</span></p>
    <p>People who entered: <span id="counterIn">0</span></p>
    <p>People who exited: <span id="counterOut">0</span></p>

    <script>
        async function fetchData() {
            const response = await fetch('https://api.thingspeak.com/channels/YOUR_CHANNEL_ID/fields/1.json?api_key=YOUR_READ_API_KEY');
            const data = await response.json();
            const field1 = data.feeds[data.feeds.length - 1].field1;
            const field2 = data.feeds[data.feeds.length - 1].field2;
            const field4 = data.feeds[data.feeds.length - 1].field4;

            document.getElementById('counterIn').innerText = field1 || 0;
            document.getElementById('counterOut').innerText = field2 || 0;
            document.getElementById('currentInRoom').innerText = field4 || 0;
        }

        setInterval(fetchData, 15000);
    </script>
</body>
</html>
```

- Udskift `YOUR_CHANNEL_ID` med dit kanal-ID og `YOUR_READ_API_KEY` med din **Read API Key** fra ThingSpeak.

### 2. Åbning af HTML-fil
- Åbn `index.html` i en webbrowser for at se tælleren opdateret med de nyeste data fra ThingSpeak.

## Afslutning

Nu har du et system, der tæller folk, der går ind og ud af et rum, og sender disse data til ThingSpeak. HTML-siden opdaterer automatisk antallet af personer i rummet baseret på de data, der er gemt i ThingSpeak.
