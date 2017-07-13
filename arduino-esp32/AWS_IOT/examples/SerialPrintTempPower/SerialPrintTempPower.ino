/*
 * Tutorial : Hornbill Industrial Data logger (Temp and Current )
 * Sensor: Max6675 Temp Sensor, non-invasive Current Sensor sensor 
  
 * Reference links:
   https://exploreembedded.com/wiki/Hornbill_Industrial_Data_Logger
   https://exploreembedded.com/wiki/Secure_IOT_with_AWS_and_Hornbill_ESP32
   https://exploreembedded.com/wiki/AWS_IOT_with_Arduino_ESP32

 * Library Links:  
   https://github.com/ExploreEmbedded/Hornbill-Examples/tree/master/arduino-esp32/AWS_IOT/examples
   https://github.com/openenergymonitor/EmonLib
   https://github.com/adafruit/MAX6675-library
  */               
#include <max6675.h>
#include <EmonLib.h>


/**********************************************************
                    Pin Mapping
**********************************************************/
int thermoDO = 27;
int thermoCS = 14;
int thermoCLK = 12;
int currentPin = 36;
/*********************************************************/


/*********************************************************
                 Create instances 
*********************************************************/
MAX6675 thermoCouple(thermoCLK, thermoCS, thermoDO);
EnergyMonitor emon;                  
/*********************************************************/
char buff[100];

void setup() {

    Serial.begin(115200);
    emon.current(currentPin, 111.1);             // Current: input pin, calibration.
}


void loop() {

    double Irms,power,tempCelcius;

        tempCelcius = thermoCouple.readCelsius();
        Irms = emon.calcIrms(1480);  // Calculate Irms only
        power = 230 * Irms;
        sprintf(buff,"tempCelcius:%f Irms:%f power:%f",tempCelcius,Irms,power); 
        Serial.println(buff);

    vTaskDelay(1000 / portTICK_RATE_MS);
}
