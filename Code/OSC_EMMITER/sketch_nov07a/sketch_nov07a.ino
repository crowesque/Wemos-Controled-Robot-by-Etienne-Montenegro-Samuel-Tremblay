#include <tinyosc.h>

// INCLUDE ESP8266WiFi:
#include <ESP8266WiFi.h>

// Please change the following values with your network settings:
const char* ssid     = "Winternet is Coming";
const char* password = "electro2018";

IPAddress ip(10,0,3,53);//wemos
IPAddress gateway(10,0,3,1);
IPAddress subnet(255, 255, 255, 0);

// UDP
int udpReceivePort = 12501;
IPAddress udpTxIp = IPAddress(10,0,3,175);//computer
int udpTxPort = 7890;

// INCLUDE ESP8266 UDP
#include <WiFiUdp.h>
WiFiUDP udp;

// UDP BUFFERS
#define UDP_RX_BUFFER_MAX_SIZE 256
char udpRxBuffer[UDP_RX_BUFFER_MAX_SIZE];

#define UDP_TX_BUFFER_MAX_SIZE 256
char udpTxBuffer[UDP_TX_BUFFER_MAX_SIZE];

// OSC PARSER AND PACKER

#include <TinyOsc.h>
TinyOsc osc;

#include <Bounce2.h>
Bounce button;

//GLOBAL VARIABLE
float jL, jR, ltrt;
bool a,x,b;

//===========
//== SETUP ==
//===========
void setup() {

        // INITIATE SERIAL COMMUNICATION FOR DEBUGGING.
        Serial.begin(57600);
        Serial.println("***STARTING WIFI***");

        // BEGIN WIFI
        WiFi.config(ip , gateway , subnet );
        WiFi.begin(ssid, password);

        // WAIT UNTIL CONNECTED
        while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(10);
        }
        //

        // PRINT CONNECTION SETTINGS
        Serial.println();
        Serial.println("WiFi connected, IP address: ");
        Serial.println( WiFi.localIP() );

        udp.begin(udpReceivePort); // BEGIN LISTENING ON UDP PORT udpReceivePort

//        // SETUP THE BUILTIN LED
//        pinMode(LED_BUILTIN, OUTPUT);
//
//        // SETUP THE OTHER LED
//        pinMode(4, OUTPUT);
//
//        // SETUP THE BUTTON ON PIN 0
//        button.attach(0, INPUT_PULLUP);
//        button.interval(5);

}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {


        // GET THE TYPE TAGS AS A C STRING POINTER
        char * typeTags = osc.getTypeTags();

//        Serial.println("***OSC***");
//        Serial.print("Address: ");
//        Serial.println(osc.getAddress());
//        Serial.print("Type tags: ");
//        Serial.println(typeTags);

        // IF THE ADDRESS IS /led AND THERE IS AN FLOAT ('f')
        if ( osc.fullMatch("/JL") && typeTags[0] == 'f' ) {
                jL = osc.getNextFloat();
                Serial.print("jL: ");
                Serial.println(jL);
        } else if ( osc.fullMatch("/JR") && typeTags[0] == 'f' ) {
                jR = osc.getNextFloat();
                Serial.print("jR: ");
                Serial.println(jR);
        } else if ( osc.fullMatch("/LTRT") && typeTags[0] == 'f' ) {
                ltrt = osc.getNextFloat();
                Serial.print("ltrt: ");
                Serial.println(ltrt);
        } else  if ( osc.fullMatch("/A") && typeTags[0] == 'i' ) { //typetag is bool receive 1 or 0 as int
                a = osc.getNextInt32();
                Serial.print("A: ");
                Serial.println(a);
        } else if ( osc.fullMatch("/X") && typeTags[0] == 'i' ) { //typetag is bool receive 1 or 0 as int
                x = osc.getNextInt32();
                Serial.print("X: ");
                Serial.println(x);
        } else  if ( osc.fullMatch("/B") && typeTags[0] == 'i' ) { //typetag is bool receive 1 or 0 as int
                b = osc.getNextInt32();
                Serial.print("B: ");
                Serial.println(b);
        }

}


//==========
//== LOOP ==
//==========
void loop() {

        // CHECK FOR OSC MESSAGES OVER UDP
        // ===============================

        // CHECK IF AN UDP PACKET WAS RECEIVED:
        // udp.parsePacket() RETURNS ture IF IT AN UDP PACKET WAS RECEIVED
        if ( udp.parsePacket() ) {

                // COPY THE PACKET INTO A BUFFER THAT WILL THEN BE USED BY TinyOsc
                // udp.read() RETURNS THE NUMBER OF chars THAT WERE RECEIVED
                int packetSize = udp.read(udpRxBuffer, UDP_RX_BUFFER_MAX_SIZE);
//
//                Serial.println("***UDP***");
//                Serial.print("Received packet of size: ");
//                Serial.println(packetSize);
//                Serial.print("From: ");
                IPAddress remoteIp = udp.remoteIP();
//                Serial.println(remoteIp);

                // PARSE THE OSC MESSAGES FROM THE BUFFER
                // <TinyOsc>.parse( buffer name, size of the data, callback function)
                // FOR EVERY PARSED OSC MESSAGE IN  udpRxBuffer THE receivedOscMessage WILL BE CALLED
                osc.parse( udpRxBuffer, packetSize, receivedOscMessage);

        }



        // UPDATE THE BUTTON
        button.update();

        if ( button.rose() ) {

                // SEND OSC MESSAGES OVER UDP
                // ===============================

                // <TinyOsc>.writeMessage( name of buffer to write to , the maximum size of the buffer , the address , the format string , data... )
                // THE FORMAT STRING MUST MATCH THE DATA
                // 'i':32-bit integer
                // IN THIS CASE, THE DATA IS 1 (integer)
                int udpTxBufferLength = osc.writeMessage( udpTxBuffer, UDP_TX_BUFFER_MAX_SIZE ,  "/button",  "i",   1 );

                // udpTxBuffer NOW CONTAINS THE OSC MESSAGE AND WE SEND IT OVER UDP
                udp.beginPacket( udpTxIp , udpTxPort );
                udp.write( udpTxBuffer ,  udpTxBufferLength );
                udp.endPacket();

        } else if ( button.fell() ) {

                // <TinyOsc>.writeMessage( name of buffer to write to , the maximum size of the buffer , the address , the format string , data... )
                // THE FORMAT STRING MUST MATCH THE DATA
                // 'i':32-bit integer
                // IN THIS CASE, THE DATA IS 0 (integer)
                int udpTxBufferLength = osc.writeMessage( udpTxBuffer, UDP_TX_BUFFER_MAX_SIZE ,  "/button",  "i",   0 );

                // udpTxBuffer NOW CONTAINS THE OSC MESSAGE AND WE SEND IT OVER UDP
                udp.beginPacket( udpTxIp , udpTxPort );
                udp.write( udpTxBuffer ,  udpTxBufferLength );
                udp.endPacket();
        }
}
