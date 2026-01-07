 
#include <Encoder.h>
#include <elapsedMillis.h>

 
/*
This code links a rotary encoder to the NAV1 OBS. If you turn the
encoder quickly, the bug changes quickly and you have fast control; if
you turn the encoder slowly, the bug changes slowly and you have fine
control.
 
This is an extension (and substantial rewrite) of the plain encoder-
reading code from before.
*/


/*  **********************************************************
********************Notes*********************************
If incoder does not spin the correct way, then reverse the encoder.
Copy and paste fron Nav 2 OBS.  This does not have the testing criteria
 */
 
// These are al the encoders and timer intervals in this section.
//Nav 1 OBS Encoder
Encoder Nav1OBSEncoder(31, 32); // nav1OBS on 11 12
short Nav1OBSEncoderPrevious = 0;
elapsedMillis Nav1OBSEncoderClickInterval = 0;

// Nav 2 OBS Encoder
Encoder Nav2OBSEncoder(25, 26); // nav2OBS on 25 26 
short Nav2OBSEncoderPrevious = 0;
elapsedMillis Nav2OBSEncoderClickInterval = 0; 

// Altimiter  Encoder
Encoder AltEncoder(29,30); // Altimiter / baramoter on 8 7 
short AltEncoderPrevious = 0;
elapsedMillis AltEncoderClickInterval = 0; 

// Heading Bug Encoder
Encoder HeadingBugEncoder(27, 28); // Heading Bug on 5 6 
short HeadingBugEncoderPrevious = 0;
elapsedMillis HeadingBugEncoderClickInterval = 0; 

// Heading Indicator Sync Encoder
Encoder Com1StanbyHzEncoder(24, 12); //Com 1 Hz on 12 24
short Com1StanbyHzEncoderPrevious = 0;
elapsedMillis Com1StanbyHzEncoderClickInterval = 0;

//  sim/cockpit/radios/adf1_cardinal_dir
// ADF Cardinal
Encoder ADFCardEncoder(10, 11); // ADF Heading Indicator on 10 11
short ADFCardEncoderPrevious = 0;
elapsedMillis ADFCardEncoderClickInterval = 0;

//sim/cockpit2/radios/actuators/nav1_standby_frequency_khz
// Nav 1 Standby Hertz
Encoder Nav1StanbyHzEncoder(9, 8); // Nav 1 Standby on 8 9
short Nav1StanbyHzEncoderPrevious = 0;
elapsedMillis Nav1StanbyHzEncoderClickInterval = 0;

//sim/cockpit/gyros/dg_drift_vac_deg
// Heading Indicator Sync Encoder
Encoder HIMagEncoder(6, 7); // HI sync on 6 and 7 (not assigned)
short HIMagEncoderPrevious = 0;
elapsedMillis HIMagEncoderClickInterval = 0;

//Linear potentiomters usually from 0-670+
//Flaps potentiometers set on pin 23
char FlapsPin = A9;



// Input/Output with simulator
FlightSimFloat Nav1OBSDataref;
FlightSimFloat Nav2OBSDataref;
FlightSimFloat AltDataref;
FlightSimFloat HeadingBugDataref;
FlightSimFloat Com1StanbyHzDataRef;
FlightSimFloat ADFCardDataref;
FlightSimFloat Nav1StanbyHzDataRef;
FlightSimFloat FlapsDataRef;
//FlightSimFloat HIMagDataref;


//Varibles for Flap Pot
float CurrentFlapValue;
int RoundFlapValue;
float TestFlapValue;
int FlapDeBounceOld;
int FlapDeBounceNew;

//Testing Values
//short TestValuePrev = 0;
//float TestDataRef=1000;  
 
