# Arduino-esp32-aws-iot
Arduini ESP32 library for AWS IoT

Please make sure you have read the [getting started guide](https://aws.amazon.com/iot/getting-started/) before trying to do anyting with this template.

This template is based on and contains the code from the [aws-iot-device-sdk-embedded-C](https://github.com/aws/aws-iot-device-sdk-embedded-C) project with an ESP32 port. It may help to review that project before attempting to use this template.

Define the below parameters in the sketch:
```
WIFI_SSID
WIFI_PASSWORD
HOST_ADDRESS
CLIENT_ID
TOPIC_NAME
```


*Certificates*
```
  root-CA.crt
  certificate.pem.crt
  private.pem.key
```
Above certificates needs to be stored in the file aws_iot_certificates.c as arrays. Check the file for more information.
 
![](https://exploreembedded.com/wiki/images/b/b9/ESP32_AWS_IOT_Certificates.png)


Please check this tutorial for setting up [AWS IOT on Amazon] (https://www.exploreembedded.com/wiki/Secure_IOT_with_AWS_and_Hornbill_ESP32)

Follow this tutorial for using the [Arduio Esp32 AWS libaray] (https://exploreembedded.com/wiki/AWS_IOT_with_Arduino_ESP32)



Kindly check our latest [AWS IoT](https://github.com/BuildStormTechnologies/arduino-esp32-aws-iot) libraries, with additional features such as:
1. Device provisioning.
2. Shadow updates.
3. Jobs management.
4. Over-the-Air (OTA) updates.
5. Mobile app for aws

   
Additionally, you can explore [libraries](https://github.com/BuildStormTechnologies/) tailored for various platforms such as Azure, Thingsboard, and Kaa IoT, supporting both WiFi and cellular-based applications.


