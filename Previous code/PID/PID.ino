//includes all the libraries
#include <Servo.h>.    //servo  
#include "MS5837.h".   //depth sensor 
#include <PID_v1.h>.   //PID controller 
#include <Wire.h>.     //I2C communication 

// Constants
const double DEPTH_SETPOINT_DEFAULT = 0.5;    //this is the manually set depth that the PID will use to compare and readjust  
const int SERVO_CENTER = 1500;    //this is the initial servo position...standard position is 1500....so it is 1500               
const int LOOP_DELAY = 250;    //I kinda tweaked the delay...this is a delay loop in microseconds between iterations of the loop function ensuring consistent timing
                 
const int SERIAL_BAUD_RATE = 9600;    // 9600 Baud ( the most common Baud rate )          
const int MOTOR_STABILIZATION_TIME = 7000;    //time in ms waited after motor initialization for it to stabalize 

// PID controller parameters
const double dkp = 500, dki = 5, dkd = 1;
double depthInput, depthOutput, depthSetpoint = DEPTH_SETPOINT_DEFAULT;

// Initialize PID controller
PID depthPID(&depthInput, &depthOutput, &depthSetpoint, dkp, dki, dkd, DIRECT);

// Servo objects for controlling motors
Servo topFront, topBack;

// Depth sensor object
MS5837 depthSensor;

void setup() {
    Wire.begin();
    Serial.begin(SERIAL_BAUD_RATE);
    
    depthPID.SetOutputLimits(-200, 200);
    depthPID.SetMode(AUTOMATIC);
    
    if (!initializeDepthSensor()) {
        Serial.println("Depth sensor initialization failed. System has been stopped");
        while (1) {}  // stopping if the sensor cant  initialize
    }
    
    initializeMotors();
    Serial.println("System ready");
}

void loop() {
    static unsigned long lastMicros = 0;
    unsigned long currentMicros = micros();

    if (Serial.available()) {
        processSerialInput();
    } else {
        updateDepthControl();
        printDebugInfo();
        writeToServos();
        
        // Maintain consistent loop timing
        while (micros() - currentMicros < LOOP_DELAY) {
            delayMicroseconds(1);
        }
        lastMicros = currentMicros;
    }
}

bool initializeDepthSensor() {
    depthSensor.setModel(MS5837::MS5837_02BA);
    if (!depthSensor.init()) {
        return false;
    }
    depthSensor.setFluidDensity(997);  // freshwater
    return true;
}

void initializeMotors() {
    topFront.attach(5);
    topBack.attach(2);
    topFront.writeMicroseconds(SERVO_CENTER);
    topBack.writeMicroseconds(SERVO_CENTER);
    delay(MOTOR_STABILIZATION_TIME);
}

void processSerialInput() {
    float incomingValue = Serial.parseFloat();
    if (incomingValue != 0) {
        depthSetpoint = incomingValue;
        Serial.print("New depth setpoint: ");
        Serial.println(depthSetpoint);
    }
}

void updateDepthControl() {
    depthSensor.read();
    depthInput = depthSensor.depth();
    depthPID.Compute();
}

void printDebugInfo() {
    Serial.print("DepthInput:");
    Serial.print(depthInput);
    Serial.print(",Setpoint:");
    Serial.println(depthSetpoint);
}

void writeToServos() {
    int servoValue = static_cast<int>(depthOutput) + SERVO_CENTER;
    topBack.writeMicroseconds(servoValue);
    topFront.writeMicroseconds(servoValue);
}