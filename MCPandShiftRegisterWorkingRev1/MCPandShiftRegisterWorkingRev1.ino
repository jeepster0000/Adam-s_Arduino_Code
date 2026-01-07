#include <Wire.h>
#include <MCP23017.h>

//**************************************************************//
// This does require SCA and SCL over I2C


//The goal is to have 14 Buttons and 16 LEDS using only 5 pins.
//
// NOTES:  This code is to combine  the MCP23017 and the 74HC595  
//  Momentary buttons are on the MCP23017
// LED's are on the two 74HC595.  T
//****************************************************************




//74HC595 PINS
//Pin connected to ST_CP of 74HC595
int latchPin = 9;
//Pin connected to SH_CP of 74HC595
int clockPin = 8;
//Pin connected to DS of 74HC595
int dataPin = 7;
//byte to send to shift register
byte register1 = 0b00000000;
byte register2 = 0b00000000;

int led0 = 0; //array 1
int led1 = 1; //array 1
int led2 = 2; //array 1
int led3 = 3; //array 1
int led4 = 4; //array 1
int led5 = 5; //array 1
int led6 = 6; //array 1
int led7 = 7; //array 1
int led8 = 0; //array 2
int led9=  1; //array 2
int led10 = 2; //array 2
int led11 = 3; //array 2
int led12 = 4; //array 2
int led13 = 5; //array 2
int led14 = 6; //array 2
int led15 = 7; //array 2

int ledarray1[] = {0,0,0,0,0,0,0,0};
int ledarray2[] = {0,0,0,0,0,0,0,0};
int ledarray1old[] = {0,0,0,0,0,0,0,0};
int ledarray2old[] = {0,0,0,0,0,0,0,0};

//I2C address of MCP23017, default is usually 0x20
#define MCP23017_ADDR 0x20

//Create MCP23017 object with the I2C address
MCP23017 mcp(MCP23017_ADDR);

//Variables to store previous button states
//bool prevButtonA[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
//bool prevButtonB[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
bool prevButton0 = HIGH;
bool prevButton1 = HIGH;
bool prevButton2 = HIGH;
bool prevButton3 = HIGH;
bool prevButton4 = HIGH;
bool prevButton5 = HIGH;
bool prevButton6 = HIGH;
bool prevButton7 = HIGH;
bool prevButton8 = HIGH;
bool prevButton9 = HIGH;
bool prevButton10 = HIGH;
bool prevButton11 = HIGH;
bool prevButton12 = HIGH;
bool prevButton13 = HIGH;
bool prevButton14 = HIGH;
bool prevButton15 = HIGH;


void setup() {
  //*********************************************************
  //*****      Cascading Shift Register Setup           *****
  //**********************************************************
  // Set all the pins as output
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  
 digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b000000000000000000000000);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b000000000000000000000000);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b000000000000000000000000);
  shiftOut(dataPin, clockPin, MSBFIRST, 0b000000000000000000000000);
  digitalWrite(latchPin, HIGH);
  //shiftOut(dataPin, clockPin, MSBFIRST, register2); 
  //shiftOut(dataPin, clockPin, MSBFIRST, register1);
  //shiftOut(dataPin, clockPin, MSBFIRST, register2);
   digitalWrite(latchPin, HIGH);
for(int i=0; i<8; i++){
  ledarray1[i]=0;
  ledarray2[i]=0;
 bitWrite(register1, i, ledarray1[i]);
 bitWrite(register2, i, ledarray2[i]);
 ledarray1old[i] =ledarray1[i];
 ledarray2old[i] =ledarray2[i];
  }
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, register1);
  shiftOut(dataPin, clockPin, MSBFIRST, register2);
 
 
 
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
//**********************************************************
//*****                 MCP Setup                       ****
//**********************************************************
  //MCP
  //Start I2C communication
  Wire.begin();

  //Initialize the MCP23017
  mcp.init();
  //THIS CODE WORKED PREVIOUSLY
  
 /* mcp.pinMode(0, INPUT_PULLUP, false);
  mcp.pinMode(1, INPUT_PULLUP, false);
  mcp.pinMode(2, INPUT_PULLUP, false);
  mcp.pinMode(3, INPUT_PULLUP, false);
  mcp.pinMode(4, INPUT_PULLUP, false);
  mcp.pinMode(5, INPUT_PULLUP, false);
  mcp.pinMode(6, INPUT_PULLUP, false);
  mcp.pinMode(7, INPUT_PULLUP, false);
 

  
  mcp.pinMode(8, INPUT_PULLUP, false);
  mcp.pinMode(9, INPUT_PULLUP, false);
  mcp.pinMode(10, INPUT_PULLUP, false);
  mcp.pinMode(11, INPUT_PULLUP, false);
  mcp.pinMode(12, INPUT_PULLUP, false);
  mcp.pinMode(13, INPUT_PULLUP, false);
  mcp.pinMode(14, INPUT_PULLUP, false);
 // mcp.pinMode(15, INPUT_PULLUP, false);
*/


