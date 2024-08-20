#include <Adafruit_BNO055.h>
#include <Wire.h>
#include <utility/imumaths.h>

// Constants
const uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;
const uint8_t BNO055_ID = 55;
const uint8_t BNO055_ADDRESS = 0x28;  // Default I2C address

// Create the BNO055 object
Adafruit_BNO055 bno = Adafruit_BNO055(BNO055_ID, BNO055_ADDRESS);

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);  // Wait for serial port to open

  Serial.println("BNO055 Orientation Sensor Test");
  
  // Initialize the BNO055
  if (!bno.begin()) {
    Serial.println("Failed to initialize BNO055! Check your wiring or I2C address.");
    while (1);
  }
  
  delay(1000);
  
  bno.setExtCrystalUse(true);
  
  Serial.println("BNO055 initialized successfully!");
  printSensorDetails();
}

void loop() {
  displaySensorStatus();
  displayOrientation();
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void displaySensorStatus() {
  uint8_t system_status, self_test_results, system_error;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);
  
  Serial.println();
  Serial.print("System Status: 0x");
  Serial.println(system_status, HEX);
  Serial.print("Self Test:     0x");
  Serial.println(self_test_results, HEX);
  Serial.print("System Error:  0x");
  Serial.println(system_error, HEX);
  Serial.println();
}

void displayOrientation() {
  sensors_event_t event;
  bno.getEvent(&event);
  
  Serial.print("Orientation: X = ");
  Serial.print(event.orientation.x, 4);
  Serial.print(", Y = ");
  Serial.print(event.orientation.y, 4);
  Serial.print(", Z = ");
  Serial.println(event.orientation.z, 4);
}

void printSensorDetails() {
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       "); Serial.println(sensor.name);
  Serial.print("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}