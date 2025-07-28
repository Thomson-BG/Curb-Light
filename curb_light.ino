/*
 * BMW 530i-Style Smart Curb Light System
 * 
 * Enhanced Arduino-based curb light controller that emulates BMW 530i functionality
 * Features: GPS-based activation, smooth PWM fading, welcome/goodbye sequences,
 * puddle lighting, comprehensive diagnostics, and fail-safe operation
 * 
 * Version: 2.0
 * Author: Enhanced for production use
 * License: MIT
 */

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <EEPROM.h>

// ================================
// PIN DEFINITIONS
// ================================
#define GPS_RX_PIN 2                 // Arduino RX (GPS TX)
#define LEFT_SIGNAL_PIN 3            // Left turn signal input (optocoupler)
#define RIGHT_SIGNAL_PIN 4           // Right turn signal input (optocoupler)
#define LEFT_CURB_LIGHT_PIN 5        // Left curb light PWM output
#define RIGHT_CURB_LIGHT_PIN 6       // Right curb light PWM output  
#define DASH_TOGGLE_PIN 7            // Manual override button
#define DASH_INDICATOR_LED 8         // Status indicator LED
#define AMBIENT_LIGHT_SENSOR 9       // Ambient light sensor (optional)
#define DOOR_SENSOR_PIN 10           // Door open sensor (optional)
#define HAZARD_SIGNAL_PIN 11         // Hazard light input (optional)
#define STATUS_LED_PIN 13            // Built-in LED for system status

// ================================
// CONFIGURATION CONSTANTS
// ================================
#define SPEED_THRESHOLD 20.0         // Activation speed threshold (MPH)
#define CURB_LIGHT_TIMEOUT 5000      // Light timeout after signal stops (ms)
#define WELCOME_SEQUENCE_DURATION 3000 // Welcome light duration (ms)
#define GOODBYE_SEQUENCE_DURATION 2000 // Goodbye light duration (ms)

// Fade settings for smooth BMW-style transitions
#define FADE_STEP_FAST 10            // Fast fade step (startup/shutdown)
#define FADE_STEP_SLOW 3             // Slow fade step (normal operation)
#define FADE_DELAY_FAST 8            // Fast fade delay (ms)
#define FADE_DELAY_SLOW 15           // Slow fade delay (ms)

// Brightness levels
#define MAX_BRIGHTNESS 255           // Maximum LED brightness
#define WELCOME_BRIGHTNESS 180       // Welcome sequence brightness
#define PUDDLE_BRIGHTNESS 120        // Puddle light brightness
#define MIN_BRIGHTNESS 20            // Minimum operational brightness

// Timing constants
#define GPS_TIMEOUT 10000            // GPS fix timeout (ms)
#define DEBOUNCE_DELAY 50            // Button debounce delay (ms)
#define DIAGNOSTIC_INTERVAL 30000    // Self-diagnostic interval (ms)
#define HEARTBEAT_INTERVAL 1000      // Status LED heartbeat (ms)

// ================================
// EEPROM ADDRESSES
// ================================
#define EEPROM_CONFIG_VERSION 0      // Configuration version
#define EEPROM_BRIGHTNESS 1          // Saved brightness level
#define EEPROM_FADE_SPEED 2          // Saved fade speed
#define EEPROM_FEATURES_ENABLED 3    // Feature enable flags

// ================================
// SYSTEM STATE VARIABLES
// ================================
SoftwareSerial gpsSerial(GPS_RX_PIN, 255);  // GPS communication
TinyGPSPlus gps;                             // GPS parser

// Light control variables
struct LightState {
  uint8_t fadeValue;
  bool active;
  bool fadingIn;
  bool fadingOut;
  unsigned long lastSignalTime;
  unsigned long sequenceStartTime;
};

LightState leftLight = {0, false, false, false, 0, 0};
LightState rightLight = {0, false, false, false, 0, 0};

