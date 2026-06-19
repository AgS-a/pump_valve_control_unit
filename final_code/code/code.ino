#include <avr/pgmspace.h>

// =========================================================
// 1. PIN DEFINITIONS & RELAY TIMING VARIABLES
// =========================================================
const int potPin1 = A0;  // Upper threshold
const int potPin2 = A1;  // Lower threshold
const int sig = A2;      // Signal voltage

const int relay1Pin = 8; // First Relay (Turns ON first, OFF last)
const int relay2Pin = 9; // Second Relay (Turns ON last, OFF first)

unsigned long relayTimer = 0;
const unsigned long delayTime = 30000; // 30,000 milliseconds = 30 seconds
bool turningOnSeq = false;
bool turningOffSeq = false;

// =========================================================
// 2. LOOKUP TABLE (PROGMEM)
// =========================================================
const int LUT_SIZE = 200;
const float y_table[] PROGMEM = {
  3.0502, 3.0751, 3.1016, 3.1298, 3.1594, 3.1904, 3.2227, 3.2562, 3.2910, 3.3268, 3.3637, 3.4016, 3.4404, 3.4801, 3.5206, 3.5619, 
  3.6039, 3.6466, 3.6899, 3.7339, 3.7785, 3.8236, 3.8692, 3.9153, 3.9618, 4.0088, 4.0563, 4.1041, 4.1523, 4.2008, 4.2497, 4.2990, 
  4.3485, 4.3984, 4.4485, 4.4990, 4.5497, 4.6007, 4.6519, 4.7034, 4.7552, 4.8072, 4.8595, 4.9120, 4.9647, 5.0177, 5.0709, 5.1243, 
  5.1780, 5.2319, 5.2860, 5.3404, 5.3949, 5.4497, 5.5047, 5.5600, 5.6154, 5.6711, 5.7270, 5.7831, 5.8394, 5.8960, 5.9527, 6.0096, 
  6.0667, 6.1240, 6.1815, 6.2392, 6.2971, 6.3551, 6.4132, 6.4716, 6.5300, 6.5886, 6.6473, 6.7062, 6.7651, 6.8241, 6.8832, 6.9423, 
  7.0015, 7.0608, 7.1200, 7.1792, 7.2385, 7.2977, 7.3568, 7.4158, 7.4748, 7.5336, 7.5924, 7.6509, 7.7093, 7.7674, 7.8254, 7.8831, 
  7.9405, 7.9976, 8.0543, 8.1108, 8.1668, 8.2224, 8.2776, 8.3323, 8.3865, 8.4402, 8.4934, 8.5459, 8.5979, 8.6492, 8.6998, 8.7497, 
  8.7990, 8.8474, 8.8950, 8.9419, 8.9878, 9.0330, 9.0771, 9.1204, 9.1627, 9.2040, 9.2443, 9.2835, 9.3217, 9.3588, 9.3947, 9.4296, 
  9.4632, 9.4957, 9.5270, 9.5570, 9.5858, 9.6134, 9.6397, 9.6647, 9.6884, 9.7108, 9.7319, 9.7517, 9.7701, 9.7873, 9.8031, 9.8176, 
  9.8308, 9.8426, 9.8532, 9.8625, 9.8706, 9.8773, 9.8829, 9.8872, 9.8904, 9.8925, 9.8934, 9.8932, 9.8921, 9.8899, 9.8868, 9.8829, 
  9.8781, 9.8725, 9.8663, 9.8594, 9.8520, 9.8442, 9.8359, 9.8274, 9.8187, 9.8099, 9.8011, 9.7925, 9.7841, 9.7760, 9.7685, 9.7616, 
  9.7556, 9.7504, 9.7464, 9.7436, 9.7422, 9.7424, 9.7445, 9.7485, 9.7547, 9.7633, 9.7745, 9.7885, 9.8056, 9.8260, 9.8500, 9.8778, 
  9.9097, 9.9459, 9.9868, 10.0327, 10.0838, 10.1404, 10.2030, 10.2718
};
const float x_table[] PROGMEM = {
  10.0000, 10.6007, 11.2374, 11.9124, 12.6280, 13.3865, 14.1906, 15.0430, 15.9466, 16.9045, 17.9199, 18.9963, 20.1374, 21.3470, 
  22.6293, 23.9886, 25.4295, 26.9570, 28.5763, 30.2928, 32.1124, 34.0413, 36.0861, 38.2537, 40.5515, 42.9874, 45.5695, 48.3068, 
  51.2085, 54.2844, 57.5452, 61.0018, 64.6660, 68.5504, 72.6680, 77.0331, 81.6603, 86.5654, 91.7652, 97.2773, 103.1206, 109.3148, 
  115.8811, 122.8418, 130.2206, 138.0427, 146.3346, 155.1246, 164.4426, 174.3203, 184.7913, 195.8913, 207.6581, 220.1316, 233.3545, 
  247.3715, 262.2306, 277.9822, 294.6800, 312.3808, 331.1448, 351.0359, 372.1218, 394.4744, 418.1696, 443.2881, 469.9154, 498.1422, 
  528.0645, 559.7841, 593.4091, 629.0539, 666.8397, 706.8953, 749.3569, 794.3691, 842.0851, 892.6673, 946.2878, 1003.1293, 1063.3850, 
  1127.2602, 1194.9722, 1266.7515, 1342.8424, 1423.5040, 1509.0107, 1599.6536, 1695.7412, 1797.6006, 1905.5785, 2020.0423, 2141.3818, 
  2270.0098, 2406.3643, 2550.9092, 2704.1366, 2866.5681, 3038.7565, 3221.2878, 3414.7834, 3619.9018, 3837.3412, 4067.8418, 4312.1880, 
  4571.2115, 4845.7940, 5136.8701, 5445.4304, 5772.5253, 6119.2680, 6486.8388, 6876.4888, 7289.5441, 7727.4108, 8191.5792, 8683.6291, 
  9205.2353, 9758.1733, 10344.3250, 10965.6856, 11624.3699, 12322.6199, 13062.8122, 13847.4663, 14679.2527, 15561.0028, 16495.7175, 
  17486.5785, 18536.9584, 19650.4322, 20830.7898, 22082.0490, 23408.4685, 24814.5630, 26305.1186, 27885.2085, 29560.2108, 31335.8268, 
  33218.1002, 35213.4374, 37328.6302, 39570.8779, 41947.8123, 44467.5240, 47138.5891, 49970.0992, 52971.6917, 56153.5832, 59526.6039, 
  63102.2344, 66892.6451, 70910.7373, 75170.1873, 79685.4930, 84472.0231, 89546.0693, 94924.9023, 100626.8297, 106671.2592, 113078.7642, 
  119871.1536, 127071.5468, 134704.4516, 142795.8480, 151373.2765, 160465.9320, 170104.7631, 180322.5773, 191154.1529, 202636.3571, 
  214808.2717, 227711.3261, 241389.4382, 255889.1641, 271259.8562, 287553.8315, 304826.5497, 323136.8015, 342546.9094, 363122.9392, 
  384934.9254, 408057.1089, 432568.1905, 458551.5982, 486095.7713, 515294.4615, 546247.0520, 579058.8956, 613841.6737, 650713.7757, 
  689800.7028, 731235.4945, 775159.1818, 821721.2672, 871080.2334, 923404.0827, 978870.9091, 1037669.5042, 1100000.0000
};