//Set pin A0 and A7 as input with internal pull-up resistors
//Set pin B0 and B7 as input with internal pull-up resistors

//*****************************************************************
//*****                IMPORTANT NOTE!!!!!!!!!!               *****
//***** Note Pin B7 does not work as an input on the MCP23017.*****  
//***** This is pin 15.  Do not use MCP Pin 15 or B7 as input.*****
//*****************************************************************
//comment out to see if this is the problem
for (int i=0; i<15; i++){
  mcp.pinMode(i, INPUT_PULLUP, false);
}

//*********************************************
//*****      Serial and Debug             *****
//*********************************************
Serial.begin(9600);
}

void loop() {


  //allOff();
   // Turn all outputs OFF (send all 0's)
  //delay(2000);
  //allOn();
  //delay(2000);
 //alternate();
 //delay(2000);
 
 mcpRead();
 //buttonStateCheck();
 arrayCheckChange();
//  Serial.print("Register one is  ");
 //Serial.println(register1);
 //Serial.print("Register two is  ");
 //Serial.println(register2);
}
void alternate(){

   for (int w = 0; w<=10; w++){
   digitalWrite(latchPin, LOW);
   ledarray1[0] = 0;
   ledarray1[1] = 1;
   ledarray1[2] = 0;
   ledarray1[3] = 1;
   ledarray1[4] = 0;
   ledarray1[5] = 1;
   ledarray1[6] = 0;
   ledarray1[7] = 1;
   ledarray2[0] = 0;
   ledarray2[1] = 1;
   ledarray2[2] = 0;
   ledarray2[3] = 1;
   ledarray2[4] = 0;
   ledarray2[5] = 1;
   ledarray2[6] = 0;
   ledarray2[7] = 1;

   //byteArray2[] = {0,1,0,1,0,1,0,1};
   for(int i=0; i<8; i++){
 bitWrite(register1, i, ledarray1[i]);
 bitWrite(register2, i, ledarray2[i]);
   }
shiftOut(dataPin, clockPin, MSBFIRST, register1);
shiftOut(dataPin, clockPin, MSBFIRST, register2);
digitalWrite(latchPin, HIGH);
delay(1000);
digitalWrite(latchPin, LOW);
for(int i=0; i<8; i++){
  ledarray1[i]=1-ledarray1[i];
  ledarray2[i]=1-ledarray2[i];
}
 for(int i=0; i<8; i++){
 bitWrite(register1, i, ledarray1[i]);
 bitWrite(register2, i, ledarray2[i]);
   }
   shiftOut(dataPin, clockPin, MSBFIRST, register1);
   shiftOut(dataPin, clockPin, MSBFIRST, register2);
   digitalWrite(latchPin, HIGH);
   delay(1000);
   }
  }