// System state variables
bool manualToggle = false;
bool lastButtonState = HIGH;
bool systemEnabled = true;
bool diagnosticMode = false;
bool welcomeSequenceActive = false;
bool goodbyeSequenceActive = false;

// Timing variables
unsigned long lastDiagnosticTime = 0;
unsigned long lastHeartbeatTime = 0;
unsigned long lastGPSFixTime = 0;
unsigned long buttonPressTime = 0;
unsigned long systemStartTime = 0;

// Configuration variables (can be saved to EEPROM)
uint8_t maxBrightness = MAX_BRIGHTNESS;
uint8_t fadeSpeed = FADE_STEP_SLOW;
bool welcomeSequenceEnabled = true;
bool puddleLightEnabled = true;
bool doorActivationEnabled = false;

// Error tracking
uint8_t errorFlags = 0;
#define ERROR_GPS_TIMEOUT     0x01
#define ERROR_SIGNAL_FAILURE  0x02
#define ERROR_POWER_ISSUE     0x04
#define ERROR_SENSOR_FAILURE  0x08

// ================================
// SYSTEM MODES
// ================================
enum SystemMode {
  MODE_NORMAL,
  MODE_WELCOME,
  MODE_GOODBYE,
  MODE_PUDDLE,
  MODE_DIAGNOSTIC,
  MODE_ERROR
};

SystemMode currentMode = MODE_NORMAL;

// ================================
// SETUP FUNCTION
// ================================
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  
  // Initialize system start time
  systemStartTime = millis();
  
  // Configure pins
  pinMode(LEFT_SIGNAL_PIN, INPUT_PULLUP);
  pinMode(RIGHT_SIGNAL_PIN, INPUT_PULLUP);
  pinMode(DASH_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(HAZARD_SIGNAL_PIN, INPUT_PULLUP);
  
  pinMode(LEFT_CURB_LIGHT_PIN, OUTPUT);
  pinMode(RIGHT_CURB_LIGHT_PIN, OUTPUT);
  pinMode(DASH_INDICATOR_LED, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Initialize outputs to safe state
  analogWrite(LEFT_CURB_LIGHT_PIN, 0);
  analogWrite(RIGHT_CURB_LIGHT_PIN, 0);
  digitalWrite(DASH_INDICATOR_LED, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Load configuration from EEPROM
  loadConfiguration();
  
  // Perform system self-test
  performSelfTest();
  
  // Start welcome sequence if enabled
  if (welcomeSequenceEnabled) {
    startWelcomeSequence();
  }
  
  Serial.println(F("BMW-Style Curb Light System v2.0 Initialized"));
  Serial.print(F("System features: Welcome="));
  Serial.print(welcomeSequenceEnabled ? "ON" : "OFF");
  Serial.print(F(", Puddle="));
  Serial.print(puddleLightEnabled ? "ON" : "OFF");
  Serial.print(F(", Door="));
  Serial.println(doorActivationEnabled ? "ON" : "OFF");
}

// ================================
// MAIN LOOP
// ================================
void loop() {
  unsigned long currentTime = millis();
  
  // Update GPS data
  updateGPS();
  
  // Handle system heartbeat
  updateHeartbeat(currentTime);
  
  // Check for diagnostic mode activation
  checkDiagnosticMode();
  
  // Run periodic diagnostics
  if (currentTime - lastDiagnosticTime > DIAGNOSTIC_INTERVAL) {
    performDiagnostics();
    lastDiagnosticTime = currentTime;
  }
  
  // Handle different system modes
  switch (currentMode) {
    case MODE_WELCOME:
      handleWelcomeSequence(currentTime);
      break;
      
    case MODE_GOODBYE:
      handleGoodbyeSequence(currentTime);
      break;
      
    case MODE_PUDDLE:
      handlePuddleLightMode(currentTime);
      break;
      
    case MODE_DIAGNOSTIC:
      handleDiagnosticMode(currentTime);
      break;
      
    case MODE_ERROR:
      handleErrorMode(currentTime);
      break;
      
    default: // MODE_NORMAL
      handleNormalOperation(currentTime);
      break;
  }
  
  // Update dash indicator
  updateDashIndicator();
  
  // Handle serial commands for configuration (if available)
  handleSerialCommands();
}

// ================================
// GPS HANDLING FUNCTIONS
// ================================
void updateGPS() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        lastGPSFixTime = millis();
        errorFlags &= ~ERROR_GPS_TIMEOUT; // Clear GPS error
      }
    }
  }
  
  // Check for GPS timeout
  if (millis() - lastGPSFixTime > GPS_TIMEOUT && lastGPSFixTime != 0) {
    errorFlags |= ERROR_GPS_TIMEOUT;
  }
}

