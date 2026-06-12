// Define the analog pins being used
const int potPin1 = A0;  // Upper threshold
const int potPin2 = A1;  // Lower threshold
const int sig = A2;      // Signal voltage

// Define the relay control pins
const int relay1Pin = 8; // First Relay (Turns ON first, OFF last)
const int relay2Pin = 9; // Second Relay (Turns ON last, OFF first)

// Timing variables for the 30-second delays
unsigned long relayTimer = 0;
const unsigned long delayTime = 30000; // 30,000 milliseconds = 30 seconds
bool turningOnSeq = false;
bool turningOffSeq = false;

void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  
  // Set the relay pins as outputs
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  
  // Start with both relays turned OFF
  digitalWrite(relay1Pin, LOW); 
  digitalWrite(relay2Pin, LOW); 
}

void loop() {
  // Read the raw input on the analog pins (0 - 1023)
  int sensorValue1 = analogRead(potPin1);
  int sensorValue2 = analogRead(potPin2);
  int sensorValue3 = analogRead(sig);
  
  // Convert the analog readings to voltages (0 - 5V)
  float voltage1 = sensorValue1 * (5.0 / 1023.0); // Upper limit (A0)
  float voltage2 = sensorValue2 * (5.0 / 1023.0); // Lower limit (A1)
  float voltage3 = sensorValue3 * (5.0 / 1023.0); // Signal (A2)
  
  // Read current relay states
  bool r1IsOn = digitalRead(relay1Pin) == HIGH;
  bool r2IsOn = digitalRead(relay2Pin) == HIGH;

  // --- RELAY CONTROL LOGIC (With Bounce Protection) ---
  
  // Condition 1: Signal goes ABOVE upper threshold
  if (voltage3 > voltage1) {
    turningOffSeq = false; // Cancel any active turn-off sequence
    
    // Normal turn-on start: R1 is off. Turn it on, start R2 timer.
    if (!r1IsOn) {
      digitalWrite(relay1Pin, HIGH);
      r1IsOn = true;            // Update local state
      relayTimer = millis();    // Start the stopwatch
      turningOnSeq = true;      // Flag waiting for R2
    } 
    // Recovery trigger: R1 is ALREADY on, but R2 is off, and no timer is running.
    // (Happens if the voltage dropped and came back up mid-sequence)
    else if (!r2IsOn && !turningOnSeq) {
      relayTimer = millis();    
      turningOnSeq = true;      
    }
  } 
  
  // Condition 2: Signal goes BELOW lower threshold
  else if (voltage3 < voltage2) {
    turningOnSeq = false; // Cancel any active turn-on sequence
    
    // Normal turn-off start: R2 is still on. Turn it off, start R1 timer.
    if (r2IsOn) {
      digitalWrite(relay2Pin, LOW);
      r2IsOn = false;           // Update local state
      relayTimer = millis();    // Start the stopwatch
      turningOffSeq = true;     // Flag waiting for R1
    } 
    // Recovery trigger: R2 is ALREADY off, but R1 is on, and no timer is running.
    // (Happens if the voltage spiked and came back down mid-sequence)
    else if (r1IsOn && !turningOffSeq) {
        relayTimer = millis();
        turningOffSeq = true;
    }
  }

  // --- TIMER LOGIC (Non-Blocking) ---
  
  // If we are waiting to turn ON Relay 2, and 30 seconds have passed
  if (turningOnSeq && (millis() - relayTimer >= delayTime)) {
    digitalWrite(relay2Pin, HIGH);
    turningOnSeq = false; // Sequence complete
  }
  
  // If we are waiting to turn OFF Relay 1, and 30 seconds have passed
  if (turningOffSeq && (millis() - relayTimer >= delayTime)) {
    digitalWrite(relay1Pin, LOW);
    turningOffSeq = false; // Sequence complete
  }

  // --- SERIAL MONITOR OUTPUT ---
  
  Serial.print("Up: "); Serial.print(voltage1);
  Serial.print("V | Low: "); Serial.print(voltage2);
  Serial.print("V | Sig: "); Serial.print(voltage3);
  
  // Print Relay States
  Serial.print("V || R1: "); 
  Serial.print(digitalRead(relay1Pin) == HIGH ? "ON" : "OFF");
  Serial.print(" | R2: "); 
  Serial.print(digitalRead(relay2Pin) == HIGH ? "ON" : "OFF");

  // --- LIVE COUNTDOWN TIMER ---
  
  if (turningOnSeq) {
      // Calculate seconds remaining
      int secondsLeft = (delayTime - (millis() - relayTimer)) / 1000;
      if (secondsLeft < 0) secondsLeft = 0; // Prevent showing negative numbers momentarily
      
      Serial.print(" (Wait ");
      Serial.print(secondsLeft);
      Serial.print("s -> R2 ON)");
  } else if (turningOffSeq) {
      int secondsLeft = (delayTime - (millis() - relayTimer)) / 1000;
      if (secondsLeft < 0) secondsLeft = 0;
      
      Serial.print(" (Wait ");
      Serial.print(secondsLeft);
      Serial.print("s -> R1 OFF)");
  }
  
  Serial.println(); // Drop to the next line
  
  // A small 500ms delay for readability
  delay(500); 
}