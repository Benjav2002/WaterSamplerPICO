#include <Arduino.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <Wire.h>
#include "TSYS01.h"
#include <string>
#include <stdlib.h>

String command;
const int MAX_SIZE = 10; // Maximum number of strings in the list
String stringList[MAX_SIZE]; // Array of strings to store the list
int listSize = 0; // Current size of the list

TSYS01 sensorTemp;
String prevstate;


// put function declarations here:
float read_onboard_temperature();
void blink();
String read_serial();
float read_external_temperature();


// Core 0

void setup() {

  //abir porta serie
  Serial.begin(9600);


}

void loop() {

  command = read_serial();
  
}


//Core 1

void setup1() {

  //sensor de temperatura do próprio pico
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  //sensor de temperatura externo
  /*
  Wire.begin();
    while (!sensorTemp.init()) {
        Serial.println("TSYS01 device failed to initialize!");
        delay(2000);
    }

  */

  
  //led 
  pinMode(LED_BUILTIN, OUTPUT);


}

void loop1() {

  //test sensor de temperatura do próprio pico
    if (stringList[0] == "blink")
  {
    blink();
    prevstate = stringList[0];
  }
  else if (stringList[0] == "iTemp")
  {
    read_onboard_temperature();
    prevstate = stringList[0];
  }
  else if (stringList[0] == "eTemp")
  {
    read_external_temperature();
    prevstate = stringList[0];
  }
  else if (stringList[0] == "stop")
  {
    prevstate = stringList[0];
  }
  else prevstate = stringList[0];

  listSize = 0;
}


String read_serial() {

    int startIndex = 0;
    
    if (Serial.available()) {
    command = Serial.readStringUntil('?'); //lê a string
    command.trim();     //retira os espaços em branco

    for (int i = 0; i < command.length(); i++){
      if (command.charAt(i) == '-' && listSize < MAX_SIZE) {
        stringList[listSize] = command.substring(startIndex, i);
        startIndex = i + 1;
        listSize++;
      }
    }
    if(listSize == 0){
      stringList[0] = command;
    }
    }
    return command;
}

float read_onboard_temperature() {
    
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    Serial.print("iTemp-");
    Serial.print(tempC);
    Serial.println('?');

    delay(5000);

    return tempC;
}

void blink() {
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

float read_external_temperature() {

  sensorTemp.read();
  Serial.print("eTemp-");
  Serial.print(sensorTemp.temperature());
  Serial.println('?');
  delay(1000);

  return sensorTemp.temperature();
}
 
/*// Firgelli Automations
  // Limited or no support: we do not have the resources for Arduino code support
  //
  // Program enables momentary direction control of actuator using push button
 
 
#include <elapsedMillis.h>
elapsedMillis timeElapsed;

int RPWM = 10;   
int LPWM = 11;
int sensorPin = A0;

int sensorVal;
int Speed=255;
float strokeLength = 2.0;                           //customize to your specific stroke length
float extensionLength;

int maxAnalogReading;
int minAnalogReading;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
  maxAnalogReading = moveToLimit(1);
  minAnalogReading = moveToLimit(-1);
}

void loop(){
  Serial.println("Extending...");
  sensorVal = analogRead(sensorPin);
  while(sensorVal < maxAnalogReading){
    driveActuator(1, Speed);
    displayOutput();  
    delay(20);
  }
  driveActuator(0, Speed);
  delay(1000);
  
  Serial.println("Retracting...");
  sensorVal = analogRead(sensorPin);
  while(sensorVal > minAnalogReading){
    driveActuator(-1, Speed);
    displayOutput();  
    delay(20);
  }
  driveActuator(0, Speed);
  delay(1000);
}

int moveToLimit(int Direction){
  int prevReading=0;
  int currReading=0;
  do{
    prevReading = currReading;
    driveActuator(Direction, Speed);
    timeElapsed = 0;
    while(timeElapsed < 200){ delay(1);}           //keep moving until analog reading remains the same for 200ms
    currReading = analogRead(sensorPin);
  }while(prevReading != currReading);
  return currReading;
}

float mapfloat(float x, float inputMin, float inputMax, float outputMin, float outputMax){
 return (x-inputMin)*(outputMax - outputMin)/(inputMax - inputMin)+outputMin;
}

void displayOutput(){
  sensorVal = analogRead(sensorPin);
    extensionLength = mapfloat(sensorVal, float(minAnalogReading), float(maxAnalogReading), 0.0, strokeLength);
    Serial.print("Analog Reading: ");
    Serial.print(sensorVal);
    Serial.print("\tActuator extension length: ");
    Serial.print(extensionLength);
    Serial.println(" inches");  
}

void driveActuator(int Direction, int Speed){
  switch(Direction){
    case 1:       //extension
      analogWrite(RPWM, Speed);
      analogWrite(LPWM, 0);
      break;
   
    case 0:       //stopping
      analogWrite(RPWM, 0);
      analogWrite(LPWM, 0);
      break;

    case -1:      //retraction
      analogWrite(RPWM, 0);
      analogWrite(LPWM, Speed);
      break;
  }
}
*/