float getCurrentSpeed() {
  if (gps.location.isValid() && gps.speed.isValid()) {
    return gps.speed.mph();
  }
  return 0.0; // Return 0 if no valid GPS data (safer default)
}

// ================================
// SYSTEM MODE HANDLERS
// ================================
void handleNormalOperation(unsigned long currentTime) {
  float speed_mph = getCurrentSpeed();
  
  // Read inputs
  bool leftSignal = digitalRead(LEFT_SIGNAL_PIN) == LOW;
  bool rightSignal = digitalRead(RIGHT_SIGNAL_PIN) == LOW;
  bool hazardSignal = digitalRead(HAZARD_SIGNAL_PIN) == LOW;
  bool doorOpen = digitalRead(DOOR_SENSOR_PIN) == LOW;
  
  // Handle manual toggle button
  handleManualToggle();
  
  // Handle door activation (if enabled)
  if (doorActivationEnabled && doorOpen && speed_mph < 2.0) {
    startPuddleLightMode();
    return;
  }
  
  // Handle hazard lights (both sides)
  if (hazardSignal && speed_mph < SPEED_THRESHOLD) {
    activateLight(&leftLight, currentTime, WELCOME_BRIGHTNESS);
    activateLight(&rightLight, currentTime, WELCOME_BRIGHTNESS);
    return;
  }
  
  // Handle normal turn signal operation
  if (speed_mph < SPEED_THRESHOLD || manualToggle) {
    if (leftSignal || manualToggle) {
      activateLight(&leftLight, currentTime, maxBrightness);
    }
    if (rightSignal || manualToggle) {
      activateLight(&rightLight, currentTime, maxBrightness);
    }
  }
  
  // Update light states
  updateLightFading(&leftLight, LEFT_CURB_LIGHT_PIN, currentTime);
  updateLightFading(&rightLight, RIGHT_CURB_LIGHT_PIN, currentTime);
}

void handleWelcomeSequence(unsigned long currentTime) {
  if (currentTime - leftLight.sequenceStartTime > WELCOME_SEQUENCE_DURATION) {
    currentMode = MODE_NORMAL;
    Serial.println(F("Welcome sequence complete"));
    return;
  }
  
  // Create smooth welcome fade pattern
  uint8_t progress = map(currentTime - leftLight.sequenceStartTime, 0, WELCOME_SEQUENCE_DURATION, 0, 255);
  uint8_t brightness = (sin(progress * PI / 255.0) * WELCOME_BRIGHTNESS);
  
  analogWrite(LEFT_CURB_LIGHT_PIN, brightness);
  analogWrite(RIGHT_CURB_LIGHT_PIN, brightness);
}

void handleGoodbyeSequence(unsigned long currentTime) {
  if (currentTime - leftLight.sequenceStartTime > GOODBYE_SEQUENCE_DURATION) {
    currentMode = MODE_NORMAL;
    analogWrite(LEFT_CURB_LIGHT_PIN, 0);
    analogWrite(RIGHT_CURB_LIGHT_PIN, 0);
    Serial.println(F("Goodbye sequence complete"));
    return;
  }
  
  // Create smooth goodbye fade pattern
  uint8_t progress = map(currentTime - leftLight.sequenceStartTime, 0, GOODBYE_SEQUENCE_DURATION, 255, 0);
  uint8_t brightness = progress * WELCOME_BRIGHTNESS / 255;
  
  analogWrite(LEFT_CURB_LIGHT_PIN, brightness);
  analogWrite(RIGHT_CURB_LIGHT_PIN, brightness);
}

