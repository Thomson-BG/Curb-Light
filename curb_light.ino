#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pin Definitions
#define GPS_RX_PIN 2     // Arduino RX (GPS TX)
#define LEFT_SIGNAL_PIN 3
#define RIGHT_SIGNAL_PIN 4
#define LEFT_CURB_LIGHT_PIN 5    // Use PWM-capable pin
#define RIGHT_CURB_LIGHT_PIN 6   // Use PWM-capable pin
#define DASH_TOGGLE_PIN 7        // Dash toggle button (INPUT_PULLUP)
#define DASH_INDICATOR_LED 8     // Dash indicator LED (OUTPUT)

// Speed threshold in MPH
#define SPEED_THRESHOLD 20.0
#define CURB_LIGHT_TIMEOUT 5000  // 5 seconds in milliseconds

// Fade settings
#define FADE_STEP 5              // 0-255, step size for fading
#define FADE_DELAY 10            // ms delay per fade step

SoftwareSerial gpsSerial(GPS_RX_PIN, 255);  // RX only, TX unused
TinyGPSPlus gps;

unsigned long leftLastSignalTime = 0;
unsigned long rightLastSignalTime = 0;
bool leftCurbActive = false;
bool rightCurbActive = false;

// Light fade states
uint8_t leftFadeValue = 0;
uint8_t rightFadeValue = 0;
bool leftFadingIn = false;
bool leftFadingOut = false;
bool rightFadingIn = false;
bool rightFadingOut = false;

// Toggle state (for latching button)
bool manualToggle = false;
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  pinMode(LEFT_SIGNAL_PIN, INPUT_PULLUP);   // Expect LOW when active (with optocoupler)
  pinMode(RIGHT_SIGNAL_PIN, INPUT_PULLUP);
  pinMode(DASH_TOGGLE_PIN, INPUT_PULLUP);   // Dash toggle button (active LOW)
  pinMode(DASH_INDICATOR_LED, OUTPUT);

  pinMode(LEFT_CURB_LIGHT_PIN, OUTPUT);
  pinMode(RIGHT_CURB_LIGHT_PIN, OUTPUT);

  analogWrite(LEFT_CURB_LIGHT_PIN, 0);
  analogWrite(RIGHT_CURB_LIGHT_PIN, 0);
  digitalWrite(DASH_INDICATOR_LED, LOW);
}

