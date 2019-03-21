
/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WEMOS_Motor.h>

#define STASSID "ESP8266_Network"
#define STAPSK  "password"

const char* ssid = STASSID;
const char* password = STAPSK;


Motor leftMotor(0x30, _MOTOR_A, 100);
Motor rightMotor(0x30, _MOTOR_B, 100);

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  leftMotor.setmotor(_STOP);
  rightMotor.setmotor(_STOP);
  leftMotor.setmotor(_STANDBY);
  rightMotor.setmotor(_STANDBY);
  delay(1000);
  Serial.begin(115200);
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  IPAddress myIP = IPAddress(192,168,1,1);
  IPAddress gatewayIP = IPAddress(192,168,1,1);
  IPAddress subnetIP = IPAddress(255,255,255,0);
  WiFi.config(myIP, gatewayIP, subnetIP);
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("HTTP server started");

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int val;
  if (req.indexOf(F("/forwardLeft")) != -1) {
    val = 0;
  } else if (req.indexOf(F("/forward")) != -1) {
    val = 1;
  } else if (req.indexOf(F("/forwardRight")) != -1) {
    val = 2;
  } else if(req.indexOf(F("/stop")) != -1){
    val = 3;
  } else if (req.indexOf(F("/backLeft")) != -1) {
    val = 4;
  }
  else if (req.indexOf(F("/backRight")) != -1) {
    val = 5;
  }
  else if (req.indexOf(F("/back")) != -1) {
    val = 6;
  }
  else {
    Serial.println(F("invalid request"));
    val = -1;
  }

  switch(val){
    case 0:{
      leftMotor.setmotor(_CCW, 15);
      rightMotor.setmotor(_CW, 25);
      Serial.println("Going forward left!");
      break;
    }  
    case 1:{
      leftMotor.setmotor(_CCW, 20);
      rightMotor.setmotor(_CW, 20);
      Serial.println("Going forward!");
      break;
    }  
    case 2:{
      leftMotor.setmotor(_CCW, 25);
      rightMotor.setmotor(_CW, 15);
      Serial.println("Going forward right!");
      break;
    }  
    case 3:{
      leftMotor.setmotor(_STOP);
      rightMotor.setmotor(_STOP);
      Serial.println("Stopping!");
      break;
    }  
    case 4: {
      leftMotor.setmotor(_CW, 15);
      rightMotor.setmotor(_CCW, 25);
      Serial.println("Going back left!");
      break;
    }
    case 5: {
      leftMotor.setmotor(_CW, 25);
      rightMotor.setmotor(_CCW, 15);
      Serial.println("Going back right!");
      break;
    }
    case 6: {
      leftMotor.setmotor(_CW, 20);
      rightMotor.setmotor(_CCW, 20);
      Serial.println("Going back!");
      break;
    }
    default:{
      Serial.println("Not a correct request, doing nothing...");     
    }
  }

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
    client.print(F("HTTP/1.1 200 OK\r\n"));

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}