void handlePuddleLightMode(unsigned long currentTime) {
  // Puddle lights stay on for a fixed duration or until door closes
  bool doorOpen = digitalRead(DOOR_SENSOR_PIN) == LOW;
  
  if (!doorOpen || (currentTime - leftLight.sequenceStartTime > 10000)) { // 10 second timeout
    currentMode = MODE_NORMAL;
    Serial.println(F("Puddle light mode ended"));
    return;
  }
  
  // Maintain steady puddle light brightness
  analogWrite(LEFT_CURB_LIGHT_PIN, PUDDLE_BRIGHTNESS);
  analogWrite(RIGHT_CURB_LIGHT_PIN, PUDDLE_BRIGHTNESS);
}
// ================================
// LIGHT CONTROL FUNCTIONS
// ================================
void activateLight(LightState* light, unsigned long currentTime, uint8_t targetBrightness) {
  light->lastSignalTime = currentTime;
  if (!light->active) {
    light->fadingIn = true;
    light->fadingOut = false;
    light->active = true;
  }
}

void updateLightFading(LightState* light, uint8_t pin, unsigned long currentTime) {
  bool shouldBeOn = manualToggle || 
                   (light->active && (currentTime - light->lastSignalTime <= CURB_LIGHT_TIMEOUT));
  
  if (shouldBeOn) {
    // Fade in
    if (light->fadingIn && light->fadeValue < maxBrightness) {
      light->fadeValue = min(light->fadeValue + fadeSpeed, maxBrightness);
      analogWrite(pin, light->fadeValue);
      delay(FADE_DELAY_SLOW);
      if (light->fadeValue >= maxBrightness) {
        light->fadingIn = false;
      }
    } else if (!light->fadingIn) {
      light->fadeValue = maxBrightness;
      analogWrite(pin, light->fadeValue);
    }
    light->fadingOut = false;
  } else {
    // Fade out
    if (light->fadeValue > 0) {
      light->fadingOut = true;
      light->fadingIn = false;
      light->fadeValue = max(light->fadeValue - fadeSpeed, 0);
      analogWrite(pin, light->fadeValue);
      delay(FADE_DELAY_SLOW);
    } else {
      light->fadeValue = 0;
      analogWrite(pin, 0);
      light->fadingOut = false;
      light->active = false;
    }
  }
}

void handleManualToggle() {
  bool buttonState = digitalRead(DASH_TOGGLE_PIN);
  
  // Debounce button
  if (buttonState == LOW && lastButtonState == HIGH && 
      (millis() - buttonPressTime > DEBOUNCE_DELAY)) {
    manualToggle = !manualToggle;
    buttonPressTime = millis();
    
    Serial.print(F("Manual toggle: "));
    Serial.println(manualToggle ? "ON" : "OFF");
    
    // Long press (>2 seconds) enters configuration mode
    if (millis() - buttonPressTime > 2000) {
      enterConfigurationMode();
    }
  }
  lastButtonState = buttonState;
}

// ================================
// SEQUENCE CONTROL FUNCTIONS
// ================================
void startWelcomeSequence() {
  currentMode = MODE_WELCOME;
  leftLight.sequenceStartTime = millis();
  rightLight.sequenceStartTime = leftLight.sequenceStartTime;
  Serial.println(F("Welcome sequence started"));
}

void startGoodbyeSequence() {
  currentMode = MODE_GOODBYE;
  leftLight.sequenceStartTime = millis();
  rightLight.sequenceStartTime = leftLight.sequenceStartTime;
  Serial.println(F("Goodbye sequence started"));
}

void startPuddleLightMode() {
  if (!puddleLightEnabled) return;
  
  currentMode = MODE_PUDDLE;
  leftLight.sequenceStartTime = millis();
  rightLight.sequenceStartTime = leftLight.sequenceStartTime;
  Serial.println(F("Puddle light mode activated"));
}

