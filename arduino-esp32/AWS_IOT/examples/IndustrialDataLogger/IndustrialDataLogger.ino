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

#include <AWS_IOT.h>
#include <WiFi.h>
#include <max6675.h>
#include <EmonLib.h>


/************************************************************************
                             AWS Configuration
*************************************************************************/
extern char WIFI_SSID[];
extern char WIFI_PASSWORD[];
extern char HOST_ADDRESS[];
extern char CLIENT_ID[];
extern char TOPIC_NAME[];
/***********************************************************************/


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
AWS_IOT AWS_CLIENT;
MAX6675 thermoCouple(thermoCLK, thermoCS, thermoDO);
EnergyMonitor emon;                  
/*********************************************************/

int status = WL_IDLE_STATUS;
int tick=0, publishMsg=0;
char payload[512];
const char payloadFormat[] = "{\"state\":{\"desired\":{\"temperature\":%f,\"irms\":%f,\"power\":%f}}}";

void setup() {

    Serial.begin(115200);
    delay(2000);

    while (status != WL_CONNECTED)
    {        
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        // wait 5 seconds for connection:
        delay(5000);
    }

    Serial.println("Connected to wifi");

    if(AWS_CLIENT.connect(HOST_ADDRESS,CLIENT_ID)== 0)
    {
        Serial.println("Connected to AWS");
        delay(1000);
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }

    delay(2000);

    emon.current(currentPin, 111.1);             // Current: input pin, calibration.
}



void loop() {

    double Irms,power,tempCelcius;

    if(tick >= 5)  // Publish every 5secs
    {
        tempCelcius = thermoCouple.readCelsius();
        Irms = emon.calcIrms(1480);  // Calculate Irms only
        power = 230 * Irms;
        sprintf(payload,payloadFormat,tempCelcius,Irms,power); 
        if(AWS_CLIENT.publish(TOPIC_NAME,payload) == 0)
        {
            Serial.println(payload);
            tick = 0; // Publish successfull, wait for 5more seconds
        }
        else
        {
            Serial.println("Publish failed, Will try again after 1sec");
        }
    }
    else
    {
        tick++;
    }

    vTaskDelay(1000 / portTICK_RATE_MS);
}
