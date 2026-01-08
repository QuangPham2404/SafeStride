#include "HX711.h"

#define DOUT 4
#define SCK  5
#define LED_PIN 8

HX711 scale;

float calibration_factor = -200000.0;

// Detection parameters
const float NO_HOLD_THRESHOLD = 0.015;
const float HOLD_THRESHOLD    = 0.020;
const int   CONFIRM_COUNT     = 5;

float filtered = 0.0;
int hold_counter = 0;
bool isHolding = false;

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT, SCK);

  scale.set_scale(calibration_factor);
  scale.tare();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // FREE = LED ON

  Serial.println("Stabilizing...");
  delay(5000);
  Serial.println("Ready");
}

void loop() {
  long raw = scale.read_average(5);
  float w = scale.get_units(5);

  filtered = 0.3 * filtered + 0.7 * w;

  Serial.print("RAW: ");
  Serial.print(raw);
  Serial.print(" | W: ");
  Serial.print(w, 4);
  Serial.print(" kg | F: ");
  Serial.print(filtered, 4);
  Serial.print(" kg | STATE: ");
  Serial.println(isHolding ? "HOLD" : "FREE");

  float absF = abs(filtered);  // <-- apply absolute value for thresholds

  if (!isHolding) {
    if (absF > HOLD_THRESHOLD) {   // use absF
      hold_counter++;
      if (hold_counter >= CONFIRM_COUNT) {
        isHolding = true;
        hold_counter = 0;
        digitalWrite(LED_PIN, LOW);   // HOLD = LED OFF
        Serial.println(">>> HOLD DETECTED");
      }
    } else {
      hold_counter = 0;
    }
  } else {
    if (absF < NO_HOLD_THRESHOLD) {  // use absF
      hold_counter++;
      if (hold_counter >= CONFIRM_COUNT) {
        isHolding = false;
        hold_counter = 0;
        digitalWrite(LED_PIN, HIGH);  // FREE = LED ON
        Serial.println(">>> RELEASE DETECTED");
      }
    } else {
      hold_counter = 0;
    }
  }

  delay(50);
}