// ================================
// DIAGNOSTIC AND SYSTEM FUNCTIONS
// ================================
void performSelfTest() {
  Serial.println(F("Performing system self-test..."));
  
  // Test all outputs
  digitalWrite(STATUS_LED_PIN, HIGH);
  digitalWrite(DASH_INDICATOR_LED, HIGH);
  analogWrite(LEFT_CURB_LIGHT_PIN, 128);
  analogWrite(RIGHT_CURB_LIGHT_PIN, 128);
  delay(500);
  
  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(DASH_INDICATOR_LED, LOW);
  analogWrite(LEFT_CURB_LIGHT_PIN, 0);
  analogWrite(RIGHT_CURB_LIGHT_PIN, 0);
  
  // Test inputs
  bool leftSignalOK = digitalRead(LEFT_SIGNAL_PIN) == HIGH; // Should be HIGH when not active
  bool rightSignalOK = digitalRead(RIGHT_SIGNAL_PIN) == HIGH;
  bool buttonOK = digitalRead(DASH_TOGGLE_PIN) == HIGH;
  
  if (!leftSignalOK) errorFlags |= ERROR_SIGNAL_FAILURE;
  if (!rightSignalOK) errorFlags |= ERROR_SIGNAL_FAILURE;
  
  Serial.print(F("Self-test complete. Errors: 0x"));
  Serial.println(errorFlags, HEX);
}

void performDiagnostics() {
  // Check GPS status
  if (errorFlags & ERROR_GPS_TIMEOUT) {
    Serial.println(F("WARNING: GPS signal lost"));
  }
  
  // Check signal integrity
  // Add more diagnostic checks as needed
  
  // Report system status
  if (errorFlags == 0) {
    Serial.println(F("System status: All OK"));
  } else {
    Serial.print(F("System errors detected: 0x"));
    Serial.println(errorFlags, HEX);
  }
}

void checkDiagnosticMode() {
  // Enter diagnostic mode with special button combination
  // Hold button for 5+ seconds while turn signals are active
  bool leftSignal = digitalRead(LEFT_SIGNAL_PIN) == LOW;
  bool rightSignal = digitalRead(RIGHT_SIGNAL_PIN) == LOW;
  bool buttonPressed = digitalRead(DASH_TOGGLE_PIN) == LOW;
  
  static unsigned long diagnosticStartTime = 0;
  
  if (buttonPressed && leftSignal && rightSignal) {
    if (diagnosticStartTime == 0) {
      diagnosticStartTime = millis();
    } else if (millis() - diagnosticStartTime > 5000) {
      currentMode = MODE_DIAGNOSTIC;
      diagnosticStartTime = 0;
      Serial.println(F("Diagnostic mode activated"));
    }
  } else {
    diagnosticStartTime = 0;
  }
}

void handleDiagnosticMode(unsigned long currentTime) {
  // Flash status LED to indicate diagnostic mode
  if (currentTime % 500 < 250) {
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    digitalWrite(STATUS_LED_PIN, LOW);
  }
  
  // Cycle through light tests
  uint8_t testPhase = (currentTime / 1000) % 4;
  
  switch (testPhase) {
    case 0: // Test left light
      analogWrite(LEFT_CURB_LIGHT_PIN, 255);
      analogWrite(RIGHT_CURB_LIGHT_PIN, 0);
      break;
    case 1: // Test right light
      analogWrite(LEFT_CURB_LIGHT_PIN, 0);
      analogWrite(RIGHT_CURB_LIGHT_PIN, 255);
      break;
    case 2: // Test both lights
      analogWrite(LEFT_CURB_LIGHT_PIN, 255);
      analogWrite(RIGHT_CURB_LIGHT_PIN, 255);
      break;
    case 3: // All off
      analogWrite(LEFT_CURB_LIGHT_PIN, 0);
      analogWrite(RIGHT_CURB_LIGHT_PIN, 0);
      break;
  }
  
  // Exit diagnostic mode with button press
  if (digitalRead(DASH_TOGGLE_PIN) == LOW) {
    currentMode = MODE_NORMAL;
    analogWrite(LEFT_CURB_LIGHT_PIN, 0);
    analogWrite(RIGHT_CURB_LIGHT_PIN, 0);
    digitalWrite(STATUS_LED_PIN, LOW);
    Serial.println(F("Exiting diagnostic mode"));
    delay(1000); // Debounce
  }
}

