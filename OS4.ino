#include <SPI.h>
#include <Ethernet.h>
#include <RCSwitch.h>
#include "DHT.h"

RCSwitch mySwitch = RCSwitch();
DHT dht;

//Port for the webserver to listen on (default = 80)
EthernetServer server(8088);

//MAC Address of the ethernet card. Any valid hexidecimal pairs are fine
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//You can use any IP you want, but you'll need to make sure it's set up in your router
IPAddress ip(192,168,1,202);

double temperature = 0;
double temper;

char c = 0;           // incoming data
char command[8] = "\0";  // command

void setup() {
 
  Serial.begin(9600);
  // initiate ethernet and web server
  Ethernet.begin(mac, ip);
  server.begin();
  // initiate RF transmitter on pin 7 and set pulse length in miliseconds
  mySwitch.enableTransmit(7);
  mySwitch.setPulseLength(188);
  // Initiate temperature sensor on pin 5
  dht.setup(5);
}


void loop() {
  EthernetClient client = server.available();
  boolean current_line_is_first = true;
  
  if (client) {
    Serial.println("new client");
    boolean current_line_is_blank = true;
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && current_line_is_blank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          //client.println("<META HTTP-EQUIV=REFRESH CONTENT=30 URL=>");
          
          client.println("<center><p><h1>Home Automation</h1></p><hr><br />");

          // Room Temperature
          temperature = dht.getTemperature();
          temper = temperature * 9 / 5 + 32;
          client.print("<p><h2>Temperature = ");
          
          // Make the Text red if it's hot, green if it's not
          if (temper > 74.99) {
            client.print("<font color=red>");
            //mySwitch.send(1070387, 24);  //Optionally turn on A/C automatically 
          } else {
            client.print("<font color=darkgreen>");
          }
          client.println(temper);
          client.println(" F</font></h2></p>");
          
          client.println("<form  method=get name=form>");
          client.println("<button name=b value=1 type=submit style=height:50px;width:100px>Light 1 ON</button>");
          client.println("<button name=b value=2 type=submit style=height:50px;width:100px>Light 1 OFF</button><br /><br />");
          client.println("<button name=b value=3 type=submit style=height:50px;width:100px>Light 2 ON</button>");
          client.println("<button name=b value=4 type=submit style=height:50px;width:100px>Light 2 OFF</button><br /><br />");
          client.println("<button name=b value=5 type=submit style=height:50px;width:100px>A/C ON</button>");
          client.println("<button name=b value=6 type=submit style=height:50px;width:100px>A/C OFF</button><br /><br />");
          client.println("<button name=b value=7 type=submit style=height:50px;width:100px>TV ON</button>");
          client.println("<button name=b value=8 type=submit style=height:50px;width:100px>TV OFF</button>");
          client.println("</form><br />");
          
          client.println("<p>The temperature will refresh anytime you press a button.<br />");
          client.println("(served by an Arduino Uno)</p></center>");
          
          break;
        }
        if (c == '\n') {
          current_line_is_first = false;
          current_line_is_blank = true;
        } 
        else if (c != '\r') {
          current_line_is_blank = false;
        }
        if (current_line_is_first && c == '=') {
          for (int i = 0; i < 1; i++) {
            c = client.read();
            command[i] = c;
          }
          // LED control
          if (!strcmp(command, "1")) {
            mySwitch.send(1072387, 24);
          }
          else if (!strcmp(command, "2")) {
            mySwitch.send(1072396, 24);
          }
          else if (!strcmp(command, "3")) {
            mySwitch.send(1078531, 24);
          }
          else if (!strcmp(command, "4")) {
            mySwitch.send(1078540, 24);
          }
          else if (!strcmp(command, "5")) {
            mySwitch.send(1070387, 24);
          }
          else if (!strcmp(command, "6")) {
            mySwitch.send(1070396, 24);
          }
          else if (!strcmp(command, "7")) {
            mySwitch.send(1070531, 24);
          }
          else if (!strcmp(command, "8")) {
            mySwitch.send(1070540, 24);
          }
        }
      }
    }
    // wait for the browser
    delay(1);
    client.stop();
    Serial.println("client disonnected");
  }
}