void loop() {
  // Read GPS data
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // Get speed
  float speed_mph = 0;
  if (gps.location.isValid() && gps.speed.isValid()) {
    speed_mph = gps.speed.mph();
  }

  // Read turn signals (active LOW)
  bool left_signal = digitalRead(LEFT_SIGNAL_PIN) == LOW;
  bool right_signal = digitalRead(RIGHT_SIGNAL_PIN) == LOW;

  // --- Dash toggle button logic ---
  bool buttonState = digitalRead(DASH_TOGGLE_PIN);
  // For a momentary button: just use (buttonState == LOW)
  // For a latching (toggle) button:
  if (buttonState == LOW && lastButtonState == HIGH) {
    manualToggle = !manualToggle; // Toggle on button press
  }
  lastButtonState = buttonState;

  unsigned long now = millis();

  // ----------- LEFT LIGHT LOGIC -----------
  // Manual toggle overrides everything else
  if (manualToggle) {
    leftCurbActive = true;
    leftLastSignalTime = now; // Keep timer fresh so fade out doesn't start
    leftFadingIn = true;
    leftFadingOut = false;
  } else if (speed_mph < SPEED_THRESHOLD && left_signal) {
    leftLastSignalTime = now;
    if (!leftCurbActive) {
      leftFadingIn = true;
      leftFadingOut = false;
    }
    leftCurbActive = true;
  }

  if ((manualToggle) ||
      (speed_mph < SPEED_THRESHOLD && leftCurbActive && (now - leftLastSignalTime <= CURB_LIGHT_TIMEOUT))) {
    // Commanded ON: Fade in if not already fully on
    if (leftFadingIn) {
      if (leftFadeValue < 255) {
        leftFadeValue = min(leftFadeValue + FADE_STEP, 255);
        analogWrite(LEFT_CURB_LIGHT_PIN, leftFadeValue);
        delay(FADE_DELAY);
      } else {
        leftFadeValue = 255;
        leftFadingIn = false;
        analogWrite(LEFT_CURB_LIGHT_PIN, leftFadeValue);
      }
    } else {
      // Remain ON at full brightness
      leftFadeValue = 255;
      analogWrite(LEFT_CURB_LIGHT_PIN, leftFadeValue);
    }
    leftFadingOut = false;
  } else {
    // Commanded OFF: Fade out
    if (leftFadeValue > 0) {
      leftFadingOut = true;
      leftFadingIn = false;
      leftFadeValue = max(leftFadeValue - FADE_STEP, 0);
      analogWrite(LEFT_CURB_LIGHT_PIN, leftFadeValue);
      delay(FADE_DELAY);
    } else {
      leftFadeValue = 0;
      analogWrite(LEFT_CURB_LIGHT_PIN, leftFadeValue);
      leftFadingOut = false;
      leftCurbActive = false;
    }
  }

  // ----------- RIGHT LIGHT LOGIC -----------
  if (manualToggle) {
    rightCurbActive = true;
    rightLastSignalTime = now;
    rightFadingIn = true;
    rightFadingOut = false;
  } else if (speed_mph < SPEED_THRESHOLD && right_signal) {
    rightLastSignalTime = now;
    if (!rightCurbActive) {
      rightFadingIn = true;
      rightFadingOut = false;
    }
    rightCurbActive = true;
  }

  if ((manualToggle) ||
      (speed_mph < SPEED_THRESHOLD && rightCurbActive && (now - rightLastSignalTime <= CURB_LIGHT_TIMEOUT))) {
    if (rightFadingIn) {
      if (rightFadeValue < 255) {
        rightFadeValue = min(rightFadeValue + FADE_STEP, 255);
        analogWrite(RIGHT_CURB_LIGHT_PIN, rightFadeValue);
        delay(FADE_DELAY);
      } else {
        rightFadeValue = 255;
        rightFadingIn = false;
        analogWrite(RIGHT_CURB_LIGHT_PIN, rightFadeValue);
      }
    } else {
      rightFadeValue = 255;
      analogWrite(RIGHT_CURB_LIGHT_PIN, rightFadeValue);
    }
    rightFadingOut = false;
  } else {
    if (rightFadeValue > 0) {
      rightFadingOut = true;
      rightFadingIn = false;
      rightFadeValue = max(rightFadeValue - FADE_STEP, 0);
      analogWrite(RIGHT_CURB_LIGHT_PIN, rightFadeValue);
      delay(FADE_DELAY);
    } else {
      rightFadeValue = 0;
      analogWrite(RIGHT_CURB_LIGHT_PIN, rightFadeValue);
      rightFadingOut = false;
      rightCurbActive = false;
    }
  }

  // --- Dash Indicator Logic: ON if either curb light is on (PWM > 0) ---
  if (leftFadeValue > 0 || rightFadeValue > 0) {
    digitalWrite(DASH_INDICATOR_LED, HIGH);
  } else {
    digitalWrite(DASH_INDICATOR_LED, LOW);
  }

  // (Optional) Debug Serial output
  Serial.print("Speed: ");
  Serial.print(speed_mph);
  Serial.print(" mph | L: ");
  Serial.print(left_signal);
  Serial.print(" R: ");
  Serial.print(right_signal);
  Serial.print(" | Toggle: ");
  Serial.print(manualToggle);
  Serial.print(" | Lfade: ");
  Serial.print(leftFadeValue);
  Serial.print(" Rfade: ");
  Serial.print(rightFadeValue);
  Serial.print(" | DashLED: ");
  Serial.println((leftFadeValue > 0 || rightFadeValue > 0));
}
