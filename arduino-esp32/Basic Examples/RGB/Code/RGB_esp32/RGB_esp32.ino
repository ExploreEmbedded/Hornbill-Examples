/*
  Blink
  Turns on an LED's on for one second, then off for one second, repeatedly.

*/

#define LED_Red 25
#define LED_Green 26
#define LED_Blue 27


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED's are an output.

  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Blue, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_Red, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(500);
  digitalWrite(LED_Red, HIGH);    // turn the LED off by making the voltage LOW
                     

  digitalWrite(LED_Green, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(500);
  digitalWrite(LED_Green, HIGH);    // turn the LED off by making the voltage LOW

  
 

  digitalWrite(LED_Blue, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(500);
  digitalWrite(LED_Blue, HIGH);    // turn the LED off by making the voltage LOW

 
}
