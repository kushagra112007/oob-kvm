#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// --- WI-FI CREDENTIALS ---
// REPLACE THESE PLACEHOLDERS BEFORE COMPILING
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- PIN & PULSE DEFINITIONS ---
const int SERVO_PIN = D4; // Yellow/Orange signal wire on D4
const int IR_PIN    = D2; // IR Sensor signal wire on D2

// SG90 Extended Pulse Bounds (Microseconds)
const int MIN_PULSE = 500;  // Forces true 0 deg reach
const int MAX_PULSE = 2400; // Forces true 180 deg reach

// --- IR REMOTE CODES ---
const uint32_t BTN_DOWN = 0xFF4AB5; // DOWN -> 5-Second Power Tap
const uint32_t BTN_UP   = 0xFF18E7; // UP   -> Manual Emergency Release to 180 deg

// --- SERVO ANGLES & TIMINGS ---
const int ANGLE_UP   = 180; // Rest position (9 o'clock)
const int ANGLE_DOWN = 110; // Press position (6 o'clock onto power button)

const int WAKE_DURATION     = 1500;  // 1.5 seconds
const int POWER_ON_DURATION = 5000;  // 5 seconds
const int PANIC_DURATION    = 15000; // 15 seconds

Servo myServo;
IRrecv irrecv(IR_PIN);
decode_results results;
ESP8266WebServer server(80);

// Helper function to handle non-blocking button press with explicit pulse widths
void pressPowerButton(int durationMs, String label) {
  Serial.print("Executing: ");
  Serial.println(label);

  // Attach using explicit min/max pulse limits
  myServo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
  myServo.write(ANGLE_DOWN);
  
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    server.handleClient(); // Keep server listening during holds
    yield();               // Keep Wi-Fi & Watchdog fed to prevent WDT resets
    delay(10);
  }

  myServo.write(ANGLE_UP);
  delay(400);              // Give motor physical time to reach 180 deg
  myServo.detach();        // Stop active PWM pulse to eliminate idle chatter/heat
  Serial.println("Action Complete: Servo returned to 180 deg and detached.");
}

// --- HTTP ROUTE HANDLERS ---
void handleRoot() {
  String html = "<html><head><title>OOB Power Node Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial,sans-serif; text-align:center; margin-top:40px; background:#121212; color:#eee;}";
  html += ".btn{display:inline-block; width:80%; max-width:300px; padding:15px; margin:10px; font-size:18px; color:white; border:none; border-radius:8px; text-decoration:none; cursor:pointer;}";
  html += ".wake{background-color:#2196F3;}";
  html += ".power{background-color:#4CAF50;}";
  html += ".panic{background-color:#f44336;}";
  html += ".release{background-color:#757575;}";
  html += "</style></head><body>";
  html += "<h2>Out-of-Band Power Control</h2>";
  html += "<p><a href='/wake'><button class='btn wake'>Wake Display (1.5s)</button></a></p>";
  html += "<p><a href='/power'><button class='btn power'>Power On (5s)</button></a></p>";
  html += "<p><a href='/panic'><button class='btn panic'>Kernel Panic Reset (15s)</button></a></p>";
  html += "<p><a href='/release'><button class='btn release'>Force Release (180 deg)</button></a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleWake() {
  server.send(200, "text/plain", "Triggering 1.5s Display Wake...");
  pressPowerButton(WAKE_DURATION, "1.5s Display Wake");
}

void handlePower() {
  server.send(200, "text/plain", "Triggering 5s Power On sequence...");
  pressPowerButton(POWER_ON_DURATION, "5s Power On");
}

void handlePanic() {
  server.send(200, "text/plain", "Triggering 15s Kernel Panic Cold Boot...");
  pressPowerButton(PANIC_DURATION, "15s Cold Boot");
}

void handleRelease() {
  myServo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
  myServo.write(ANGLE_UP);
  delay(400);
  myServo.detach();
  server.send(200, "text/plain", "Emergency Release: Moved back to 180 degrees.");
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initial position alignment and release
  myServo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
  myServo.write(ANGLE_UP);
  delay(400);
  myServo.detach();

  // Start IR Receiver on D2
  irrecv.enableIRIn();

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("NodeMCU IP Address: http://");
  Serial.println(WiFi.localIP());

  // Web Server Routes
  server.on("/", handleRoot);
  server.on("/wake", handleWake);
  server.on("/power", handlePower);
  server.on("/panic", handlePanic);
  server.on("/release", handleRelease);

  server.begin();
  Serial.println("Web Server & IR Receiver Ready!");
}

void loop() {
  // Listen for Web HTTP requests
  server.handleClient();

  // Listen for IR Remote Signals
  if (irrecv.decode(&results)) {
    uint32_t code = results.value;

    if (code != 0xFFFFFFFF && code != 0x0) { 
      Serial.print("IR Received: 0x");
      Serial.println(code, HEX);

      if (code == BTN_DOWN) {
        pressPowerButton(POWER_ON_DURATION, "IR Remote 5s Press");
      } 
      else if (code == BTN_UP) {
        myServo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
        myServo.write(ANGLE_UP);
        delay(400);
        myServo.detach();
        Serial.println("IR Remote Action: Moved UP to 180 deg");
      }
    }
    
    irrecv.resume();
  }
}
