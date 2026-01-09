//MODIFIED from original code by Rui Santos, https://randomnerdtutorials.com 

 
//Declare pins
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;
//Declare buzzer pin
int buzzerPin = 10;

//Define constant
const int DISTANCE_THRESH = 5; // 5 cm
const int CLOSEST_DIST = 15; //cm
const int MEDIUM_DIST = 30; //cm
const int FURTHEST_DIST = 50; //cm
const int CLOSEST_FREQ = 60;
const int MEDIUM_FREQ = 120;
const int FURTHEST_FREQ = 250;


//Function to have the buzzer create a pulse pattern rather than a continuous sound
void buzzer_pulsate(int buzzerPin, unsigned long pulseInterval, bool enable)
{
  static unsigned long past_time = 0;
  static bool buzzerState = false;

  unsigned long current_time = millis();

  if (!enable) {
    buzzerState = false;
    past_time = current_time;
    digitalWrite(buzzerPin, LOW);
    return;
  }

  if (current_time - past_time >= pulseInterval) {
    past_time = current_time;
    buzzerState = !buzzerState;
    digitalWrite(buzzerPin, buzzerState);
  }
}

 
void setup() {
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  delay(500);
}
 
void loop() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  //Check distance and control buzzer with 3 stages
  if (cm <= CLOSEST_DIST) {
      buzzer_pulsate(buzzerPin, CLOSEST_FREQ, true);    // fastest beeping
  }
  else if (cm <= MEDIUM_DIST) {
      buzzer_pulsate(buzzerPin, MEDIUM_FREQ, true);   // medium beeping
  }
  else if (cm <= FURTHEST_DIST) {
      buzzer_pulsate(buzzerPin, FURTHEST_FREQ, true);   // slow beeping
  }
  else {
      buzzer_pulsate(buzzerPin, 250, false);  // reset & silence - 250 is an arbitrary placeholder
  }
  
  delay(50);
}