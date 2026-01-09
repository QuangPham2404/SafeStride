#define DO_PIN 13  

const int ledPin =  6;    

void setup() {
  Serial.begin(9600);
  pinMode(DO_PIN, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int lightState = digitalRead(DO_PIN);

  if (lightState == HIGH) {
    digitalWrite(ledPin, HIGH); }
  else {
    digitalWrite(ledPin, LOW);}

}