void handleErrorMode(unsigned long currentTime) {
  // Flash indicator LED rapidly to show error state
  if (currentTime % 200 < 100) {
    digitalWrite(DASH_INDICATOR_LED, HIGH);
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    digitalWrite(DASH_INDICATOR_LED, LOW);
    digitalWrite(STATUS_LED_PIN, LOW);
  }
  
  // Try to recover from errors
  if (errorFlags & ERROR_GPS_TIMEOUT) {
    // Attempt GPS recovery
    gpsSerial.begin(9600);
  }
  
  // Auto-exit error mode after 30 seconds if errors clear
  static unsigned long errorStartTime = 0;
  if (errorStartTime == 0) errorStartTime = currentTime;
  
  if (errorFlags == 0 || (currentTime - errorStartTime > 30000)) {
    currentMode = MODE_NORMAL;
    errorStartTime = 0;
    Serial.println(F("Recovered from error mode"));
  }
}

void updateHeartbeat(unsigned long currentTime) {
  if (currentTime - lastHeartbeatTime > HEARTBEAT_INTERVAL) {
    // Quick flash of status LED to show system is alive
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(50);
    digitalWrite(STATUS_LED_PIN, LOW);
    lastHeartbeatTime = currentTime;
  }
}

void updateDashIndicator() {
  // Dash indicator shows when any light is active
  bool anyLightOn = (leftLight.fadeValue > 0) || (rightLight.fadeValue > 0);
  digitalWrite(DASH_INDICATOR_LED, anyLightOn ? HIGH : LOW);
}

// ================================
// CONFIGURATION FUNCTIONS
// ================================
void loadConfiguration() {
  // Check if EEPROM contains valid configuration
  if (EEPROM.read(EEPROM_CONFIG_VERSION) == 1) {
    maxBrightness = EEPROM.read(EEPROM_BRIGHTNESS);
    fadeSpeed = EEPROM.read(EEPROM_FADE_SPEED);
    uint8_t features = EEPROM.read(EEPROM_FEATURES_ENABLED);
    
    welcomeSequenceEnabled = (features & 0x01) != 0;
    puddleLightEnabled = (features & 0x02) != 0;
    doorActivationEnabled = (features & 0x04) != 0;
    
    Serial.println(F("Configuration loaded from EEPROM"));
  } else {
    // Use defaults and save to EEPROM
    saveConfiguration();
    Serial.println(F("Default configuration saved to EEPROM"));
  }
}

void saveConfiguration() {
  EEPROM.write(EEPROM_CONFIG_VERSION, 1);
  EEPROM.write(EEPROM_BRIGHTNESS, maxBrightness);
  EEPROM.write(EEPROM_FADE_SPEED, fadeSpeed);
  
  uint8_t features = 0;
  if (welcomeSequenceEnabled) features |= 0x01;
  if (puddleLightEnabled) features |= 0x02;
  if (doorActivationEnabled) features |= 0x04;
  
  EEPROM.write(EEPROM_FEATURES_ENABLED, features);
  Serial.println(F("Configuration saved to EEPROM"));
}

