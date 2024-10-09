





// #include <DHT.h>

// #define DHTPIN 25          // DHT11 data pin connected to GPIO 25 of ESP32
// #define DHTTYPE DHT11      // DHT 11
// #define relayPin 26        // Relay control pin connected to GPIO 26 of ESP32

// #define TRIG_PIN 5         // Trigger pin for ultrasonic sensor
// #define ECHO_PIN 18        // Echo pin for ultrasonic sensor

// DHT dht(DHTPIN, DHTTYPE);

// long duration;
// float distance;

// void setup() {
//   Serial.begin(115200);
  
//   // Initialize DHT11 sensor
//   dht.begin();
  
//   // Initialize the relay pin as output
//   pinMode(relayPin, OUTPUT);
  
//   // Set relay to OFF initially (LOW)
//   digitalWrite(relayPin, LOW);

//   // Initialize ultrasonic sensor pins
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);
// }

// void loop() {
//   // Temperature Monitoring and Relay Control
//   float temperature = dht.readTemperature();
  
//   // Check if the reading is valid
//   if (isnan(temperature)) {
//     Serial.println("Failed to read from DHT sensor!");
//   } else {
//     // Print the temperature to the Serial Monitor
//     Serial.print("Temperature: ");
//     Serial.print(temperature);
//     Serial.println(" *C");

//     // Control the relay based on the temperature
//     if (temperature > 33.40) {
//       digitalWrite(relayPin, LOW); // Turn the fan ON
//       Serial.println("Fan ON");
//     } else {
//       digitalWrite(relayPin, HIGH); // Turn the fan OFF
//       Serial.println("Fan OFF");
//     }
//   }

//   // Oil Level Monitoring with Ultrasonic Sensor
//   // Send out a trigger pulse
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);

//   // Measure the time taken for the echo pulse
//   duration = pulseIn(ECHO_PIN, HIGH);

//   // Calculate distance in cm (speed of sound is 340 m/s or 29.1 microseconds per cm)
//   distance = duration * 0.034 / 2;

//   Serial.print("Oil Level (cm): ");
//   Serial.println(distance);

//   // Check if oil level is below the threshold
//   if (distance > 5.0) { // Example threshold of 15 cm
//     Serial.println("Oil level is low! Alert triggered.");
//     // Add relay or notification logic here
//   } else {
//     Serial.println("Oil level is adequate.");
//   }

//   // Delay before next reading
//   delay(2000); 
// }






//TEMP CODE

// #include <DHT.h>

// #define DHTPIN 25          // DHT11 data pin connected to GPIO 18 of ESP32
// #define DHTTYPE DHT11      // DHT 11
// #define relayPin 26       // Relay control pin connected to GPIO 17 of ESP32

// DHT dht(DHTPIN, DHTTYPE);

// void setup() {
//   Serial.begin(115200);
  
//   dht.begin();
  
//   // Initialize the relay pin as output
//   pinMode(relayPin, OUTPUT);
  
//   // Set relay to OFF initially (LOW)
//   digitalWrite(relayPin, LOW);
// }

// void loop() {
//   // Reading temperature from DHT11
//   float temperature = dht.readTemperature();
  
//   // Check if the reading is valid
//   if (isnan(temperature)) {
//     Serial.println("Failed to read from DHT sensor!");
//     return;
//   }

//   // Print the temperature to the Serial Monitor
//   Serial.print("Temperature: ");
//   Serial.print(temperature);
//   Serial.println(" *C");

//   // Control the relay based on the temperature
//   if (temperature > 33.40) {
//     digitalWrite(relayPin, LOW); // Turn the fan ON
//     Serial.println("Fan ON");
//   } else {
//     digitalWrite(relayPin, HIGH); // Turn the fan OFF
//     Serial.println("Fan OFF");
//   }

//   // Delay for 2 seconds before reading again
//   delay(2000);
// }





//OIL LEVEL CODE

