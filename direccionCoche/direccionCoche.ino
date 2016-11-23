/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#define RANGO 22

#define CENTRO 75



int MIN = CENTRO-RANGO;
int MAX = CENTRO+RANGO+5;
int tiempo=12;

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (pos = MIN; pos <= MAX; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(tiempo);                       // waits 15ms for the servo to reach the position
  }
    delay(500);

  for (pos = MAX; pos >= MIN; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(tiempo);                       // waits 15ms for the servo to reach the position
  }
      delay(500);

}

