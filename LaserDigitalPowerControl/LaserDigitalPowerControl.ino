#include <EEPROM.h>
#include <Servo.h>
#define BUTTON_ADD    8
#define BUTTON_ENT    9
#define BUTTON_RMV    10
#define TURBINE_PIN   11

byte laserLevel=EEPROM.read(10);
byte turbineLevel=EEPROM.read(20);

Servo turbine;

void setup(){
  pinMode(BUTTON_ADD,INPUT);
  pinMode(BUTTON_ENT,INPUT);
  pinMode(BUTTON_RMV,INPUT);
  DDRD=B11111111;
  turbine.attach(TURBINE_PIN);
  
}

void loop(){

  if(!digitalRead(BUTTON_ADD)&&laserLevel<255){
    laserLevel++;
    EEPROM.write(10,laserLevel);
    PORTD=laserLevel;
    delay(100);
  }
  if(!digitalRead(BUTTON_RMV)&&laserLevel>0){
    laserLevel--;
    EEPROM.write(10,laserLevel);
    PORTD=laserLevel;
    delay(100);
  }
 
  //turbine.writeMicroseconds(map(turbineLevel,0,255,1000,2000));

}