void enterConfigurationMode() {
  Serial.println(F("=== CONFIGURATION MODE ==="));
  Serial.println(F("Send commands via Serial:"));
  Serial.println(F("b<value> - Set brightness (0-255)"));
  Serial.println(F("f<value> - Set fade speed (1-20)"));
  Serial.println(F("w<0/1> - Enable/disable welcome sequence"));
  Serial.println(F("p<0/1> - Enable/disable puddle lights"));
  Serial.println(F("d<0/1> - Enable/disable door activation"));
  Serial.println(F("s - Save configuration"));
  Serial.println(F("x - Exit configuration mode"));
  
  // Configuration mode loop
  while (true) {
    if (Serial.available()) {
      String command = Serial.readString();
      command.trim();
      
      if (command.startsWith("b")) {
        maxBrightness = constrain(command.substring(1).toInt(), 0, 255);
        Serial.print(F("Brightness set to: "));
        Serial.println(maxBrightness);
      }
      else if (command.startsWith("f")) {
        fadeSpeed = constrain(command.substring(1).toInt(), 1, 20);
        Serial.print(F("Fade speed set to: "));
        Serial.println(fadeSpeed);
      }
      else if (command.startsWith("w")) {
        welcomeSequenceEnabled = command.substring(1).toInt() != 0;
        Serial.print(F("Welcome sequence: "));
        Serial.println(welcomeSequenceEnabled ? "ENABLED" : "DISABLED");
      }
      else if (command.startsWith("p")) {
        puddleLightEnabled = command.substring(1).toInt() != 0;
        Serial.print(F("Puddle lights: "));
        Serial.println(puddleLightEnabled ? "ENABLED" : "DISABLED");
      }
      else if (command.startsWith("d")) {
        doorActivationEnabled = command.substring(1).toInt() != 0;
        Serial.print(F("Door activation: "));
        Serial.println(doorActivationEnabled ? "ENABLED" : "DISABLED");
      }
      else if (command == "s") {
        saveConfiguration();
      }
      else if (command == "x") {
        Serial.println(F("Exiting configuration mode"));
        break;
      }
    }
  }
}

void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readString();
    command.trim();
    
    if (command == "status") {
      printSystemStatus();
    }
    else if (command == "test") {
      performSelfTest();
    }
    else if (command == "welcome") {
      startWelcomeSequence();
    }
    else if (command == "goodbye") {
      startGoodbyeSequence();
    }
    else if (command == "config") {
      enterConfigurationMode();
    }
  }
}

void printSystemStatus() {
  Serial.println(F("=== SYSTEM STATUS ==="));
  Serial.print(F("Uptime: "));
  Serial.print((millis() - systemStartTime) / 1000);
  Serial.println(F(" seconds"));
  
  Serial.print(F("Current mode: "));
  switch (currentMode) {
    case MODE_NORMAL: Serial.println(F("Normal")); break;
    case MODE_WELCOME: Serial.println(F("Welcome Sequence")); break;
    case MODE_GOODBYE: Serial.println(F("Goodbye Sequence")); break;
    case MODE_PUDDLE: Serial.println(F("Puddle Light")); break;
    case MODE_DIAGNOSTIC: Serial.println(F("Diagnostic")); break;
    case MODE_ERROR: Serial.println(F("Error")); break;
  }
  
  Serial.print(F("GPS Status: "));
  if (gps.location.isValid()) {
    Serial.print(F("LOCKED ("));
    Serial.print(gps.speed.mph());
    Serial.println(F(" mph)"));
  } else {
    Serial.println(F("NO FIX"));
  }
  
  Serial.print(F("Left Light: "));
  Serial.print(leftLight.fadeValue);
  Serial.print(F(" | Right Light: "));
  Serial.println(rightLight.fadeValue);
  
  Serial.print(F("Error Flags: 0x"));
  Serial.println(errorFlags, HEX);
  
  Serial.print(F("Configuration - Brightness: "));
  Serial.print(maxBrightness);
  Serial.print(F(", Fade Speed: "));
  Serial.print(fadeSpeed);
  Serial.print(F(", Features: W="));
  Serial.print(welcomeSequenceEnabled ? "1" : "0");
  Serial.print(F(" P="));
  Serial.print(puddleLightEnabled ? "1" : "0");
  Serial.print(F(" D="));
  Serial.println(doorActivationEnabled ? "1" : "0");
  Serial.println(F("===================="));
}