// #define TRIG_PIN 5 // Trigger pin
// #define ECHO_PIN 18 // Echo pin

// long duration;
// float distance;

// void setup() {
//   Serial.begin(115200);
  
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);
// }

// void loop() {
//   // Send out a trigger pulse
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);

//   // Measure the time taken for the echo pulse
//   duration = pulseIn(ECHO_PIN, HIGH);

//   // Calculate distance in cm (speed of sound is 340 m/s or 29.1 microseconds per cm)
//   distance = duration * 0.034 / 2;

//   Serial.print("Oil Level (cm): ");
//   Serial.println(distance);

//   // Check if oil level is below the threshold
//   if (distance > 15.0) { // Example threshold of 15 cm
//     Serial.println("Oil level is low! Alert triggered.");
//     // Add relay or notification logic here
//   } else {
//     Serial.println("Oil level is adequate.");
//   }

//   delay(1000); // Delay of 1 second
// }





//RELAY TESTING CODE 

// int relay_pin_1 =18;
// void setup(){
//   pinMode(relay_pin_1,OUTPUT);
//     pinMode(relay_pin_1,OUTPUT);

// }
// void loop(){
//   digitalWrite(relay_pin_1,LOW);
  

//   delay(5000);
//   digitalWrite(relay_pin_1,HIGH);


//   delay(5000);

// }





// const int currentPin = 25;  // Pin connected to ACS712 output
// const float sensitivity = 0.185;  // Sensitivity of ACS712 5A (185 mV/A)
// const float offsetVoltage = 2.47;  // Adjust based on actual measured offset

// void setup() {
//     Serial.begin(115200);
//     pinMode(currentPin, INPUT);
// }

// void loop() {
//     float totalCurrent = 0.0;
//     int numReadings = 100;  // Number of readings to average

//     for (int i = 0; i < numReadings; i++) {
//         int adcValue = analogRead(currentPin);  // Read the analog input from ACS712
//         float adcVoltage = adcValue * (3.3 / 4096.0);  // Convert ADC value to voltage
//         float current = abs((adcVoltage - offsetVoltage) / sensitivity);  // Calculate current
//         totalCurrent += current;  // Accumulate the readings

//         delay(10);  // Small delay between readings for stability
//     }

//     float averageCurrent = totalCurrent / numReadings;  // Calculate the average current

//     // Convert average current to mA for display
//     float currentInmA = averageCurrent * 1000;  // Convert to milliamps

    
//     // Display the average current value
//     Serial.print("Average Current: ");
//     Serial.print(currentInmA);
//     Serial.println(" mA");

//     // Add logic for detecting small currents, triggering alerts, etc.
//     if (currentInmA > 700) {  // Example threshold of 2000mA (2A)
//         Serial.println("Overcurrent detected! Triggering alert.");
//         // Code to trigger relay or alert
//     }

//     delay(1000);  // Delay before the next set of readings
// }





// const int voltagePin = 25; // Pin connected to the voltage sensor output
// float calibrationFactor = 5.088;  // Sensor scales 25V down to 3.3V max and variation makes it this

// void setup() {
//     Serial.begin(115200);
//     pinMode(voltagePin, INPUT);
// }

// void loop() {
//     int adcValue = analogRead(voltagePin);  // Read the analog value from the sensor
//     float voltageMeasured = adcValue * (3.3 / 4096.0);  // Convert ADC value to voltage (0-3.3V)
//     float actualVoltage = voltageMeasured * calibrationFactor;  // Scale the voltage back to real value
//     Serial.print("Voltage: ");
//     Serial.print(actualVoltage);
//     Serial.println(" V");


//     // Add logic for overload or underload detection
//     if (actualVoltage > 12.0) {  // Example threshold for overvoltage
//         Serial.println("Overvoltage detected!");
//         // Code to trigger relay or alert
//     }

//     delay(1000);  // Delay before next reading
// }