void allOff(){
// 1. Set the latch pin LOW to enable data shifting
  digitalWrite(latchPin, LOW);

  // 2. Shift out the value 0 to the shift register
  // 0 is a byte value (8 bits of 0s: 00000000)
  // MSBFIRST or LSBFIRST can be used, both work for 0
  register1=0;
  register2=0;

  shiftOut(dataPin, clockPin, MSBFIRST, register1);
  shiftOut(dataPin, clockPin, MSBFIRST, register2);

  // 3. Set the latch pin HIGH to make the new value appear at the output pins
  digitalWrite(latchPin, HIGH);

  // The outputs will now be all LOW.
  // You can add a delay if needed for your application


}
void allOn(){
// 1. Set the latch pin LOW to enable data shifting
  digitalWrite(latchPin, LOW);

  // 2. Shift out the value 0 to the shift register
  // 0 is a byte value (8 bits of 0s: 00000000)
  // MSBFIRST or LSBFIRST can be used, both work for 0
  register1= 0b11111111;
  register2= 0b11111111;
  

  shiftOut(dataPin, clockPin, MSBFIRST, register1);
  shiftOut(dataPin, clockPin, MSBFIRST, register2);

  // 3. Set the latch pin HIGH to make the new value appear at the output pins
  digitalWrite(latchPin, HIGH);

  // The outputs will now be all LOW.
  // You can add a delay if needed for your application

}
//The MCPread code worked individually, but the button state did not. the 
void mcpRead(){
  //Read current state of button A0
  bool currentButton0 = mcp.digitalRead(0);
  //If button is pressed and was not pressed before, toggle LED on B0
  if (currentButton0 == LOW && prevButton0 == HIGH) {
 
    ledarray1[led0]=1-ledarray1[led0];
  }
  //Update previous state of button A0
  prevButton0 = currentButton0;

  //Read current state of button A1
  bool currentButton1 = mcp.digitalRead(1);
  //If button is pressed and was not pressed before, toggle LED on B1
  if (currentButton1 == LOW && prevButton1 == HIGH) {
   
    ledarray1[led1]=1-ledarray1[led1];
  }
  //Update previous state of button A1
  prevButton1 = currentButton1;

 //Read current state of button A2
  bool currentButton2 = mcp.digitalRead(2);
  //If button is pressed and was not pressed before, toggle LED on B2
  if (currentButton2 == LOW && prevButton2 == HIGH) {
  
    ledarray1[led2]=1-ledarray1[led2];
  }
  prevButton2 = currentButton2;
 
 //Read current state of button A3
  bool currentButton3 = mcp.digitalRead(3);
  //If button is pressed and was not pressed before, toggle LED on B3
  if (currentButton3 == LOW && prevButton3 == HIGH) {

    ledarray1[led3]=1-ledarray1[led3];
  }
  prevButton3 = currentButton3;

   //Read current state of button A4
  bool currentButton4 = mcp.digitalRead(4);
  //If button is pressed and was not pressed before, toggle LED on B4
  if (currentButton4 == LOW && prevButton4 == HIGH) {
 
    ledarray1[led4]=1-ledarray1[led4];
  }
  prevButton4 = currentButton4;

   //Read current state of button A5
  bool currentButton5 = mcp.digitalRead(5);
  //If button is pressed and was not pressed before, toggle LED on B5
  if (currentButton5 == LOW && prevButton5 == HIGH) {
   
    ledarray1[led5]=1-ledarray1[led5];
  }
  prevButton5 = currentButton5;

   //Read current state of button A6
  bool currentButton6 = mcp.digitalRead(6);
  //If button is pressed and was not pressed before, toggle LED on B6
  if (currentButton6 == LOW && prevButton6 == HIGH) {
   
    ledarray1[led6]=1-ledarray1[led6];
  }
  prevButton6 = currentButton6;

//Read current state of button A7
  bool currentButton7 = mcp.digitalRead(7);
  //If button is pressed and was not pressed before, toggle LED on B7
  if (currentButton7 == LOW && prevButton7 == HIGH) {
   
    ledarray1[led7]=1-ledarray1[led7];
  }
  //Update previous state of button A7
  prevButton7 = currentButton7;

//Read current state of button A8
  bool currentButton8 = mcp.digitalRead(8);
  //If button is pressed and was not pressed before, toggle LED on B8
  if (currentButton8 == LOW && prevButton8 == HIGH) {
   
    ledarray2[led8]=1-ledarray2[led8];
  }
  //Update previous state of button A8
  prevButton8 = currentButton8;

//Read current state of button A9
  bool currentButton9 = mcp.digitalRead(9);
  //If button is pressed and was not pressed before, toggle LED on B9
  if (currentButton9 == LOW && prevButton9 == HIGH) {
   
    ledarray2[led9]=1-ledarray2[led9];
  }
  //Update previous state of button A9
  prevButton9 = currentButton9;

//Read current state of button A10
  bool currentButton10 = mcp.digitalRead(10);
  //If button is pressed and was not pressed before, toggle LED on B10
  if (currentButton10 == LOW && prevButton10 == HIGH) {
   
    ledarray2[led10]=1-ledarray2[led10];
  }
  //Update previous state of button A10
  prevButton10 = currentButton10;

//Read current state of button A11
  bool currentButton11 = mcp.digitalRead(11);
  //If button is pressed and was not pressed before, toggle LED on B11
  if (currentButton11 == LOW && prevButton11 == HIGH) {
   
    ledarray2[led11]=1-ledarray2[led11];
  }
  //Update previous state of button A11
  prevButton11 = currentButton11;

//Read current state of button A12
  bool currentButton12 = mcp.digitalRead(12);
  //If button is pressed and was not pressed before, toggle LED on B12
  if (currentButton12 == LOW && prevButton12 == HIGH) {
   
    ledarray2[led12]=1-ledarray2[led12];
  }
  //Update previous state of button A12
  prevButton12 = currentButton12;

//Read current state of button A13
  bool currentButton13 = mcp.digitalRead(13);
  //If button is pressed and was not pressed before, toggle LED on B13
  if (currentButton13 == LOW && prevButton13 == HIGH) {
   
    ledarray2[led13]=1-ledarray2[led13];
  }
  //Update previous state of button A13
  prevButton13 = currentButton13;
//Read current state of button A14
  bool currentButton14 = mcp.digitalRead(14);
  //If button is pressed and was not pressed before, toggle LED on B14
  if (currentButton14 == LOW && prevButton14 == HIGH) {
   
    ledarray2[led14]=1-ledarray2[led14];
  }
  //Update previous state of button A14
  prevButton14 = currentButton14;

//Read current state of button A15
  bool currentButton15 = mcp.digitalRead(15);
  //If button is pressed and was not pressed before, toggle LED on B15
  if (currentButton15 == LOW && prevButton15 == HIGH) {
   
    ledarray2[led15]=1-ledarray2[led15];
  }
  //Update previous state of button A15
  prevButton15 = currentButton15;

  //Delay to debounce buttons
  delay(100);

  
}


