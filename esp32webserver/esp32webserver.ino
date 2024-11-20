#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTPIN 25          // DHT11 data pin connected to GPIO 25 of ESP32
#define DHTTYPE DHT11      // DHT 11
#define relayPin 26        // Relay control pin connected to GPIO 26 of ESP32

#define TRIG_PIN 5         // Trigger pin for ultrasonic sensor
#define ECHO_PIN 18       
  int currentPin = 33;  // Pin connected to ACS712 output
  float sensitivity = 0.185;  // Sensitivity of ACS712 5A (185 mV/A)
  float offsetVoltage = 2.360;  // Adjust based on actual measured offset


// WiFi credentials
const char* ssid = "Motog85";
const char* password = "00000000";

// Create WebServer object on port 80
WebServer server(80);

DHT dht(DHTPIN, DHTTYPE);

long duration;
float distance;
bool fanOn = false;  // Variable to track fan status (ON/OFF)
bool manualControl = false;  // Flag for manual control of the fan




// HTML/JavaScript for the webpage with manual control and added humidity
const char* webpageCode = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Sensor Data</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    body {
      font-family: 'Roboto', sans-serif;
      background: linear-gradient(135deg, #f0f4f8, #e2e6ea);
      color: #333;
      text-align: center;
      margin: 0;
      padding: 20px;
    }
    
    h2 {
      font-size: 2.5em;
      color: #2c3e50;
      margin: 20px 0;
      text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.2);
    }

    #warning {
      color: red;
      font-size: 1.5em;
      display: none;
      margin-top: 20px;
      padding: 10px;
      border: 2px solid red;
      border-radius: 5px;
      background-color: rgba(255, 0, 0, 0.1);
      transition: all 0.3s ease;
      animation: pulse 2s infinite;
    }

    @keyframes pulse {
      0% { transform: scale(0.95); }
      50% { transform: scale(1); }
      100% { transform: scale(0.95); }
    }

    canvas {
      margin-top: 20px;
      padding:20px;
      border: 1px solid #ddd;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);
      border-radius: 10px;
      background-color: white;
    }

    button {
      padding: 10px 20px;
      margin: 10px;
      font-size: 1em;
      background-color: #3498db;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s ease, transform 0.2s ease;
    }

    button:hover {
      background-color: #2980b9;
      transform: scale(1.05);
    }

    button:active {
      transform: scale(0.95);
    }
  </style>
</head>
<body>
  <h2>Real-Time Sensor Data</h2>
  <div id="warning">Warning: Oil level is below 5 cm!</div>
  <canvas id="sensorChart" width="400" height="200"></canvas>
  
  <button onclick="toggleFan()">Toggle Fan</button> <!-- Button to toggle fan -->
  <p id="fanStatus">Fan is OFF</p>

  <script>
    var ctx = document.getElementById('sensorChart').getContext('2d');
    var chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'Temperature (°C)',
          borderColor: 'rgb(255, 99, 132)',
          data: []
        }, {
          label: 'Humidity (%)',
          borderColor: 'rgb(75, 192, 192)',
          data: []
        }, {
          label: 'Oil Level (cm)',
          borderColor: 'rgb(54, 162, 235)',
          data: []
        }]
      },
      options: {
        scales: { y: { beginAtZero: true } },
        plugins: {
          title: {
            display: true,
            text: 'Fan Status: '
          }
        }
      }
    });

    function updateData() {
      fetch('/data')
      .then(response => response.json())
      .then(data => {
        if (data.temperature !== -1) {
          chart.data.labels.push(new Date().toLocaleTimeString());
          chart.data.datasets[0].data.push(data.temperature);
        }

        if (data.humidity !== -1) {
          chart.data.datasets[1].data.push(data.humidity);
        }

        if (data.oilLevel !== -1) {
          chart.data.datasets[2].data.push(data.oilLevel);
          const warningDiv = document.getElementById("warning");
          if (data.oilLevel >= 3) {
            warningDiv.style.display = "block";
          } else {
            warningDiv.style.display = "none";
          }
        }

        document.getElementById("fanStatus").innerText = data.fan == 1 ? "Fan is ON" : "Fan is OFF";
        chart.update();
      });
    }

    // Function to toggle the fan manually
    function toggleFan() {
      fetch('/toggleFan')
      .then(response => response.json())
      .then(data => {
        document.getElementById("fanStatus").innerText = data.fan == 1 ? "Fan is ON" : "Fan is OFF";
      });
    }

    setInterval(updateData, 5000);  
  </script>
