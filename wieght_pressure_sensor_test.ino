//This script is for ESP32. For Arduino UNO, change pins and OVERIDE_BUTTON_THRESH to 50


#include "HX711.h"

#define DOUT 26
#define SCK  25
#define LED_PIN 4
#define OVERIDE_BUTTON_PIN 34

HX711 scale;

float calibration_factor = -200000.0;

// Detection parameters
const float NO_HOLD_THRESHOLD = 0.015;
const float HOLD_THRESHOLD    = 0.015;
const int   CONFIRM_COUNT     = 3;
const int OVERIDE_BUTTON_THRESH = 1000;

float filtered = 0.0;
int hold_counter = 0;
bool isHolding = false;
bool overide_button = false;

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
  Serial.print(isHolding ? "HOLD" : "FREE");

  //Print overide_button
  Serial.print(" | OVERIDE: ");
  Serial.println(overide_button);

  float absF = abs(filtered);  // <-- apply absolute value for thresholds

  //Check overide_button
  int overide_button_input = analogRead(OVERIDE_BUTTON_PIN);
  if ((overide_button_input > OVERIDE_BUTTON_THRESH) && (!overide_button)){
    overide_button = true;
  }

  if(overide_button){
    digitalWrite(LED_PIN, LOW); //Turn off buzzer
  }

  //Check holding-release state
  if (!isHolding) {
    if (absF > HOLD_THRESHOLD) {   // use absF
      hold_counter++;
      if (hold_counter >= CONFIRM_COUNT) {
        isHolding = true;
        hold_counter = 0;
        digitalWrite(LED_PIN, LOW);   // HOLD = LED OFF
        Serial.println(">>> HOLD DETECTED");
        //If hold then set overide_button to false again
        overide_button = false;
      }
    } else {
      hold_counter = 0;
    }
  } else {
    if ((absF < NO_HOLD_THRESHOLD) && (overide_button == false)) {  // use absF
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
