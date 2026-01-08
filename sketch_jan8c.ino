#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const int BUZZER_PIN = 15;
const float DROP_THRESHOLD = 2.5; // adjust sensitivity (g-force)

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(BUZZER_PIN, OUTPUT);

  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  Serial.println("MPU6050 connected");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert to g (MPU6050 scale factor)
  float x = ax / 16384.0;
  float y = ay / 16384.0;
  float z = az / 16384.0;

  // Total acceleration magnitude
  float totalG = sqrt(x*x + y*y + z*z);

  Serial.println(totalG);

  // Detect impact
  if (totalG > DROP_THRESHOLD) {
    tone(BUZZER_PIN, 1000);  // 1kHz alarm
    delay(500);
    noTone(BUZZER_PIN);
    delay(1000); // prevent repeated triggering
  }

  delay(50);
}

