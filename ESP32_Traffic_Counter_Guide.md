
# Hurtig Guide: Brug ESP32 til at tælle personer med ThingSpeak

## Hvad skal du bruge?
- **ESP32** (fx ESP32 DevKit).  
- **TC51-sensorer** til at tælle personer.  
- **Wi-Fi-oplysninger** (navn og adgangskode).  
- En gratis **ThingSpeak-konto**.  
- **Visual Studio Code** med PlatformIO.  

---

## Sådan kommer du i gang

### 1. Sæt software op
1. Download **Visual Studio Code**:  
   [code.visualstudio.com](https://code.visualstudio.com/)  

2. Installer **PlatformIO**:  
   - Åbn Visual Studio Code.  
   - Gå til *Extensions* (ikonet med firkant og klods).  
   - Søg efter "PlatformIO" og klik på **Install**.

---

### 2. Tilslut sensorer
1. Forbind dine sensorer til ESP32:  
   - **Sensor 1 (IN)**: Tilslut OUT til GPIO 25.  
   - **Sensor 2 (OUT)**: Tilslut OUT til GPIO 23.  
   - **3V3**: Tilslut begge sensorer til VCC.  
   - **GND**: Tilslut begge sensorer til GND.  

---

### 3. Opsæt ThingSpeak
1. Gå til [thingspeak.com](https://thingspeak.com/) og opret en konto.  
2. Lav en ny kanal med disse felter:  
   - Personer ind (Field 1).  
   - Personer ud (Field 2).  
   - Samlet besøg (Field 3).  
   - Aktuelle personer i rummet (Field 4).  
3. Kopiér din **Channel ID** og **Write API Key** – de skal bruges senere.

---

### 4. Upload kode til ESP32
1. Åbn PlatformIO, og lav et nyt projekt:  
   - Vælg **ESP32 DevKit** og **Arduino Framework**.  

2. Indsæt dette i din kode:  
   ```cpp
   const int sensor1Pin = 25; 
   const int sensor2Pin = 23; 
   const char* ssid = "DIN_WIFI_NAVN"; 
   const char* password = "DIN_WIFI_KODE"; 
   unsigned long channelID = 123456; 
   const char* writeAPIKey = "DIN_API_KEY"; 
   ```
   Udskift `DIN_WIFI_NAVN`, `DIN_WIFI_KODE`, og `DIN_API_KEY`.  

3. Tilslut ESP32 til din computer, og klik på **Upload** i PlatformIO.  

---

### 5. Tjek systemet
1. Sørg for, at ESP32 er forbundet til Wi-Fi.  
2. Tjek ThingSpeak-kanalen for live-data om personer, der går ind og ud.  

---

## Brug for hjælp?
- Hvis sensorerne ikke virker: Tjek ledningerne.  
- Hvis data ikke opdateres på ThingSpeak: Kontrollér Wi-Fi eller API-oplysninger.  
