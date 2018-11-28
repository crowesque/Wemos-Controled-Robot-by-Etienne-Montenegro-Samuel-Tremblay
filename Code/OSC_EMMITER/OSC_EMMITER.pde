// INTALL oscP5 FROM THE LIBRARY MANAGER

import net.java.games.input.*;
import org.gamecontrolplus.*;
import org.gamecontrolplus.gui.*;

ControlIO control;
ControlDevice xbox360;

import oscP5.*;
import netP5.*;

OscP5 oscP5;

/* myRemoteLocation is a NetAddress. a NetAddress takes 2 parameters,
 * an ip address and a port number. myRemoteLocation is used as parameter in
 * oscP5.send() when sending osc packets to another computer, device,
 * application. usage see below. for testing purposes the listening port
 * and the port of the remote location address are the same, hence you will
 * send messages back to this sketch. 
 */
NetAddress remoteLocation = new NetAddress("10.0.3.53", 7777); // wemos adress ipv4 + port


color backgroundColor = color(0);

int a, b,jL ,lb,rb, lastA = 3 , lastB = 3 , lastJL = 3 , lastLB =3 , lastRB = 3;

public void userInput() {
  boolean _a, _b, _rb, _lb;

  jL = floor(map( xbox360.getSlider("JL").getValue(), -1, 1, -100, 100));
  
  if( jL <= 100 && jL > 35 ) jL = 35 ;
  if( jL >=-100 && jL< -35 ) jL = -35 ;

   _lb = xbox360.getButton("LB").pressed();
  _rb = xbox360.getButton("RB").pressed();
  
  
  _a = xbox360.getButton("A").pressed();
  _b = xbox360.getButton("B").pressed();

  a = _a ? 100 : 0; //send 100 to pwm _CW (GOES FOWARD)
  b = _b ? 100 : 0; //send 100 to pwm _CCW(GOES IN REVERSE)
  lb = _lb ? 1 : 0; 
  rb = _rb ? 1 : 0;
}



void setup() {
  
  size(400, 400);

  /* start oscP5, listening for incoming messages at port 7777 */
  oscP5 = new OscP5(this, 12500);
  
  
  control = ControlIO.getInstance(this);
  xbox360 = control.getMatchedDevice("Robot_Config_2");
  if (xbox360 == null) {
    println("No suitable device configured");
    System.exit(-1); // End the program NOW!
    
  }
}


void draw() {

  userInput();
  oscSend();
  background(backgroundColor);
}



void oscSend() {

  
  
  
  
  
  if (jL != lastJL){
  /* in the following different ways of creating osc messages are shown by example */
  println(jL);
  OscMessage messJL = new OscMessage("/JL");
  messJL.add(jL); /* add an int to the osc message */
  println(jL);
  /* send the message */

  oscP5.send(messJL, remoteLocation);
  lastJL= jL;
  } 
 
 if( a != lastA){

  OscMessage messA = new OscMessage("/A");
  messA.add(a); /* add an int to the osc message */
    println(a);

  /* send the message */
  oscP5.send(messA, remoteLocation);
  lastA = a ; 
 }

if( b !=  lastB){
  OscMessage messB = new OscMessage("/B");
  messB.add(b); /* add an int to the osc message */
    println(b);

  /* send the message */
  oscP5.send(messB, remoteLocation);
  
  lastB = b;
}

if( lb != lastLB){

  OscMessage messLB = new OscMessage("/LB");
  messLB.add(lb); /* add an int to the osc message */
    println(lb);

  /* send the message */
  oscP5.send(messLB, remoteLocation);
  lastLB = lb ; 
 }

if( rb !=  lastRB){
  OscMessage messRB = new OscMessage("/RB");
  messRB.add(rb); /* add an int to the osc message */
    println(rb);

  /* send the message */
  oscP5.send(messRB, remoteLocation);
  
  lastRB = rb;
}

}








/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  /* print the address pattern and the typetag of the received OscMessage */
  println("### received an osc message:");
  println("addrpattern: "+theOscMessage.addrPattern());
  println("typetag: "+theOscMessage.typetag());

  String address = theOscMessage.addrPattern();

  if ( address.equals("/button") ) {

    // Get the first int value
    int intValue = theOscMessage.get(0).intValue();
    if ( intValue == 0 )  backgroundColor = color(0);
    else {  
      backgroundColor = color(255);
    }
  }
}