</body>
</html>
)rawliteral";

// Serve the HTML/JavaScript webpage
void handleRoot() {
  server.send(200, "text/html", webpageCode);
}

// API to send sensor data as JSON
void handleData() {
  float temperature = dht.readTemperature();  // Get temperature reading
  float humidity = dht.readHumidity();        // Get humidity reading
  if (isnan(temperature)) {
    temperature = -1;
  }
  if (isnan(humidity)) {
    humidity = -1;
  }

  // Oil level monitoring
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;
  if (distance < 0) distance = -1;

  // Prepare JSON data
  String data = "{\"temperature\":";
  data += temperature;
  data += ", \"humidity\":";
  data += humidity;
  data += ", \"fan\":";
  data += fanOn ? "1" : "0";
  data += ", \"oilLevel\":";
  data += distance;
  data += "}";

  server.send(200, "application/json", data);
}

// Toggle fan state manually
void handleFanToggle() {
  manualControl = !manualControl;  // Toggle manual control flag
  fanOn = manualControl;  // Set fanOn based on manual control
  digitalWrite(relayPin, fanOn ? LOW : HIGH);  // Control relay based on fanOn state

  String data = "{\"fan\":";
  data += fanOn ? "1" : "0";
  data += "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");

  server.send(200, "application/json", data);  // Send fan status back as JSON
}

void setup() {
  Serial.begin(115200);

  // Initialize DHT11 sensor
  dht.begin();

  // Initialize relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);  // Turn fan off initially

 // Initialize ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Handle routes
  server.on("/", handleRoot);            // Serve the webpage
  server.on("/data", handleData);        // Serve sensor data as JSON
  server.on("/toggleFan", handleFanToggle);  // Handle fan toggle

 

 pinMode(currentPin, INPUT);


  server.begin();
}

void loop() {
  server.handleClient();  // Handle client requests

  if (!manualControl) {  // Only handle automatic control if not in manual mode
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      if (temperature > 32.00) {
        fanOn = true;
        digitalWrite(relayPin, LOW);  // Turn fan ON
      } else {
        fanOn = false;
        digitalWrite(relayPin, HIGH);  // Turn fan OFF
      }
    }
  }

  delay(200);


  float totalCurrent = 0.0;
    int numReadings = 100;  // Number of readings to average
    int adcValue;

    for (int i = 0; i < numReadings; i++) {
        adcValue = analogRead(currentPin);  // Read the analog input from ACS712
        float adcVoltage = adcValue * (3.3 / 4096.0);  // Convert ADC value to voltage
        float current = ((-adcVoltage + offsetVoltage) / sensitivity);  // Calculate current
        totalCurrent += current;  // Accumulate the readings

        delay(10);  // Small delay between readings for stability
    }
    float averageCurrent = totalCurrent / numReadings;  // Calculate the average current

    // Convert average current to mA for display
    float currentInmA = averageCurrent * 1000;  // Convert to milliamps

    if (currentInmA > 12000) {  
        currentInmA=0;
    }
    //Display the average current value



    // Add logic for detecting small currents, triggering alerts, etc.
    if (currentInmA < 20) {  // Example threshold of 2000mA (2A)
        Serial.println("UnderCurrent detected! Triggering alert.");
        // Code to trigger relay or alert
    }
  Serial.println(WiFi.localIP());

    // Oil Level Monitoring
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;
  delay(200);
  Serial.print("Oil Level (cm): ");
  Serial.println(distance);

  Serial.print("Average Current: ");
  Serial.print(currentInmA);
  Serial.println(" mA");

  // Print temperature and humidity in the Serial Monitor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.print("Temperature (°C): ");
  Serial.println(temperature);
  Serial.print("Humidity (%): ");
  Serial.println(humidity);  



  Serial.println("------------");

  delay(1000);
  
  


}