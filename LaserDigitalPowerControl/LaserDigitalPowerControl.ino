/*
 * DIGITAL POWER CONTROL FOR A K40 CNC LASER DRIVER
 *  
 * ATMega328P uses a R-2R resistor net in Port D to 
 * generate an analog voltage between 0V and 5V with 
 * 8 bit resolution (256 different levels). This analog
 * signal is used by the CNC LASER Driver to control 
 * output power.
 *
 *
 * 
 * Created on 06/07/2018
 * By Aaron G.
 */


#include <EEPROM.h>
#include <Servo.h>
#define BUTTON_ADD    8
#define BUTTON_ENT    9
#define BUTTON_RMV    10
//#define TURBINE_PIN   11
#define MIRROR_BITS




byte laserLevel;
byte dataToPort;
#ifdef TURBINE_PIN
  byte turbineLevel;
  Servo turbine;
#endif


void setup(){
  pinMode(BUTTON_ADD,INPUT);
  pinMode(BUTTON_ENT,INPUT);
  pinMode(BUTTON_RMV,INPUT);
  DDRD=B11111111;
  laserLevel=EEPROM.read(10);
  #ifdef TURBINE_PIN
    turbine.attach(TURBINE_PIN);
    turbineLevel=EEPROM.read(20);
  #endif
  
}

void loop(){

  if(!digitalRead(BUTTON_ADD)&&laserLevel<255){
    laserLevel++;
    EEPROM.write(10,laserLevel);
    delay(100);
  }
  if(!digitalRead(BUTTON_RMV)&&laserLevel>0){
    laserLevel--;
    EEPROM.write(10,laserLevel);
    delay(100);
  }
  
  #ifdef MIRROR_BITS
    dataToPort = ( laserLevel * 0x0202020202ULL & 0x010884422010ULL ) % 1023;
  #else
    dataToPort = laserLevel;
  #endif
  PORTD=dataToPort;
  
  #ifdef TURBINE_PIN
    turbine.writeMicroseconds(map(turbineLevel,0,255,1000,2000));
  #endif
  
}