// =========================================================
// 3. INVERSE LOOKUP FUNCTION
// =========================================================
float get_x_from_y(float target_y) {
    // Boundary enforcement: clamp to min/max if the signal goes out of bounds
    float y_min = pgm_read_float(&y_table[0]);
    float y_max = pgm_read_float(&y_table[LUT_SIZE - 1]);
    
    if (target_y <= y_min) return pgm_read_float(&x_table[0]);
    if (target_y >= y_max) return pgm_read_float(&x_table[LUT_SIZE - 1]);

    // Forward scan for the correct interval
    for (int i = 0; i < LUT_SIZE - 1; i++) {
        float y0 = pgm_read_float(&y_table[i]);
        float y1 = pgm_read_float(&y_table[i + 1]);

        // When we find the bracket containing our target_y
        if (target_y >= y0 && target_y <= y1) {
            float x0 = pgm_read_float(&x_table[i]);
            float x1 = pgm_read_float(&x_table[i + 1]);
            
            // Linear interpolation equation
            return x0 + (target_y - y0) * ((x1 - x0) / (y1 - y0));
        }
    }
    
    return 0.0; // Fallback 
}

// =========================================================
// 4. ARDUINO SETUP
// =========================================================
void setup() {
  Serial.begin(9600);
  
  // Wait for serial monitor to open 
  while (!Serial) { ; }
  Serial.println("System Ready. LUT Initialized & Relays Online.");
  
  // Set the relay pins as outputs
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  
  // Start with both relays turned OFF
  digitalWrite(relay1Pin, LOW); 
  digitalWrite(relay2Pin, LOW); 
}

