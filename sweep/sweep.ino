/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

#define CAR_DIR_FW 0
#define CAR_DIR_BK 1

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(85);
  delay(3000);
}

void loop() {
  Serial.println("right");
  myservo.write(85 - 40);
  delay(3000);

  Serial.println("center");
  myservo.write(85 - 0);
  delay(3000);

  Serial.println("left");
  myservo.write(85 + 40);
  delay(3000);

}
