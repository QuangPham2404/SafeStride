#include "HX711.h"

/* =======================
   HX711 LOAD CELL SECTION
   ======================= */

#define DOUT 8
#define SCK  9
#define LED_PIN 7
#define OVERIDE_BUTTON_PIN A5

HX711 scale;

float calibration_factor = -200000.0;

// Detection parameters
const float NO_HOLD_THRESHOLD = 0.015;
const float HOLD_THRESHOLD    = 0.015;
const int   CONFIRM_COUNT     = 3;
const int   OVERIDE_BUTTON_THRESH = 50;

float filtered = 0.0;
int hold_counter = 0;
bool isHolding = false;
bool overide_button = false;


/* =======================
   ULTRASONIC + BUZZER
   ======================= */

int trigPin = 11;
int echoPin = 12;
long duration, cm, inches;

int buzzerPin = 10;

const int DISTANCE_THRESH = 5;
const int CLOSEST_DIST = 15;
const int MEDIUM_DIST = 30;
const int FURTHEST_DIST = 50;

const int CLOSEST_FREQ = 60;
const int MEDIUM_FREQ  = 120;
const int FURTHEST_FREQ = 250;


// Non-blocking buzzer pulse function
void buzzer_pulsate(int buzzerPin, unsigned long pulseInterval, bool enable)
{
  static unsigned long past_time = 0;
  static bool buzzerState = false;
  static unsigned long lastInterval = 0;

  unsigned long current_time = millis();

  if (!enable) {
    buzzerState = false;
    past_time = current_time;
    lastInterval = 0;
    digitalWrite(buzzerPin, LOW);
    return;
  }

  // Reset timing if interval changes
  if (pulseInterval != lastInterval) {
    past_time = current_time;
    lastInterval = pulseInterval;
  }

  if (current_time - past_time >= pulseInterval) {
    past_time = current_time;
    buzzerState = !buzzerState;
    digitalWrite(buzzerPin, buzzerState);
  }
}


/* =======================
   SETUP
   ======================= */

void setup() {
  Serial.begin(115200);

  // HX711
  scale.begin(DOUT, SCK);
  scale.set_scale(calibration_factor);
  scale.tare();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // Ultrasonic + buzzer
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Stabilizing...");
  delay(5000);
  Serial.println("Ready");
}


/* =======================
   LOOP
   ======================= */

void loop() {

  /* ---------- HX711 LOGIC ---------- */

  // FIX: use single reading to reduce blocking time
  long raw = scale.read();          
  float w = scale.get_units(1);     

  filtered = 0.3 * filtered + 0.7 * w;
  float absF = abs(filtered);

  int overide_button_input = analogRead(OVERIDE_BUTTON_PIN);

  Serial.print("RAW: ");
  Serial.print(raw);
  Serial.print(" | W: ");
  Serial.print(w, 4);
  Serial.print(" kg | F: ");
  Serial.print(filtered, 4);
  Serial.print(" kg | STATE: ");
  Serial.print(isHolding ? "HOLD" : "FREE");
  Serial.print(" | OVERIDE: ");
  Serial.print(overide_button);
  Serial.print(" | overide_signal: ");
  Serial.println(overide_button_input);

  if ((overide_button_input > OVERIDE_BUTTON_THRESH) && (!overide_button)) {
    overide_button = true;
  }

  if (overide_button) {
    digitalWrite(LED_PIN, LOW);
  }

  if (!isHolding) {
    if (absF > HOLD_THRESHOLD) {
      hold_counter++;
      if (hold_counter >= CONFIRM_COUNT) {
        isHolding = true;
        hold_counter = 0;
        digitalWrite(LED_PIN, LOW);
        Serial.println(">>> HOLD DETECTED");
        overide_button = false;
      }
    } else {
      hold_counter = 0;
    }
  } else {
    if ((absF < NO_HOLD_THRESHOLD) && (!overide_button)) {
      hold_counter++;
      if (hold_counter >= CONFIRM_COUNT) {
        isHolding = false;
        hold_counter = 0;
        digitalWrite(LED_PIN, HIGH);
        Serial.println(">>> RELEASE DETECTED");
      }
    } else {
      hold_counter = 0;
    }
  }


  /* ---------- ULTRASONIC + BUZZER LOGIC ---------- */

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  cm = (duration / 2) / 29.1;
  inches = (duration / 2) / 74;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.println("cm");

  if (cm <= CLOSEST_DIST) {
    buzzer_pulsate(buzzerPin, CLOSEST_FREQ, true);
  }
  else if (cm <= MEDIUM_DIST) {
    buzzer_pulsate(buzzerPin, MEDIUM_FREQ, true);
  }
  else if (cm <= FURTHEST_DIST) {
    buzzer_pulsate(buzzerPin, FURTHEST_FREQ, true);
  }
  else {
    buzzer_pulsate(buzzerPin, 250, false);
  }

  // FIX: faster loop to allow short pulse intervals
  delay(10);
}
