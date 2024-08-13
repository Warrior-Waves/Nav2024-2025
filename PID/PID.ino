#include <Servo.h>           // Library for controlling servo motors
#include "MS5837.h"          // Library for the MS5837 depth sensor
#include <PID_v1.h>          // Library for the PID controller
#include <Wire.h>            // Library for I2C communication

// PID controller parameters
double dKp = 500, dKi = 5, dKd = 1;
double depthInput, depthOutput;   // Variables for PID controller
double depthSetpoint = 0.5;        // Desired depth setpoint (meters)

// Initialize PID controller
PID depthPID(&depthInput, &depthOutput, &depthSetpoint, dKp, dKi, dKd, DIRECT);

// Servo objects for controlling motors
Servo top_front;
Servo top_back;

// Depth sensor object
MS5837 depthSensor;

void setup()
{
  Wire.begin();                    // Start I2C communication
  Serial.begin(9600);              // Initialize serial communication for debugging
  depthPID.SetOutputLimits(-200, 200);  // Set the PID output limits
  depthPID.SetMode(AUTOMATIC);     // Set PID controller to automatic mode
  depthSensorSetup();              // Initialize depth sensor
  motorSetup();                    // Initialize servos
  Serial.println("ready");         // Indicate that setup is complete
}

long microseconds;  // Variable to track time in microseconds

void loop()
{
  if (Serial.available())
  {
    float incomingValue = Serial.parseFloat();  // Read incoming float value from serial
    if (incomingValue != 0)
    {
      depthSetpoint = incomingValue;  // Update depth setpoint if a valid value is received
    }
  }
  else
  {
    long prevMicroseconds = microseconds;  // Store previous time
    microseconds = micros();  // Update current time
    depthSensor.read();       // Read depth from sensor
    depthInput = depthSensor.depth();  // Get depth input for PID
    depthPID.Compute();       // Compute PID output
    Serial.print("DepthInput:");  // Print current depth
    Serial.print(depthInput);
    Serial.print(",");
    Serial.print("Setpoint:");    // Print current setpoint
    Serial.println(depthSetpoint);

    // Write PID output to servos
    top_back.writeMicroseconds(depthOutput + 1500);
    top_front.writeMicroseconds(depthOutput + 1500);

    // Maintain a 250 microsecond loop delay
    while (micros() - microseconds < 250)
      delayMicroseconds(1);
  }
}

void depthSensorSetup() 
{
    depthSensor.setModel(MS5837::MS5837_02BA);  // Set the sensor model
    if (!depthSensor.init()) {
        Serial.println("Failed to initialize depth sensor!");  // Error message if initialization fails
    }
    depthSensor.setFluidDensity(997);  // Set fluid density for the sensor (freshwater)
}

void motorSetup()
{
  top_front.attach(5);   // Attach top front servo to pin 5
  top_back.attach(2);    // Attach top back servo to pin 2
  top_front.writeMicroseconds(1500);  // Set initial position of top front servo
  top_back.writeMicroseconds(1500);   // Set initial position of top back servo
  delay(7000);  // Wait 7 seconds to stabilize servos
}
