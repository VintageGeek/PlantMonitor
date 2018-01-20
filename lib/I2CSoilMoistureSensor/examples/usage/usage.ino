// Example usage for I2CSoilMoistureSensor library by Mike Morales.

#include "I2CSoilMoistureSensor.h"

// Initialize objects from the lib for default address of 0x20 
I2CSoilMoistureSensor i2CSoilMoistureSensorDefault;
// Use this two lines for non-standard address
#define soilSensorAddress 0x30
I2CSoilMoistureSensor i2CSoilMoistureSensor(soilSensorAddress);


void setup() {
    // Call functions on initialized library objects that require hardware
    i2CSoilMoistureSensor.begin();
}

void loop() {
    // Read moisture value
  int soilMoisture = i2CSoilMoistureSensorDefault.getCapacitance();
  int temperature = i2CSoilMoistureSensorDefault.getTemperature();
  int light = i2CSoilMoistureSensorDefault.getLight(true);//true initiates startMeasureLightand adds a 3 second pausebefore reading, false reads immediately
}
