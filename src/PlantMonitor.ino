/*
 * Project PlantMonitor
 * Description:
 * Author:
 * Date:
 */
#include <I2CSoilMoistureSensor.h>
I2CSoilMoistureSensor soilSensor;
bool published=false;
int numOfReads=0;
int readsTotal=0;
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

const size_t bufferSize = JSON_OBJECT_SIZE(5) + 130;
DynamicJsonBuffer jsonBuffer(bufferSize);

HttpClient http;
http_header_t headers[] = {
      { "Content-Type", "application/json" },
      { "Accept" , "application/json;odata=nometadata" },
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};
http_request_t request;
http_response_t response;


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//int moistureLow=294;
//int moistureHigh=446;

int moistureLow=1;
int moistureHigh=100;

int updateRange(String command);
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Particle.function("updateRange", updateRange);
  Wire.setSpeed(48000);
  if (!Wire.isEnabled()) {
      Wire.begin();
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  Particle.variable("MoistureLow", moistureLow);
  Particle.variable("MoistureHigh", moistureHigh);
  getRange();


  // register the cloud function


  int i = 20;

  soilSensor.begin();
  delay(3000); // give some time to boot up
  String msg = "I2C Soil Sensor: ";
  String address = String(soilSensor.getAddress(), HEX);
  String version = String(soilSensor.getVersion());

 // publish sensors info
 Particle.publish("PlantMonitor", msg + " Soil Sensor: " + i + " - Addr: " + address + " - FW: " + version, 60, PRIVATE);


 delay(1000);


}

int updateRange(String command)
{
  Particle.publish("Mr.Green", "Fn Called: " + command, 60, PRIVATE);
bool error=false;
String errorMsg="";
  if (command.length()==7)
  {
    int delim = command.indexOf(",");
    String low = command.substring(0, delim);
      String high = command.substring(delim+1);

      if (low.toInt()<=0 or high.toInt()<=0 or low.toInt()>999 or high.toInt()>=999)
      {
        error=true;
        errorMsg="Invalid number specified for low or high";
      }
      else
      {
      moistureLow=low.toInt();
      moistureHigh=high.toInt();

      request.body = "{\"MoistureLow\":\""+low+"\",\"MoistureHigh\":\""+high+"\"}";
      Particle.publish("WebCall-Request", request.body, 60, PRIVATE);
      request.hostname = "varstore.table.core.windows.net";
      request.port = 80;
      request.path = "/LivingRoom(PartitionKey='MrGreen',RowKey='variables')?sv=2017-04-17&ss=t&srt=co&sp=rwlau&se=2019-01-19T21:20:07Z&st=2018-01-19T13:20:07Z&spr=https,http&sig=ZnuWcZy12r8gErGXyM%2BUU8thqU23HTA5PHrWMium%2BfI%3D";
      http.put(request, response, headers);
      delay(3000);
      Particle.publish("WebCall-Response", String(response.status), 60, PRIVATE);
      Particle.publish("WebCall-Body", response.body, 60, PRIVATE);
      return 0;
    }

  }
  else
  {
    error=true;
    errorMsg="Invalid length of input, should be nnn,nnn format";
  }

  if (error){
    Particle.publish("Mr.Green", "Fn Error: "+errorMsg, 60, PRIVATE);
    return -1;
  }
}
int getRange()
{

request.hostname = "varstore.table.core.windows.net";
request.port = 80;
request.path = "/LivingRoom(PartitionKey='MrGreen',RowKey='variables')?sv=2017-04-17&ss=t&srt=co&sp=rwlau&se=2019-01-19T21:20:07Z&st=2018-01-19T13:20:07Z&spr=https,http&sig=ZnuWcZy12r8gErGXyM%2BUU8thqU23HTA5PHrWMium%2BfI%3D";

// The library also supports sending a body with your request:
//request.body = "{\"key\":\"value\"}";
http.get(request, response, headers);
delay(3000);
String jsonResult = response.body;
jsonResult.replace("83\r\n","");
jsonResult.replace("\r\n0\r\n\r\n","");
Particle.publish("WebCall-Response", String(response.status), 60, PRIVATE);
Particle.publish("WebCall-Body", jsonResult, 60, PRIVATE);

JsonObject& root = jsonBuffer.parseObject(jsonResult);
if (!root.success()) {
  Particle.publish("Mr.Green", "Error: Could not parse config variables", 60, PRIVATE);
  moistureLow=295;
  moistureHigh=447;
  Particle.publish("Mr.Green-Config", "Moisture Low/High:  DEFAULT! "+String(moistureLow) + " / " + String(moistureHigh), 60, PRIVATE);
}
else
{
  moistureLow=root["MoistureLow"];
  moistureHigh=root["MoistureHigh"];
  Particle.publish("Mr.Green-Config", "Moisture Low/High:  "+String(moistureLow) + " / " + String(moistureHigh), 60, PRIVATE);
}
 display.clearDisplay();

DisplayTitle("Moisture Low/High");
DisplayInfo(String(moistureLow) + " / " + String(moistureHigh));

delay(5000);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
    int soilMoistureRaw = soilSensor.getCapacitance();
while (soilSensor.isBusy()) {
  //Do nothing, waiting
}
    soilMoistureRaw = soilSensor.getCapacitance();
    int moisture = map(soilMoistureRaw,moistureLow,moistureHigh,0,100);
    int tempInC = soilSensor.getTemperature();
    float tempInF=(tempInC/(float)10)*9/5+32;
    int light = soilSensor.getLight(true);
    numOfReads++;
    readsTotal+=moisture;
    int avg = readsTotal/numOfReads;

    int t = round(tempInF);

  //delay(3600000);
  int min = Time.minute();

    if(min%5==0)
    {
      if (!published)
      {
        PublishSoilMeasurements(soilMoistureRaw,avg,tempInF,light);
        published=true;
        numOfReads=0;
        readsTotal=0;
      }
    }
    else
    {
      published=false;
    }
   display.clearDisplay();
   DisplayTitle("Moisture Avg - Raw");
   DisplayInfo(String(avg) + "% (" + String(moisture) + "%)");
  delay(5000);
}
void PublishSoilMeasurements(int raw, int moisture, float temp, int light){

      String jsonSensorData = "{\"SoilMoistureRaw\":{{SoilMoistureRaw}},\"SoilMoisture\":{{SoilMoisture}},\"SoilTemperature\":{{SoilTemperature}},\"Light\":{{Light}}}";
      jsonSensorData.replace("{{SoilMoistureRaw}}",String(raw));
      jsonSensorData.replace("{{SoilMoisture}}",String(moisture));
      jsonSensorData.replace("{{SoilTemperature}}",String(temp));
      jsonSensorData.replace("{{Light}}",String(light));

      Particle.publish("Mr.Green", jsonSensorData, 60, PRIVATE);
}

void DisplayTitle(String title){
  int len = title.length();
  int start = ((21-len) / 2)*6;
  display.setTextSize(1);
display.setTextColor(WHITE);
  display.setCursor(start,0);
  display.print(title);
  display.display();
}

void DisplayInfo(String info){
  int len = info.length();
  int start = ((11-len) / 2)*14;
  display.setTextSize(2);
display.setTextColor(WHITE);
  display.setCursor(start,18);
  display.print(info);
  display.display();
}
