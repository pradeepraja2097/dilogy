#include <HX711_ADC.h>
#include <Wire.h>
HX711_ADC LoadCell(9, 8);
void setup() {
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(2000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(44.0); // calibration factor for load cell => strongly dependent on your individual setup
  Serial.begin(9600); // begins connection to the LCD module
}
void loop() {
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData(); // get output value
  Serial.println("Weight[kg]:"); // print out to LCD
  Serial.print(i/-1000); // print out the retrieved value to the second row
  delay(1000);
}
