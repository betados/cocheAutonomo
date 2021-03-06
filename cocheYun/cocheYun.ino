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
int IN4 = 10;    // Input4 conectada al pin 10
int ENB = 11;    // ENB conectada al pin 11 de Arduino para PWM

//SERVO DIRECCION////////////////////////
#define RANGO 19
#define CENTRO 75
int MIN = CENTRO-RANGO;
int MAX = CENTRO+RANGO+0;
int tiempo=12;
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards



// Socket port 9999 to communicate.
YunServer server(9999);


int ancho=320;
int kp=35,kd=50;
float ki=0.1f;



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
  bool atras=false;
  bool *pAtras=&atras;
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
          //kd = Serial.parseInt();
          //Serial.print("Konstante derivativa: ");
         //Serial.println(kd);
          String commandS = client.readStringUntil(' ');// Get the first element of the command.
          int command = client.parseInt();// Get the first int
          if (command==9999 || command == 0) freno();
          else{
            if (commandS=="equis"){
              //Serial.print(commandS);
              //Serial.println(command); 
              gira(command,atras);
            }
            if (commandS=="tamano"){
              //Serial.print(commandS);
              //Serial.println(command);
              mueve(command,pAtras); 
            }
            if (commandS=="ancho"){
              ancho = command;
              //Serial.println(ancho);
            }
          }
       }
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
void gira(int aonde, bool atras)
{
  //Serial.print("Marcha Atras= ");
  //Serial.println(atras);
  
  if (atras == false){
    if (aonde > (ancho/2.0f)) myservo.write(MIN);
    else myservo.write(MAX);   
  }
  if (atras == true){
    if (aonde > (ancho/2.0f)) myservo.write(MAX);
    else myservo.write(MIN);   
  }
  
}

void adelante(int velocidad, bool *pAtras)
{
  if (velocidad >255 ) velocidad = 255;
  //if (velocidad < 100 ) velocidad = 100;
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, HIGH);
  analogWrite(ENB,velocidad);
  //analogWrite(ENB,0);
  *pAtras=false;
}
void atrasF(int velocidad,bool *pAtras)
{
  if (velocidad >255 ) velocidad = 255;
  //if (velocidad < 100 ) velocidad = 100;
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);
  analogWrite(ENB,velocidad);
  //analogWrite(ENB,0);
  *pAtras=true;
}
void freno()
{
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, HIGH);
  analogWrite(ENB,255);
}
void mueve(float tamano, bool *pAtras){
  //tamano = tamano / ancho;
  float distancia = ancho/tamano;
  //Serial.print("Distancia= ");
  //Serial.println(distancia);
  float distanciaConsigna=8/*ancho/40*/; //el denominador es el tamaño de la jeta //mas pequeña más lejano el cambio
  float error = distancia -distanciaConsigna;
  static float errorAnterior=0;
  static float errorAcumulado=errorAcumulado+error;
  
  
  float dE = error - errorAnterior; 

  if ( error<0  || error>0 ) {
      int velocidad = error * /*KP*/ kp  + dE * kd + errorAcumulado * ki;
      //Serial.print("Velocidad= ");
      //Serial.println(velocidad);
      if (velocidad>0) adelante(velocidad,pAtras);
      else{ 
        if (velocidad<0){
                velocidad = velocidad *-1;
                atrasF(velocidad,pAtras);  
        }
        else freno(); 
      }
    
  }
  else{
    freno(); 
    errorAcumulado=0;
  }
  
  errorAnterior = error;
  
  
}


