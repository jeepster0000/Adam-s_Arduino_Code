#include <Encoder.h>
#include <elapsedMillis.h>

//Encoder setup and arrays
elapsedMillis encoderClickInterval = 0;
const int totalEncCount = 11;
const int arrayEncCount = totalEncCount -1;
//These should be filled with encoder values.  May change to inturrept style in future
float encoderArray[arrayEncCount][3]; //[Pin A][Pin A Value][Pin B][Pin B Value]
float encoderArrayOld[arrayEncCount][3]; //[Pin A][Pin A Value][Pin B][Pin B Value]
//Encoder parameter arrays.  These arrays hold the values to be passed to function
int encPosArray[arrayEncCount];
Encoder encVarName[arrayEncCount];
float encDataRefFloat[arrayEncCount];
int encDataRefInt[arrayEncCount];
float encLowValue[arrayEncCount];
float encHighValue[arrayEncCount];
bool encValCont[arrayEncCount];
float encSlowSpeed[arrayEncCount];
float encFastSpeed[arrayEncCount];




//Invividual Encoder Paramerts
// Heading Bug Encoder
Encoder HeadingBugEncoder(27, 28);  // Heading Bug on 5 6
int HeadingBugEncPos =0;
//short HeadingBugEncoderPrevious = 0;
float HeadingBugDataref = 0;
int unUsedInt =0;

void setup() {
Serial.begin(9600);  //for debugging


}



void loop(){
//encoderValChange(HeadingBugEncoder, HeadingBugDataref, unUsedInt);
encoderCheck();

}
void encoderCheck(){
for(int i=0, i<arrayEncCount, i++){
  if (encoderArray[i][1]!= encoderArrayOld[i][1] || encoderArray[i][1]!= encoderArrayOld[i][1]){

  
encoderValChange(encVarName[i], encDataRefFloat[i], encDataRefInt[i], encLowValue[i], encHighValue[i], encValCont[i], encSlowSpeed[i], encFastSpeed[i]);
encoderArrayOld[i][1]=encoderArray[i][1];//May need to set to 0
encoderArrayOld[i][1]=encoderArray[i][1];

}
}



void encoderValChange(Encoder encoderValue, float &simDataRef, int &simDataRefInt, float lowVal, float hiVal, bool loopVlaues, float slowSpeed, float hiSpeed ){
//**************************************************************
//********   Check Encoder 
//**************************************************************
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.
  // (+ve clockwise, -ve anticlockwise, normally)
  short encoderPrevious = 0;
  short encoderClicks = (encoderValue.read() - encoderPrevious) / 4;


  // when encoder 'clicks' into a new detent:
  if (encoderClicks != 0) {

    // change in degrees from current stored value (can be <0 )
    float encoderChange = 0;

    // Threshold between FINE and FAST control: 30ms per click
    //Change this to get a different threshold time  Must change in 2 spots
    if (encoderClickInterval > 30) {
      // FINE/slow mode. Change by 0.5 degrees per click.
      encoderChange = encoderClicks * 0.5;


      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (encoderClickInterval <= 30 && encoderClickInterval > 1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      encoderChange = encoderClicks * 5.0;
    }

    // new value = current value plus changes
    float encNewValue = simDataRef + encoderChange;
    



    // make sure new value is valid (i.e. when moving across High Value and Low value
    while (encNewValue < lowVal) encNewValue += hiVal;
    while (encNewValue >= hiVal) encNewValue -= hiVal;

    // write validated new heading back to dataref
    simDataRef = encNewValue;
    simDataRefInt=(int)simDataRef;//used if flight sim varible is an int.  Truncates and rounds down
    Serial.print("Sim Float = ");//debugging to see if encoder values change
    Serial.print(simDataRef);
    Serial.print(",  Sim Int = ");
    Serial.print("Float = ");
    Serial.println(simDataRefInt);

    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    encoderPrevious = 0;
    HeadingBugEncoder.write(0);
    encoderClickInterval = 0;
  }
}