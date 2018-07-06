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


#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <EEPROM.h>
#include <Servo.h>
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



void readButtons(void){
  if(!digitalRead(BUTTON_ADD)&&laserLevel<255){
    laserLevel++;
    EEPROM.write(10,laserLevel);
    printLCD();
    delay(100);
  }
  if(!digitalRead(BUTTON_RMV)&&laserLevel>0){
    laserLevel--;
    EEPROM.write(10,laserLevel);
    printLCD();
    delay(100);
  }
}



void writeLCD(void){
  lcd.setCursor(13,0);
  if(laserLevel>=100){
    lcd.print(laserLevel);
  }
  else if(laserLevel>=10){
    lcd.print(" ");
    lcd.print(laserLevel);
  }
  else{
    lcd.print("  ");
    lcd.print(laserLevel);
  }
  #ifdef TURBINE_PIN
    lcd.setCursor(13,1);
    if(turbineEnable){
      if(turbineLevel>=100){
        lcd.print(turbineLevel);
      }
      else if(turbineLevel>=10){
        lcd.print(" ");
        lcd.print(turbineLevel);
      }
      else{
        lcd.print("  ");
        lcd.print(turbineLevel);
      }
    }
    else{
      lcd.print("OFF");
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
  #endif
  LCD.init();
  LCD.backlight();
  lcd.setCursor(0,0);
  lcd.print(" LASER CONTROL  ");
  lcd.setCursor(0,1);
  lcd.print("LaCajaMakerSpace");
  delay(2500);
  lcd.setCursor(0,0);
  lcd.print("> LASER:     ---");
  lcd.setCursor(0,1);
  lcd.print("  TURBINE:   OFF");
  
}

void loop(){
  
  readButtons();
  
  if( (laserLevel!=laserLevelPrev) || (turbineLevel!=turbineLevelPrev) || (turbineEnable!=turbineEnablePrev) ){
    writeActuators();
    laserLevelPrev=laserLevel;
    turbineLevelPrev=turbineLevel;
    turbineEnablePrev=turbineEnable;
  }
  
}
