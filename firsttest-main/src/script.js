const channelID = 2771658; // Your ThingSpeak Channel ID
const readAPIKey = 'W9RF5JQJ6UKWZXVN'; // Replace with your ThingSpeak Read API Key
const url = `https://api.thingspeak.com/channels/${channelID}/feeds.json?api_key=${readAPIKey}&results=1`;

async function fetchData() {
  try {
    const response = await fetch(url);
    if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
    const data = await response.json();

    if (data.feeds.length === 0) {
      console.error("No feeds available in ThingSpeak response.");
      return;
    }

    const latestFeed = data.feeds[0];
    // Fetch the data from the fields
    document.getElementById('counterIn').textContent = latestFeed.field1 || 0;
    document.getElementById('counterOut').textContent = latestFeed.field2 || 0;
    document.getElementById('currentInRoom').textContent = latestFeed.field4 || 0;
    document.getElementById('finalCount').textContent = latestFeed.field3 || 0;
  } catch (error) {
    console.error('Error fetching data from ThingSpeak:', error);
  }
}

setInterval(fetchData, 21000); // Update every 21 seconds
fetchData(); // Initial fetch

// Function to save data to a text file
function saveData() {
  const counterIn = document.getElementById('counterIn').textContent;
  const counterOut = document.getElementById('counterOut').textContent;
  const currentInRoom = document.getElementById('currentInRoom').textContent;
  const finalCount = document.getElementById('finalCount').textContent;
  const timestamp = new Date().toLocaleString();

  const data = `Timestamp: ${timestamp}\nPeople Entered: ${counterIn}\nPeople Exited: ${counterOut}\nCurrent People in Room: ${currentInRoom}\nTotal Visits Today: ${finalCount}\n\n`;

  // Trigger download of a text file
  const blob = new Blob([data], { type: 'text/plain' });
  const link = document.createElement('a');
  link.href = URL.createObjectURL(blob);
  link.download = 'data_output.txt';
  link.click();
}
