/*
 * Recibe un dato por socket, avanza y gira la direccion en ese sentido. si no hay dato para. 
 */

#include <Bridge.h>
#include <Console.h>
#include <Wire.h>
#include <YunServer.h>
#include <YunClient.h>

#include <Servo.h>

////DRIVER MOTOR/////////////////
int IN3 = 12;    // Input3 conectada al pin 12
int IN4 = 13;    // Input4 conectada al pin 13 
int ENB = 11;    // ENB conectada al pin 11 de Arduino para PWM

//SERVO DIRECCION////////////////////////
#define RANGO 21
#define CENTRO 75
int MIN = CENTRO-RANGO;
int MAX = CENTRO+RANGO+0;
int tiempo=12;
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards



// Socket port 9999 to communicate.
YunServer server(9999);


/**
* Entry point of the program.
* Initialize everything. Called when the Arduino is powered.
*/
void setup() {
  // Bridge startup
  Bridge.begin();

  // Listen the entire network (socket communication)
  server.noListenOnLocalhost();
  server.begin();

  Serial.begin(9600);
  Serial.setTimeout(10000);
  myservo.attach(3);  // attaches the servo on pin 9 to the servo object

  //MOTOR////
  pinMode (ENB, OUTPUT); 
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
}

/**
* Has to act as an -infinite- loop.
* Contains the program logic.
* Wait for a client then process it when he's found.
*/
void loop() {
  // Get clients coming from server
  YunClient client = server.accept();

  // There is a new client  
  if (client) {
    // Change the predifined timeout from 2000 to 5. Avoid impressive timeout.
    client.setTimeout(5);
    Serial.println("Client connected!");

    // When we get a client, go in the loop and exit only when the client disconnect. This will happens when the android application is killed (the socket must be closed by the app). This will automatically happens from the website for each http request.
    while(client.connected())
    {
       if (client.available())
       {
          String commandS = client.readStringUntil(' ');// Get the first element of the command.
          if(commandS.length() > 0){
            int command = client.parseInt();// Get the first int
            //Serial.println(commandS); 
            if (command !=0 && command != 9999)
            {
              Serial.println(command); 
              adelante(100);
              gira(command);
            }
            else freno();
          }
       }
       else freno();
    }
    
    // Close connection and free resources.
    client.stop();
    freno();
  }
  else {
    Serial.println("no client connected, retrying");
    freno();
    delay(1000);
  }
}
void gira(int aonde)
{
  if (aonde > 320) myservo.write(MIN);
  else myservo.write(MAX);   
}

void adelante(int velocidad)
{
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, HIGH);
  analogWrite(ENB,velocidad);
}
void freno()
{
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, LOW);
}