void setup() {
  Nav1OBSDataref = XPlaneRef("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot");
  Nav2OBSDataref = XPlaneRef("sim/cockpit2/radios/actuators/nav2_obs_deg_mag_pilot");
  AltDataref = XPlaneRef("sim/cockpit/misc/barometer_setting");
  HeadingBugDataref = XPlaneRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");
  Com1StanbyHzDataRef = XPlaneRef("sim/cockpit2/radios/actuators/com1_standby_frequency_khz");
  ADFCardDataref = XPlaneRef("sim/cockpit/radios/adf1_cardinal_dir");
  Nav1StanbyHzDataRef = XPlaneRef("sim/cockpit2/radios/actuators/nav1_standby_frequency_khz");
  FlapsDataRef = XPlaneRef("sim/flightmodel/controls/flaprqst");
  //HIMagDataref = XPlaneRef("sim/cockpit/gyros/dg_drift_vac_deg");
  
  //Linear Pots need the current position noted when the program boots up  
  //RoundFlapValue = (analogRead(FlapsPin) * 10)/100;
  //CurrentFlapValue = RoundFlapValue/100.;
  //FlapsDataRef = CurrentFlapValue;
  FlapDeBounceOld=FlapDeBounceNew +1;
  TestFlapValue = FlapsDataRef;
  
 
  FlightSim.update();

}
 
 
void loop() {
//***
//Flaps code testing
//***
//This uses a 10K linear Pot.  The postiion of the Pot is important to the flap value  
  
  if (analogRead(FlapsPin >0.001)){
  RoundFlapValue = (analogRead(FlapsPin)*500)/510;
  FlapDeBounceNew =round(analogRead(FlapsPin)*10/100);
  CurrentFlapValue = RoundFlapValue/1000.;
  
  if (FlapDeBounceNew != FlapDeBounceOld){
    FlapsDataRef = CurrentFlapValue;
    FlapDeBounceOld=FlapDeBounceNew;
  }
  } else {FlapsDataRef = 0.0000;}

  
 //***
 //Nav 1 OBS 
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short Nav1OBSClicks = (Nav1OBSEncoder.read() - Nav1OBSEncoderPrevious) / 4;
  //short TestClicks = (Nav1OBSEncoder.read()-TestValuePrev)/4;//For Testing only 
 
  // when encoder 'clicks' into a new detent:
  if (Nav1OBSClicks != 0) {
      //if (TestClicks !=0){//For Testing only 
    
    
    // change in degrees from current stored value (can be <0 )
    float Nav1OBSChange = 0;
     //float TestChange=0;//For Testing only 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (Nav1OBSEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 0.5 degrees per click.
      Nav1OBSChange = Nav1OBSClicks * 0.5;
      
       //TestChange=TestClicks *.5;//For Testing only 
       //Serial.print("The encoder moved slow and incremented  ");//For Testing only 
       //Serial.println(TestChange);//For Testing only 

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (Nav1OBSEncoderClickInterval <=30 && Nav1OBSEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      Nav1OBSChange = Nav1OBSClicks * 5.0;
      
      //TestChange=TestClicks *5.0;//For Testing only 
       //Serial.print("The encoder moved fast and incremented  ");//For Testing only 
       //Serial.println(TestChange);//For Testing only 
    }
 
    // new value = current value plus changes
    float Nav1OBSNewValue = Nav1OBSDataref + Nav1OBSChange;
     //float TestNewValue =TestDataRef+ TestChange;//For Testing only 
  
    // make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (Nav1OBSNewValue  <   0.0) Nav1OBSNewValue += 360.0;
    while (Nav1OBSNewValue >= 360.0) Nav1OBSNewValue -= 360.0;
 
    // write validated new heading back to dataref
    Nav1OBSDataref = Nav1OBSNewValue;
    //TestDataRef = TestNewValue;//For Testing only 
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    Nav1OBSEncoderPrevious = 0;
    Nav1OBSEncoder.write(0);
    Nav1OBSEncoderClickInterval = 0;
    
   //Serial.print("The New Value is  ");//For Testing only 
    //Serial.println(TestDataRef);//For Testing only   
  }

  //***
 //Nav 2 OBS 
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short Nav2OBSClicks = (Nav2OBSEncoder.read() - Nav2OBSEncoderPrevious) / 4;
  
 
  // when encoder 'clicks' into a new detent:
  if (Nav2OBSClicks != 0) {
    
    // change in degrees from current stored value (can be <0 )
    float Nav2OBSChange = 0; 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (Nav2OBSEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 0.5 degrees per click.
      Nav2OBSChange = Nav2OBSClicks * 0.5;

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (Nav2OBSEncoderClickInterval <=30 && Nav2OBSEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      Nav2OBSChange = Nav2OBSClicks * 5.0;
      
    }
 
    // new value = current value plus changes
    float Nav2OBSNewValue = Nav2OBSDataref + Nav2OBSChange;
  
    // make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (Nav2OBSNewValue  <   0.0) Nav2OBSNewValue += 360.0;
    while (Nav2OBSNewValue >= 360.0) Nav2OBSNewValue -= 360.0;
 
    // write validated new heading back to dataref
    Nav2OBSDataref = Nav2OBSNewValue;
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    Nav2OBSEncoderPrevious = 0;
    Nav2OBSEncoder.write(0);
    Nav2OBSEncoderClickInterval = 0;
    
  }

  //***
 //Altimiter Dial
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short AltClicks = (AltEncoder.read() - AltEncoderPrevious) / 4;
  
 
  // when encoder 'clicks' into a new detent:
  if (AltClicks != 0) {
   
    // change in degrees from current stored value (can be <0 )
    float AltChange = 0; 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    //if (AltEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 0.5 degrees per click.
      AltChange = AltClicks * 0.01;

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
   /* } else if (AltEncoderClickInterval <=30 && AltEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      AltChange = AltClicks * 0.1;
      
    }*/
 
    // new value = current value plus changes
    float AltNewValue = AltDataref + AltChange;
  
    // make sure new value is valid (i.e. the baromter / altimiter cannot be <29.0 or >30.8)
    while (AltNewValue  <   29.00) AltNewValue = 29.00;
    while (AltNewValue > 30.80) AltNewValue = 30.80;
 
    // write validated new heading back to dataref
    AltDataref = AltNewValue;

    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    AltEncoderPrevious = 0;
    AltEncoder.write(0);
    AltEncoderClickInterval = 0;   
  }

    //***
 //Heading Bug
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short HeadingBugClicks = (HeadingBugEncoder.read() - HeadingBugEncoderPrevious) / 4;
  
 
  // when encoder 'clicks' into a new detent:
  if (HeadingBugClicks != 0) {
    
    // change in degrees from current stored value (can be <0 )
    float HeadingBugChange = 0; 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (HeadingBugEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 0.5 degrees per click.
      HeadingBugChange = HeadingBugClicks * 0.5;

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (HeadingBugEncoderClickInterval <=30 && HeadingBugEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      HeadingBugChange = HeadingBugClicks * 5.0;
      
    }
 
    // new value = current value plus changes
    float HeadingBugNewValue = HeadingBugDataref + HeadingBugChange;
    
    
    
    // make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (HeadingBugNewValue  <   0.0) HeadingBugNewValue += 360.0;
    while (HeadingBugNewValue >= 360.0) HeadingBugNewValue -= 360.0;
 
    // write validated new heading back to dataref
    HeadingBugDataref = HeadingBugNewValue;
  
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    HeadingBugEncoderPrevious = 0;
    HeadingBugEncoder.write(0);
    HeadingBugEncoderClickInterval = 0; 
    
  }
//***
 //ADF Heading Indicator / Cardinal Indicator 
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short ADFCardClicks = (ADFCardEncoder.read() - ADFCardEncoderPrevious) / 4;
  
 
  // when encoder 'clicks' into a new detent:
  if (ADFCardClicks != 0) {
    
    // change in degrees from current stored value (can be <0 )
    float ADFCardChange = 0; 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (ADFCardEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 0.5 degrees per click.
      ADFCardChange = ADFCardClicks * 0.5;

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (ADFCardEncoderClickInterval <=30 && ADFCardEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      ADFCardChange = ADFCardClicks * 5.0;
      
    }
 
    // new value = current value plus changes
    float ADFCardNewValue = ADFCardDataref + ADFCardChange;
  
    // make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (ADFCardNewValue  <   0.0) ADFCardNewValue += 360.0;
    while (ADFCardNewValue >= 360.0) ADFCardNewValue -= 360.0;
 
    // write validated new heading back to dataref
    ADFCardDataref = ADFCardNewValue;
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    ADFCardEncoderPrevious = 0;
    ADFCardEncoder.write(0);
    ADFCardEncoderClickInterval = 0;
    
  }

 
  
  //***
 //Com 1 Hz Test
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short Com1StanbyHzClicks = (Com1StanbyHzEncoder.read() - Com1StanbyHzEncoderPrevious) / 4;
  
 
  // when encoder 'clicks' into a new detent:
  if (Com1StanbyHzClicks != 0) {
    
    // change in degrees from current stored value (can be <0 )
    float Com1StanbyHzChange = 0; 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (Com1StanbyHzEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 5 degrees per click.
      Com1StanbyHzChange = Com1StanbyHzClicks * 5;

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (Com1StanbyHzEncoderClickInterval <=30 && Com1StanbyHzEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      Com1StanbyHzChange = Com1StanbyHzClicks * 50.0;
      
    }
 
    // new value = current value plus changes
    float Com1StanbyHzNewValue = Com1StanbyHzDataRef + Com1StanbyHzChange;
  
    //// make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (Com1StanbyHzNewValue  <   0) Com1StanbyHzNewValue += 1000;
    while (Com1StanbyHzNewValue >= 1000) Com1StanbyHzNewValue -= 1000;
 
    // write validated new heading back to dataref
    Com1StanbyHzDataRef = Com1StanbyHzNewValue;
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    Com1StanbyHzEncoderPrevious = 0;
    Com1StanbyHzEncoder.write(0);
    Com1StanbyHzEncoderClickInterval = 0;
    
  }

//***
 //Nav 1 Hz 
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short Nav1StanbyHzClicks = (Nav1StanbyHzEncoder.read() - Nav1StanbyHzEncoderPrevious) / 4;
  
 
  // when encoder 'clicks' into a new detent:
  if (Nav1StanbyHzClicks != 0) {
    
    // change in degrees from current stored value (can be <0 )
    float Nav1StanbyHzChange = 0; 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (Nav1StanbyHzEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 5 degrees per click.
      Nav1StanbyHzChange = Nav1StanbyHzClicks * 5;

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (Nav1StanbyHzEncoderClickInterval <=30 && Nav1StanbyHzEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      Nav1StanbyHzChange = Nav1StanbyHzClicks * 20.0;
      
    }
 
    // new value = current value plus changes
    float Nav1StanbyHzNewValue = Nav1StanbyHzDataRef + Nav1StanbyHzChange;
  
    //// make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (Nav1StanbyHzNewValue  <   0) Nav1StanbyHzNewValue += 100;
    while (Nav1StanbyHzNewValue >= 100) Nav1StanbyHzNewValue -= 100;
 
    // write validated new heading back to dataref
    Nav1StanbyHzDataRef = Nav1StanbyHzNewValue;
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    Nav1StanbyHzEncoderPrevious = 0;
    Nav1StanbyHzEncoder.write(0);
    Nav1StanbyHzEncoderClickInterval = 0;
  }

  //***
 //HI Mag
 //***
  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short HIMagClicks = (HIMagEncoder.read() - HIMagEncoderPrevious) / 4;
  
 
  // when encoder 'clicks' into a new detent:
  if (HIMagClicks != 0) {
    
    // change in degrees from current stored value (can be <0 )
    float HIMagChange = 0; 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (HIMagEncoderClickInterval > 30) {
      // FINE/slow mode. Change by 0.5 degrees per click.
      HIMagChange = HIMagClicks * 0.5;

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (HIMagEncoderClickInterval <=30 && HIMagEncoderClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      HIMagChange = HIMagClicks * 5.0;
      
    }
 
   /* // new value = current value plus changes
    float HIMagNewValue = HIMagDataref + HIMagChange;
  
    // make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (HIMagNewValue  <   0.0) HIMagNewValue += 360.0;
    while (HIMagNewValue >= 360.0) HIMagNewValue -= 360.0;
 
    // write validated new heading back to dataref
    HIMagDataref = HIMagNewValue;
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    HIMagEncoderPrevious = 0;
    HIMagEncoder.write(0);
    HIMagEncoderClickInterval = 0;
    
*/    
  }

    FlightSim.update();
    
 
    
}
