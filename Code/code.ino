

// ETIENNE MONTENEGRO & SAMUEL TREMBLAY
// ROBOT LEAGUE 2018

#include <WEMOS_Motor.h>

// INCLUDE ESP8266WiFi:
#include <ESP8266WiFi.h>

Motor ML(0x30, _MOTOR_A, 1000); //DECLARING LEFT MOTOR
Motor MR(0x30, _MOTOR_B, 1000); //DECLARING RIGHT MOTOR


// Please change the following values with your network settings:
const char* ssid     = "*******"; //Your network id
const char* password = "*******"; //Your network Password

IPAddress ip(10, 0, 3, 53); //wemos ipv4
IPAddress gateway(10, 0, 3, 1);
IPAddress subnet(255, 255, 255, 0);

// UDP
int udpReceivePort = 7777;
IPAddress udpTxIp = IPAddress(10, 0, 3,182); //computer ipv4
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
#include <tinyosc.h>
TinyOsc osc;

//GLOBAL VARIABLE
int jL, jR, speedL, speedR, compensationL, compensationR,pwm , pwmR , pwmL, lb ,rb;
bool dir;

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

  // PRINT CONNECTION SETTINGS
  Serial.println();
  Serial.println("WiFi connected, IP address: ");
  Serial.println( WiFi.localIP() );

  udp.begin(udpReceivePort); // BEGIN LISTENING ON UDP PORT udpReceivePort

}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {

  // GET THE TYPE TAGS AS A C STRING POINTER
  char * typeTags = osc.getTypeTags();

  if ( osc.fullMatch("/JL") && typeTags[0] == 'i') {

    int _comp = osc.getNextInt32();

    if ( _comp < 0 ) {
      compensationL = abs(_comp);

    } else if ( _comp > 0 ) {
      compensationR = _comp;
    } else {

      compensationL = 0;
      compensationR = 0;

    }

  // Serial.println( compensationL);
  // Serial.println( compensationR);    

  }



 if ( osc.fullMatch("/LB") && typeTags[0] == 'i') {
     lb = osc.getNextInt32()*100;
   // if( lb == 1 )pwmL = 0 ;
   //     Serial.println(lb);


  }


   if ( osc.fullMatch("/RB") && typeTags[0] == 'i') {
     rb = osc.getNextInt32() *100;
  //  Serial.println(rb);
   // if( rb == 1 )pwmR = 0 ;

  }


  if ( osc.fullMatch("/A") && typeTags[0] == 'i') {
    int _pwm = osc.getNextInt32();
    pwm = _pwm;
   // Serial.println(_pwm);
    dir = true;

  }



  if ( osc.fullMatch("/B") && typeTags[0] == 'i') {
    int _pwm = osc.getNextInt32();
  //  Serial.println(_pwm);
     pwm = _pwm;
    dir = false;

  }




 



}


//==========
//== LOOP ==
//==========
void loop() {

Serial.println(pwmR);
Serial.println(pwmL);
 moving();
  



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

}



void moving(){
  
  if(dir){

    foward(pwm);
    
  } else if ( !dir){

   reverse(pwm);
    
  }
  
  
  
  
  }

void foward(int pwm) {
pwmL =  pwm - compensationL -lb;
pwmR = pwm - compensationR - rb;
  if( pwmR < 0 ) pwmR = 0;
  if( pwmL < 0 ) pwmL = 0;

  ML.setmotor( _CCW, pwmL );
  MR.setmotor( _CCW, pwmR);

}


void reverse(int pwm) {

  ML.setmotor( _CW, pwm - compensationL);
  MR.setmotor( _CW, pwm - compensationR);

}
