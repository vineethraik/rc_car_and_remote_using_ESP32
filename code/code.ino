#define CODE
//#define TEST
//#define TEST_joystick

#ifdef CODE


#define CONTROL_PIN 12
#define globle_delay 1000
#define UDP_server_port 1234

#define inicator_led LED_BUILTIN

#include "libs.h"



int device;

void setup(){
Serial.begin(115200);
pinMode(CONTROL_PIN,INPUT_PULLUP);
pinMode(inicator_led,OUTPUT);
delay(2000);
if(digitalRead(CONTROL_PIN)){
  device=REMOTE;
  Serial.println("Device is REMOTE");
  digitalWrite(inicator_led,HIGH);
}else{
  device=CAR;
  Serial.println("Device is CAR");
  digitalWrite(inicator_led,LOW);
}

delay(2000);

if(device==CAR){
  
  car.init();




}else if(device==REMOTE){
  
  remote.init();



}

}

void loop(){
if(device==CAR){

  car.handle();




}else if(device==REMOTE){

  remote.handle();




}
}

#endif
