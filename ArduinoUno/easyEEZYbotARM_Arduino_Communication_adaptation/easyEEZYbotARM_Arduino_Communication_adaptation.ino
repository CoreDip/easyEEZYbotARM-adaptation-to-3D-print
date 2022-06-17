
/*
   <HelloWorld,10,0,40,80,1000,2000,1000,1000>
   <HelloWorld,10,90,50,120,1000,2000,1000,1000>
*/

#include <Arduino.h>
#include "ServoEasing.hpp"

#define VERSION "3.1"
#define ACTION_TIME_PERIOD 1000
const int SERVO1_PIN = 9; //servo 1
const int SERVO2_PIN = 10; //servo 2
const int SERVO3_PIN = 11; //servo 3
const int SERVO0_PIN = 8; //servo 0 (effector)

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
char messageFromPC[buffSize] = {0};

unsigned long curMillis;

float floatFromPC0 = 10;
float floatFromPC1 = 0;
float floatFromPC2 = 90;
float floatFromPC3 = 60;
int intFromPC0 = 1000;
int intFromPC1 = 1000;
int intFromPC2 = 1000;
int intFromPC3 = 1000;
float last_servoAngle_q1 = floatFromPC1;
float last_servoAngle_q2 = floatFromPC2;
float last_servoAngle_q3 = floatFromPC3;
float last_servoAngle_EE = floatFromPC0;

ServoEasing Servo1;
ServoEasing Servo2;
ServoEasing Servo3;
ServoEasing Servo0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Servo1.attach(SERVO1_PIN);
  Servo2.attach(SERVO2_PIN);
  Servo3.attach(SERVO3_PIN);
  Servo0.attach(SERVO0_PIN);

  Servo1.setEasingType(EASE_CUBIC_IN_OUT);
  Servo2.setEasingType(EASE_CUBIC_IN_OUT);
  Servo3.setEasingType(EASE_CUBIC_IN_OUT);
  Servo0.setEasingType(EASE_CUBIC_IN_OUT);
  
  Servo1.write(last_servoAngle_q1);
  Servo2.write(last_servoAngle_q2);
  Servo3.write(last_servoAngle_q3);
  Servo0.write(last_servoAngle_EE);

  delay(500);

  Serial.println("<Arduino is ready>");
}

void loop() {

  curMillis = millis();
  getDataFromPC();

  if (newDataFromPC == true)  {
    actionInstructionsFromPC();
    replyToPC();
  }
  
}


void actionInstructionsFromPC() {

  float servoAngle_q1 = floatFromPC1;
  float servoAngle_q2 = floatFromPC2;
  float servoAngle_q3 = floatFromPC3;
  float servoAngle_EE = floatFromPC0;

  int servoTime_q1 = intFromPC1;
  int servoTime_q2 = intFromPC2;
  int servoTime_q3 = intFromPC3;
  int servoTime_EE = intFromPC0;

  if (servoAngle_q1 != last_servoAngle_q1) {
    Serial.println(F("Сервопривод 1 змінює кут повороту"));
    Servo1.startEaseToD(servoAngle_q1, servoTime_q1);
  }
  if (servoAngle_q2 != last_servoAngle_q2) {
    Serial.println(F("Сервопривод 2 змінює кут повороту"));
    Servo2.startEaseToD(servoAngle_q2, servoTime_q2);
  }
  if (servoAngle_q3 != last_servoAngle_q3) {
    Serial.println(F("Сервопривод 3 змінює кут повороту"));
    Servo3.startEaseToD(servoAngle_q3, servoTime_q3);
  }
  if (servoAngle_EE != last_servoAngle_EE) {
    Serial.println(F("Сервопривод 0 змінює кут повороту"));
    Servo0.startEaseToD(servoAngle_EE, servoTime_EE);
  }
  
  last_servoAngle_q1 = servoAngle_q1;
  last_servoAngle_q2 = servoAngle_q2;
  last_servoAngle_q3 = servoAngle_q3;
  last_servoAngle_EE = servoAngle_EE;

}

void getDataFromPC() {

  if (Serial.available() > 0 && newDataFromPC == false) {

    char x = Serial.read();

    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    
    if (readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }
    
    if (x == startMarker) {
      bytesRecvd = 0;
      readInProgress = true;
    }
  }
}

void parseData() {
 
  char * strtokIndx; 

  strtokIndx = strtok(inputBuffer, ","); 
  strcpy(messageFromPC, strtokIndx); 

  strtokIndx = strtok(NULL, ","); 
  floatFromPC0 = atof(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  floatFromPC1 = atof(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  floatFromPC2 = atof(strtokIndx);
  
  strtokIndx = strtok(NULL, ",");
  floatFromPC3 = atof(strtokIndx);     

  strtokIndx = strtok(NULL, ",");
  intFromPC0 = atoi(strtokIndx);     

  strtokIndx = strtok(NULL, ",");
  intFromPC1 = atoi(strtokIndx);     

  strtokIndx = strtok(NULL, ",");
  intFromPC2 = atoi(strtokIndx);     

  strtokIndx = strtok(NULL, ",");
  intFromPC3 = atoi(strtokIndx);    

}


void replyToPC() {
  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print(F("<Msg "));
    Serial.print(messageFromPC);
    Serial.print(F(" floatFromPC0 "));
    Serial.print(floatFromPC0);
    Serial.print(F(" floatFromPC1 "));
    Serial.print(floatFromPC1);
    Serial.print(F(" floatFromPC2 "));
    Serial.print(floatFromPC2);
    Serial.print(F(" floatFromPC3 "));
    Serial.print(floatFromPC3);
    Serial.print(F(" intFromPC0 "));
    Serial.print(intFromPC0);
    Serial.print(F(" intFromPC1 "));
    Serial.print(intFromPC1);
    Serial.print(F(" intFromPC2 "));
    Serial.print(intFromPC2);
    Serial.print(F(" intFromPC3 "));
    Serial.print(intFromPC3);
    Serial.print(F(" Time "));
    Serial.print(curMillis / 1000);
    Serial.println(F(">"));
  }
}
