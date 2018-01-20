# I2CSoilMoistureSensor

Simple Particle Library for the I2C Soil Moisture Sensor version from Chirp
(https://github.com/Miceuz/i2c-moisture-sensor).  This readme and code is a conversion of the great work done by
Apollon77 who's original code can be found at https://github.com/Apollon77/I2CSoilMoistureSensor.  Most of the method descriptions left as is from his branch.

## Where can I buy this sensor?

More informations at: https://www.tindie.com/products/miceuz/i2c-soil-moisture-sensor/.
If you have an older version of the ruggedized sensor and the wire colors don't match Miceuz's site, try this

Blue = gnd  
Red = vcc   
Yellow = scl  
White = sda  

## Version History

### v1.0.2
- Fixed bug in getAddress. Was using 16bit read, should have been 8bit
### v1.0.1
- Updated properties, published to PCL
### v1.0.0
- Initial Release, published to the Particle Community Library (PCL)

## Notes/Hints
None

## Methods

### Constructor I2CSoilMoistureSensor
Optionally set sensor I2C address if different from default (0x20).  Per Miceuz's site, if you hot plug the sensor, your address may shift unexpectedly.  This happened to me (0x20 to 0x45).  You'll need to use a simple I2C Scanner to find your addresses and then issue a setAddress command to reset.


### begin(bool wait)
Initializes anything ... it does a reset.
When used without parameter or parameter value is false then a
waiting time of at least 1 second is expected to give the sensor
some time to boot up.
Alternatively use true as parameter and the method waits for a
second and returns after that.

### getCapacitance()
Return measured Soil Moisture Capacitance Moisture is somewhat linear. More moisture will
give you higher reading. Normally all sensors give about 200 - 300 as value in free air at
5V supply.

### setAddress(int addr, bool reset)
Change I2C address of the sensor to the provided address (1..127) and do a reset after it
in order for the new address to become effective if second parameter is true. Method
returns true if the new address is set successfully on sensor.

### getAddress()
Return current Address of the Sensor

### changeSensor(int addr, bool wait)
Changes the address (1..127) of the sensor, this instance is trying to read from
and do a reset after to initialize.
The second parameter is optional and tells the method to wait for a second to allow
the sensor to boot up.

### startMeasureLight()
Starts the measurement for the Light sensor. Wait at least 3 seconds till you call method
getLight to get the Light value.                *

### getLight(bool wait)
Read the Light Measurement from the sensor. When used without parameter or parameter value
is false then a former call to method startMeasureLight and a waiting time of at least 3
seconds is expected.

Alternatively use true as parameter and the method does the call to startMeasureLight and
a 3 seconds delay automatically and no former call is needed.

The measurement gives 65535 in a dark room away form desk lamp - so more light, lower
reading. When it's dark, it takes longer to measure light, reading the light register
while measurement is in progress (e.g. wait time too short) will return the previous
reading. Be aware, light sensor is pretty noisy.

### getTemperature()
Read the Temperature Measurement. Temperature is measured by the thermistor on the tip of
the sensor. Calculated absolute measurement accuracy is better than 2%. The returned value
is in degrees Celsius with factor 10, so need to divide by 10 to get real value

### sleep()
Powers down the sensor. Use this function in order to save power inbetween measurements.
You need to have FW 2.3 from the Sensor to use this method.

### isBusy()
Checks if sensor is busy. Returns true if a measurement is running.
You need to have FW 2.3 from the Sensor to use this method.

### resetSensor()
Resets sensor. Give the sensor 0.5-1 second time to boot up after reset.

### getVersion()
Get Firmware Version. 0x22 means 2.2

## Examples
You can find examples in the examples folder of this library

## LICENSE
Licensed under the MIT license
