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
 * Aaron G.
 * Jul 2018
 */


#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <EEPROM.h>
#include <Servo.h>
#define POT_PIN       A0
#define BUTTON_ADD    8
#define BUTTON_ENT    9
#define BUTTON_RMV    10
//#define TURBINE_PIN   11
//#define INVERT_ANALOG_SIGNAL
#define MIRROR_BITS


byte laserLevel,laserLevelPrev;
byte dataToPort;
#ifdef TURBINE_PIN
  bool turbineEnable=false,turbineEnablePrev;
  byte turbineLevel,turbineLevelPrev;
  Servo turbine;
#endif
LiquidCrystal_I2C LCD(0x27,16,2);




void readPotentiometer(void){
  laserLevel=map(analogRead(POT_PIN),0,1023,0,255);
}

/*
void readButtons(void){
  if(!digitalRead(BUTTON_ADD)&&laserLevel<255){
    laserLevel++;
    printLCD();
    delay(100);
  }
  if(!digitalRead(BUTTON_RMV)&&laserLevel>0){
    laserLevel--;
    printLCD();
    delay(100);
  }
}
*/

void writeLCD(void){
  LCD.setCursor(13,0);
  if(laserLevel>=100){
    LCD.print(laserLevel);
  }
  else if(laserLevel>=10){
    LCD.print(" ");
    LCD.print(laserLevel);
  }
  else{
    LCD.print("  ");
    LCD.print(laserLevel);
  }
  #ifdef TURBINE_PIN
    LCD.setCursor(13,1);
    if(turbineEnable){
      if(turbineLevel>=100){
        LCD.print(turbineLevel);
      }
      else if(turbineLevel>=10){
        LCD.print(" ");
        LCD.print(turbineLevel);
      }
      else{
        LCD.print("  ");
        LCD.print(turbineLevel);
      }
    }
    else{
      LCD.print("OFF");
    }
  #endif
}


void writeActuators(void){
  
  #ifdef MIRROR_BITS
    dataToPort = ( laserLevel * 0x0202020202ULL & 0x010884422010ULL ) % 1023;
  #else
    dataToPort = laserLevel;
  #endif
  
  #ifdef INVERT_ANALOG_SIGNAL
    PORTD = 255 - dataToPort;
  #else
    PORTD = dataToPort;
  #endif
  
  #ifdef TURBINE_PIN
    if(turbineEnable) turbine.writeMicroseconds(map(turbineLevel,0,255,1000,2000));
    else turbine.writeMicroseconds(1000);
  #endif
  
}


void setup(){
  pinMode(BUTTON_ADD,INPUT);
  pinMode(BUTTON_ENT,INPUT);
  pinMode(BUTTON_RMV,INPUT);
  DDRD = B11111111;
  laserLevel = EEPROM.read(10);
  #ifdef TURBINE_PIN
    turbine.attach(TURBINE_PIN);
    turbineLevel = EEPROM.read(20);
    turbine.writeMicroseconds(1000);
  #endif
  LCD.init();
  LCD.backlight();
  LCD.setCursor(0,0);
  LCD.print(" LASER CONTROL  ");
  LCD.setCursor(0,1);
  LCD.print("LaCajaMakerSpace");
  delay(2500);
  LCD.setCursor(0,0);
  LCD.print("> LASER:     ---");
  LCD.setCursor(0,1);
  #ifdef TURBINE_PIN
    LCD.print("  TURBINE:   OFF");
  #else
    LCD.print("  TURBINE:   N-C");
  #endif
  
}


void loop(){
  
  //readButtons();
  readPotentiometer();
  
  #ifdef TURBINE_PIN
    if( (laserLevel!=laserLevelPrev) || (turbineLevel!=turbineLevelPrev) || (turbineEnable!=turbineEnablePrev) ){
      writeActuators();
      EEPROM.write(10,laserLevel);
      EEPROM.write(20,turbineLevel);
      laserLevelPrev=laserLevel;
      turbineLevelPrev=turbineLevel;
      turbineEnablePrev=turbineEnable;
    }
  #else
    if( laserLevel!=laserLevelPrev ){
      writeLCD();
      writeActuators();
      EEPROM.write(10,laserLevel);
      laserLevelPrev=laserLevel;
    }
  #endif
  
  
}
