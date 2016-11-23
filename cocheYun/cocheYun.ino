/*
 * Recibe un dato por socket y gira la direccion
 */

#include <Bridge.h>
#include <Console.h>
#include <Wire.h>
#include <YunServer.h>
#include <YunClient.h>

#include <Servo.h>

//SERVO DIRECCION
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
            if (command !=0)
            {
              Serial.println(command); 
              gira(command);
            }
          }
       }
    }
    
    // Close connection and free resources.
    client.stop();
  }
  else {
    Serial.println("no client connected, retrying");
    delay(1000);
  }
}
void gira(int aonde)
{
  if (aonde > 320) myservo.write(MIN);
  else myservo.write(MAX);   
}


