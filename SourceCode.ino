#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define MQ135_PIN A0
#define MQ7_PIN A1
#define DUST_SENSOR_PIN A2
#define DUST_LED_PIN 3
#define FAN_PIN 4
#define MIST_PIN 5

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial espSerial(10, 11); // RX (10) <- ESP TX ; TX (11) -> ESP RX

// Put only the ThingSpeak API key (not full URL)
const String apiKey = "VXNHO2YY7Z54OSHS"; // <-- replace with your own key if needed
const char* ssid = "Athish_Kanakaraj";
const char* pass = "12345679";

// --- Thresholds (tune these for your environment) ---
const float TEMP_HIGH_THRESHOLD = 30.0;    // Celsius, above this turn ON fan
const int MQ_THRESHOLD = 400;              // ADC raw value threshold for MQ sensors
const int DUST_THRESHOLD = 300;            // ADC raw threshold for dust -> treat as smoke
const float HUM_LOW_THRESHOLD = 40.0;      // Percent, below this turn ON mist

// Upload interval
const unsigned long UPLOAD_INTERVAL_MS = 20000UL; // 20 seconds

unsigned long lastUpload = 0;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600); // If ESP default is 115200 you'll need to change or reconfigure module
  dht.begin();

  pinMode(DUST_LED_PIN, OUTPUT);
  digitalWrite(DUST_LED_PIN, HIGH); // LED off if active-low

  pinMode(FAN_PIN, OUTPUT);
  pinMode(MIST_PIN, OUTPUT);

  // Initialize actuators OFF (assume active-HIGH turns ON)
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(MIST_PIN, LOW);

  delay(2000); // Allow sensors & ESP to settle

  connectWiFi();
}

void loop() {
  unsigned long now = millis();
  if (now - lastUpload < UPLOAD_INTERVAL_MS) {
    // Still waiting for next cycle; do nothing blocking (but small delay to avoid busy-loop)
    delay(50);
    return;
  }
  lastUpload = now;

  // Read sensors
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int airQuality = analogRead(MQ135_PIN);
  int coLevel = analogRead(MQ7_PIN);
  int dust = readDustSensor();

  // If DHT failed, set NaN handling
  if (isnan(h) || isnan(t)) {
    Serial.println("Warning: DHT read failed.");
  }

  // Control logic (rule-based)
  bool fanShouldBeOn = false;
  bool mistShouldBeOn = false;

  // Rule: if temperature high -> fan ON
  if (!isnan(t) && t >= TEMP_HIGH_THRESHOLD) {
    fanShouldBeOn = true;
    Serial.println("Rule: Temp high -> fan ON");
  }

  // Rule: if MQ sensors exceed threshold -> fan ON
  if (airQuality >= MQ_THRESHOLD) {
    fanShouldBeOn = true;
    Serial.println("Rule: MQ135 high -> fan ON");
  }
  if (coLevel >= MQ_THRESHOLD) {
    fanShouldBeOn = true;
    Serial.println("Rule: MQ7 high -> fan ON");
  }

  // Rule: if dust (treated as smoke) exceeds threshold -> fan ON
  if (dust >= DUST_THRESHOLD) {
    fanShouldBeOn = true;
    Serial.println("Rule: Dust/Smoke detected -> fan ON");
  }

  // Rule: if humidity low -> mist ON
  if (!isnan(h) && h <= HUM_LOW_THRESHOLD) {
    mistShouldBeOn = true;
    Serial.println("Rule: Humidity low -> mist ON");
  }

  // Apply actuator states (assumed active-HIGH: HIGH -> ON)
  digitalWrite(FAN_PIN, fanShouldBeOn ? HIGH : LOW);
  digitalWrite(MIST_PIN, mistShouldBeOn ? HIGH : LOW);

  // Debug prints
  Serial.print("Temp: "); Serial.print(t); Serial.print(" C, ");
  Serial.print("Hum: "); Serial.print(h); Serial.print(" %, ");
  Serial.print("MQ135: "); Serial.print(airQuality); Serial.print(", ");
  Serial.print("MQ7: "); Serial.print(coLevel); Serial.print(", ");
  Serial.print("Dust: "); Serial.println(dust);

  // Upload data to ThingSpeak
  sendData(t, h, airQuality, coLevel, dust, fanShouldBeOn, mistShouldBeOn);
}

// Read dust sensor (GP2Y1010-like)
int readDustSensor() {
  // Pulse LED (active-low) and read analog
  digitalWrite(DUST_LED_PIN, LOW);
  delayMicroseconds(280);
  int dustVal = analogRead(DUST_SENSOR_PIN);
  delayMicroseconds(40);
  digitalWrite(DUST_LED_PIN, HIGH);
  delayMicroseconds(9680);
  return dustVal;
}

// Connect to WiFi (basic)
void connectWiFi() {
  Serial.println("Connecting ESP to WiFi...");
  sendAT("AT+RST", 2000);
  delay(1000);
  sendAT("AT+CWMODE=1", 1000); // Station mode
  delay(500);

  String joinCmd = String("AT+CWJAP=\"") + ssid + String("\",\"") + pass + String("\"");
  sendAT(joinCmd.c_str(), 8000); // longer timeout for join
}

// Build and send HTTP GET to ThingSpeak
void sendData(float temp, float hum, int airQ, int co, int dust, bool fanState, bool mistState) {
  // Build query path
  String path = "/update?api_key=" + apiKey +
                "&field1=" + String(temp) +
                "&field2=" + String(hum) +
                "&field3=" + String(airQ) +
                "&field4=" + String(co) +
                "&field5=" + String(dust) +
                "&field6=" + String(fanState ? 1 : 0) +
                "&field7=" + String(mistState ? 1 : 0);

  // Full HTTP request
  String httpRequest = String("GET ") + path + " HTTP/1.1\r\n" +
                       "Host: api.thingspeak.com\r\n" +
                       "Connection: close\r\n" +
                       "\r\n";

  Serial.println("Connecting to api.thingspeak.com...");
  sendAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80", 4000);
  delay(500);

  // Send exact payload length
  int len = httpRequest.length();
  String cipsend = String("AT+CIPSEND=") + len;
  Serial.println("CIPSEND cmd: " + cipsend);
  sendAT(cipsend.c_str(), 3000);

  // Small delay for '>' prompt; then send payload bytes
  delay(200);
  Serial.println("HTTP request:");
  Serial.print(httpRequest);
  espSerial.print(httpRequest); // send exact bytes (do not println)

  // Read response for a short while and then close
  readEspResponse(3000);
  sendAT("AT+CIPCLOSE", 1000);
}

// Helper: send AT command and print responses for a short timeout
void sendAT(const char* cmd, unsigned long timeout) {
  Serial.print(">> ");
  Serial.println(cmd);
  espSerial.println(cmd);

  unsigned long tStart = millis();
  while (millis() - tStart < timeout) {
    if (espSerial.available()) {
      String resp = espSerial.readStringUntil('\n');
      resp.trim();
      if (resp.length()) {
        Serial.print("ESP: ");
        Serial.println(resp);
      }
    }
  }
}

// Read and print ESP responses for up to timeout ms
void readEspResponse(unsigned long timeout) {
  unsigned long tStart = millis();
  while (millis() - tStart < timeout) {
    if (espSerial.available()) {
      String resp = espSerial.readStringUntil('\n');
      resp.trim();
      if (resp.length()) {
        Serial.print("ESP: ");
        Serial.println(resp);
      }
    }
  }
}
