#include <Bridge.h>
#include <Console.h>
#include <Wire.h>
#include <YunServer.h>
#include <YunClient.h>

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
            if (command !=0) Serial.println(command); 
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
