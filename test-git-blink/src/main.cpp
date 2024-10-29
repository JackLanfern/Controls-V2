#include <Arduino.h>

// put function declarations here:

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(23, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Hello ");
  digitalWrite(23, HIGH);
  delay(500);
  Serial.println("World");
  digitalWrite(23, LOW);
  delay(500);
  //I changed this GitHub, look at me go!
}
