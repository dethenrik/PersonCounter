
# SensorCounter

The **SensorCounter** project uses two sensors to track activity in a doorway, enabling traffic counting for anyone walking in or out.

## Logic Overview

The system works by monitoring changes in sensor values to track the number of people entering and exiting a space. Here's the breakdown of the logic:

### Sensor 1 (Counts people walking IN):
- The counter increases when the sensor value changes from **0 to 1** (indicating someone walked in).

### Sensor 2 (Counts people walking OUT):
- The counter increases when the sensor value changes from **1 to 0** (indicating someone walked out).

---

## Example of Sensor Logic

### If the sensor states are as follows:
- **IN** = 1: Increment `counterIn` by 1 (someone entered).
- **OUT** = 0: Increment `counterOut` by 1 (someone exited).

### Sample Data:
- `counterIn`: 30
- `counterOut`: 20

---

## Checking and Displaying the Difference
Every 21 seconds, the difference between the two counters is checked and displayed.

### Example Calculation:
- `counterIn(30)` and `counterOut(20)` give a difference of **10**.
- This means **10 people** are still in the room.

---

## Updating and Displaying Today's Visits
Whenever the two counters are equal, the system updates the **"Today's Visits"** count.

### Condition:
- When `counterIn` equals `counterOut`, the system will display "Today's Visits" with the current counter value.

### Example:
- `counterIn(30)` and `counterOut(30)` means **Today's Visits** = 30.

---

## Final Data Fields for Display:
- `counterIn`: Total number of people who have entered.
- `counterOut`: Total number of people who have exited.
- `finalCount`: Displays the total number of visits for the day.
- `currentInRoom`: Number of people currently inside (calculated as `counterIn - counterOut`).

---

## System Update Frequency
- The system sends data to **ThingSpeak** every 21 seconds, which includes the number of people entering, exiting, the total visits for the day, and the current number of people in the room.

---

## Files Created

The project consists of the following key files:

### 1. **Arduino Code** (`sensor_counter.ino`)
This is the Arduino script that runs on the ESP32. It monitors the two sensors and sends data to ThingSpeak.

**Key Components:**
- Initializes the sensors and Wi-Fi connection.
- Monitors the sensors and increments the counters.
- Sends data to ThingSpeak every 21 seconds.

```cpp
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Arduino.h>

// Sensor and Wi-Fi setup...
void setup() {
  // Sensor and Wi-Fi initialization...
}

void loop() {
  // Logic for reading sensors and sending data to ThingSpeak...
}
```

### 2. **HTML File** (`index.html`)
This is the webpage that will display the data fetched from ThingSpeak.

**Key Components:**
- Displays real-time data for `counterIn`, `counterOut`, `currentInRoom`, and `finalCount`.
- Fetches data from ThingSpeak using JavaScript.

```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Room Counter</title>
  <!-- Add CSS for styling -->
</head>
<body>
  <div class="container">
    <h1>People Counter</h1>
    <!-- Display counters here -->
  </div>
  <script src="script.js"></script>
</body>
</html>
```

### 3. **JavaScript File** (`script.js`)
This JavaScript file fetches data from ThingSpeak and updates the webpage with the real-time values.

**Key Components:**
- Fetches data from ThingSpeak's API.
- Updates the webpage dynamically every 21 seconds.

```javascript
const channelID = 2771658;
const readAPIKey = 'YOUR_READ_API_KEY';

async function fetchData() {
  // Fetch data from ThingSpeak and update the HTML elements
}
setInterval(fetchData, 21000);
```

### 4. **CSS File** (Optional, within `index.html` or as a separate file)
The CSS file styles the webpage to make the output visually appealing.

---

## Handling the Output

### 1. **Displaying the Data on the Webpage**
The data from ThingSpeak is displayed on the webpage using the `index.html` and `script.js` files. The JavaScript file fetches data from ThingSpeak using its REST API and updates the page with the latest values for `counterIn`, `counterOut`, `finalCount`, and `currentInRoom`.

### 2. **Using Visual Studio Code's Go Live**
- Open the project folder in Visual Studio Code.
- Right-click on the `index.html` file and select **Open with Live Server**.
- This launches a local server, and the webpage will automatically open in your default browser.
- The webpage will fetch and display the real-time data from ThingSpeak every 21 seconds.

### 3. **Data Updates from ThingSpeak**
Every 21 seconds, the system sends updated sensor data to ThingSpeak. The webpage fetches this data and dynamically updates the displayed values, showing the number of people who have entered, exited, and the total count of people currently in the room or who have visited during the day.

### 4. **Example Webpage Output**
After 21 seconds of operation, the webpage will display:

| Field          | Value     |
|----------------|-----------|
| `counterIn`    | 30        |
| `counterOut`   | 20        |
| `finalCount`   | 30        |
| `currentInRoom`| 10        |

---
