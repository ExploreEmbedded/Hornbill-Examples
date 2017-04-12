/*

*/

// Import required libraries
#include <ESP_SSD1306.h>    // Modification of Adafruit_SSD1306 for ESP8266 compatibility
#include <Adafruit_GFX.h>   // Needs a little change in original Adafruit library (See README.txt file)
#include <SPI.h>            // For SPI comm (needed for not getting compile error)
#include <Wire.h>           // For I2C comm, but needed for not getting compile error

/*
HardWare OLED ESP32 I2C pins
GPIO21   SCL
GPIO22   SDA
*/

// Pin definitions
#define OLED_RESET  16  // Pin 15 -RESET digital signal
#define trigPin 18
#define echoPin 19

ESP_SSD1306 display(OLED_RESET); // FOR I2C


void setup(void)
{     
	// Start Serial
	Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // SSD1306 Init
  display.begin(SSD1306_SWITCHCAPVCC);  // Switch OLED 
  display.clearDisplay();


}

void loop() {   

   long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 4) {  
  
}
  else {
 
  }
  if (distance >= 200 || distance <= 0){
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(10,5);
   display.println("Distance");
   display.display();
   
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(30,40);
   display.println("Out of range");
   display.display();
  // Serial.println("Out of range");
  }
  else {
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(10,5);
   display.println("Distance");
   display.setCursor(30,40);
   display.println(distance);
   display.setCursor(70,40);
   display.println("cm");   
   display.display();
   // Serial.print(distance);
   // Serial.println(" cm");
  }
  delay(1000); 
}