/*
void buttonStateCheck(){
  bool currentButtonA[8];
  bool currentButtonB[8];
  for (int i= 0; i<8; i++){
   currentButtonA[i] = mcp.digitalRead(i);
   
    if (currentButtonA[i] == LOW && prevButtonA[i] == HIGH) {
   
    ledarray1[i]=1-ledarray1[i];
    prevButtonA[i] = currentButtonA[i];
  }
  currentButtonB[i] = mcp.digitalRead(i+8);
if (currentButtonB[i] == LOW && prevButtonB[i] == HIGH) {
 
    ledarray2[i]=1-ledarray2[i];
    prevButtonB[i] = currentButtonB[i];
  }
//Delay to debounce buttons
  //delay(100);
  }

}
*/

void arrayCheckChange(){
for (int i=0; i<8; i++){
  if (ledarray1old[i] != ledarray1[i]){
    arrayupdate();
Serial.print("LED1 array change ");
Serial.println(i);
  }
  if (ledarray2old[i] != ledarray2[i]){
    arrayupdate();
    Serial.print("LED2 array change ");
Serial.println(i);
}
}
}



void arrayupdate(){
    digitalWrite(latchPin, LOW);
    register1 = 0b00000000;
    register2 = 0b00000000;
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
  for (int w=0; w<8; w++){
    

    bitWrite(register1, w, ledarray1[w]);
    bitWrite(register2, w, ledarray2[w]);
    ledarray1old[w] = ledarray1[w];
    ledarray2old[w] = ledarray2[w];
  }
  shiftOut(dataPin, clockPin, MSBFIRST, register1);
  shiftOut(dataPin, clockPin, MSBFIRST, register2);
  digitalWrite(latchPin, HIGH);
}