// =========================================================
// 5. MAIN EXECUTION LOOP
// =========================================================
void loop() {
  // Read the raw input on the analog pins (0 - 1023)
  int sensorValue1 = analogRead(potPin1);
  int sensorValue2 = analogRead(potPin2);
  int sensorValue3 = analogRead(sig);
  
  // Convert the analog readings to voltages (0 - 5V)
  float voltage1 = sensorValue1 * (5.0 / 1023.0); // Upper limit (A0)
  float voltage2 = sensorValue2 * (5.0 / 1023.0); // Lower limit (A1)
  float voltage3 = sensorValue3 * (5.0 / 1023.0); // Signal (A2)
  
  // --- INTEGRATED LUT LOGIC ---
  // The A2 signal is divided by 2 before going into the Lookup Table as requested
  float lut_input_voltage = voltage3 * 2.0; 
  float calculated_pressure = get_x_from_y(lut_input_voltage);
  
  // Read current relay states
  bool r1IsOn = digitalRead(relay1Pin) == HIGH;
  bool r2IsOn = digitalRead(relay2Pin) == HIGH;

  // --- RELAY CONTROL LOGIC (With Bounce Protection) ---
  
  // Condition 1: Signal goes ABOVE upper threshold
  if (voltage3 > voltage1) {
    turningOffSeq = false; 
    
    if (!r1IsOn) {
      digitalWrite(relay1Pin, HIGH);
      r1IsOn = true;            
      relayTimer = millis();    
      turningOnSeq = true;      
    } 
    else if (!r2IsOn && !turningOnSeq) {
      relayTimer = millis();    
      turningOnSeq = true;      
    }
  } 
  
  // Condition 2: Signal goes BELOW lower threshold
  else if (voltage3 < voltage2) {
    turningOnSeq = false; 
    
    if (r2IsOn) {
      digitalWrite(relay2Pin, LOW);
      r2IsOn = false;           
      relayTimer = millis();    
      turningOffSeq = true;     
    } 
    else if (r1IsOn && !turningOffSeq) {
        relayTimer = millis();
        turningOffSeq = true;
    }
  }

  // --- TIMER LOGIC (Non-Blocking) ---
  
  if (turningOnSeq && (millis() - relayTimer >= delayTime)) {
    digitalWrite(relay2Pin, HIGH);
    turningOnSeq = false; // Sequence complete
  }
  
  if (turningOffSeq && (millis() - relayTimer >= delayTime)) {
    digitalWrite(relay1Pin, LOW);
    turningOffSeq = false; // Sequence complete
  }

  // --- SERIAL MONITOR OUTPUT ---
  
  Serial.print("Up: "); Serial.print(voltage1);
  Serial.print("V | Low: "); Serial.print(voltage2);
  Serial.print("V | Sig(A2): "); Serial.print(voltage3);
  Serial.print("V | Pressure: "); Serial.print(calculated_pressure);
  
  // Print Relay States
  Serial.print(" || R1: "); 
  Serial.print(digitalRead(relay1Pin) == HIGH ? "ON" : "OFF");
  Serial.print(" | R2: "); 
  Serial.print(digitalRead(relay2Pin) == HIGH ? "ON" : "OFF");

  // --- LIVE COUNTDOWN TIMER ---
  
  if (turningOnSeq) {
      int secondsLeft = (delayTime - (millis() - relayTimer)) / 1000;
      if (secondsLeft < 0) secondsLeft = 0; 
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
  
  // Delay for readability
  delay(500); 
}