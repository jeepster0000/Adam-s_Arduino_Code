 
#include <Encoder.h> //Encoder library is needed for encoders
#include <elapsedMillis.h> //This is needed for the timing of the encoder

 
/*
*****************************************************************************************
*********************X-PLANE TEENSY CONTROLLER*******************************************																*
*This code is used for the TEENSY flight sim controller to "talk" to x-plane.  			*
*The encoder is speed sensitive.  If you turn the										*
*encoder quickly, the bug changes quickly and you have fast control; if					*
*you turn the encoder slowly, the bug changes slowly and you have fine					*
*control.  Encoders must be on a digital input.  Potieometers can be on an				*
*analong input.																			*
 *																						*
*This is an extension (and substantial rewrite) of the plain encoder-					*
*reading code from before.  Pots are added as well										*
*****************************************************************************************
*/


/*  **********************************************************
********************ENCODER*********************************
If encoder does not spin the correct way, then swap A & B pins
Copy and paste fron Nav 2 OBS.  This does not have the testing criteria

Do not delete the Encoder calls.  Just change to a port not being used or Note out.
 */
 
 
 //Encoder Pins  Change or assign pin numbers here  
 
	//Encoders are on pins 0-12 and 24-32
	//Pins 0-7 are coming.  Debounce has already been done
	//Encoders must be on digital pins
 
 const int ETrim_A =9;
 const int ETrim_B=8;
 const int ADF_A = 10;
 const int ADF_B =11;
 const int HdgSync_A =12;
 const int HdgSync_B = 24;
 const int Nav2OBS_A =25;
 const int Nav2OBS_B = 26;
 const int Hdg_A = 27;
 const int Hdg_B = 28;
 const int Alt_A = 29;
 const int Alt_B = 30;
 const int Nav1OBS_A = 31;
 const int Nav1OBS_B = 32;

 
 //Potieometer pins  These pins must be analog pins on the teensy.  Change or assign pin numbers here
		//pots are on pins 14-23.  They are addressed as A0-A9
		//If more pots are needed, they can be robbed from the switch pins
		//Linear potentiomters usually from 0-670+
		//The red wire is the wiper.  
		//Flaps potentiometers set on pin 23 = A9 on Teensy
		//Elevator Trim Pin set on pin 22 = A8 on Teensy
 
 char FlapsPin = A9; 
 char ElevatorTrimPin = A8;
 
 
 //Audo panel pins.  These pins are used for the audo panel. 
		//The audio is coming from the back of the board.  These are pins 40-57
		//Some pins have been broken.
 
 
 //Switch Pins or other Input / Outputs 
 
 
 //Varibles for Flap Pot
float CurrentFlapValue;
int RoundFlapValue;
float TestFlapValue;
int FlapDeBounceOld;
int FlapDeBounceNew;

//Varible for Elevator Trim Pot
float CurrentElevatorTrimValue;
int RoundElevatorTrimValue;
int ElevatorTrimDeBounceOld;
int ElevatorTrimDeBounceNew;

/* *****************ENCODER VARIBLES*****************************************************************************
*These are all the encoders and timer intervals assigned varibles in this section.								*
*the elapsedMillis command automaticlly increases as time ellapses.  											*
*This allows time to continue without stopping the program or using delay.  It is assigning a varible			*
*to a varible																									*	
*																												*	
*The encoder command assigns a varible as an encoder.  This uses the encoder.h library  						*
*****************************************************************************************************************/

Encoder Nav1OBSEncoder(31, 32);  //Nav 1 OBS Encoder
short Nav1OBSEncoderPrevious = 0;
elapsedMillis Nav1OBSEncoderClickInterval = 0;

Encoder Nav2OBSEncoder(25, 26);  // Nav 2 OBS Encoder
short Nav2OBSEncoderPrevious = 0;
elapsedMillis Nav2OBSEncoderClickInterval = 0; 

Encoder AltEncoder(29,30);  // Altimiter  Encoder
short AltEncoderPrevious = 0;
elapsedMillis AltEncoderClickInterval = 0; 

Encoder HeadingBugEncoder(27, 28); // Heading Bug Encoder
short HeadingBugEncoderPrevious = 0;
elapsedMillis HeadingBugEncoderClickInterval = 0; 

Encoder ADFCardEncoder(10, 11); // // ADF Cardinal 
short ADFCardEncoderPrevious = 0;
elapsedMillis ADFCardEncoderClickInterval = 0;

Encoder HIMagEncoder(24, 12); // Heading Indicator Sync Encoder 
short HIMagEncoderPrevious = 0;
elapsedMillis HIMagEncoderClickInterval = 0;

Encoder ElevatorTrimWheelEncoder(9, 8); //Elevator Trim Wheel Encoder
short ElevatorTrimPrevious = 0;
elapsedMillis ElevatorTrimClickInterval = 0;

// Input/Output with simulator
	//x-plane datarefs can be found at https://developer.x-plane.com/datarefs/
FlightSimFloat Nav1OBSDataref;			//nav 1 OBS
FlightSimFloat Nav2OBSDataref;			//nav 2 OBS
FlightSimFloat AltDataref;				//altimiter
FlightSimFloat HeadingBugDataref;		//heading bug
FlightSimFloat ADFCardDataref;			//ADF
FlightSimFloat FlapsDataRef;			//flaps position
FlightSimFloat HIMagDataref;			//heading indicator magnetic sync
FlightSimFloat ElevatorTrimDataref;		//elevator trim
FlightSimInteger Com1ListenDataRef;		//audio panel
FlightSimInteger TransmitComDataRef;	//audio panel
FlightSimInteger Com2ListenDataRef;		//audio panel
//FlightSimFloat Com1StanbyHzDataRef;	//com 1 standby	Hz
//FlightSimFloat Nav1StanbyHzDataRef;	//nav 1 Standby Hz

/************************************************************************************
*********************** TESTING SETUPS***********************************************
*************************************************************************************
	Do not erase,
	
	*** PUT UNUSED SETUPS HERE****
	
	*/
/* // Com 1 Hz Encoder TEST  24 an 12 is assigned to the Gyro sync mag encoder HIMagEncoder.
Encoder Com1StanbyHzEncoder(24, 12); //Com 1 Hz on 12 24
short Com1StanbyHzEncoderPrevious = 0;
elapsedMillis Com1StanbyHzEncoderClickInterval = 0;
*/	
/*
 
 //sim/cockpit2/radios/actuators/nav1_standby_frequency_khz
// Nav 1 Standby Hertz
Encoder Nav1StanbyHzEncoder(9, 8); // Nav 1 Standby on 8 9  8&9 are currently assigned to elevator trim
short Nav1StanbyHzEncoderPrevious = 0;
elapsedMillis Nav1StanbyHzEncoderClickInterval = 0;

*/

//Testing Values
//short TestValuePrev = 0;
//float TestDataRef=1000;
  
 
void setup() {
  Nav1OBSDataref = XPlaneRef("sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot");
  Nav2OBSDataref = XPlaneRef("sim/cockpit2/radios/actuators/nav2_obs_deg_mag_pilot");
  AltDataref = XPlaneRef("sim/cockpit/misc/barometer_setting");
  HeadingBugDataref = XPlaneRef("sim/cockpit2/autopilot/heading_dial_deg_mag_pilot");
  //Com1StanbyHzDataRef = XPlaneRef("sim/cockpit2/radios/actuators/com1_standby_frequency_khz");
  ADFCardDataref = XPlaneRef("sim/cockpit/radios/adf1_cardinal_dir");
  //Nav1StanbyHzDataRef = XPlaneRef("sim/cockpit2/radios/actuators/nav1_standby_frequency_khz");
  FlapsDataRef = XPlaneRef("sim/flightmodel/controls/flaprqst");
  HIMagDataref = XPlaneRef("sim/cockpit/gyros/dg_drift_vac_deg");
  ElevatorTrimDataref = XPlaneRef("sim/flightmodel/controls/elv_trim");


  //AudioPanel
   = XPlaneRef("sim/cockpit2/radios/actuators/audio_selection_com1");
  //To transmit over com1 set this dataref to 6, to transmit to com2 set to 7
  TransmitComDataRef = XPlaneRef("sim/cockpit/switches/audio_panel_out");

  //Linear Pots need the current position noted when the program boots up  
  //RoundFlapValue = (analogRead(FlapsPin) * 10)/100;
  //CurrentFlapValue = RoundFlapValue/100.;
  //FlapsDataRef = CurrentFlapValue;
  FlapDeBounceOld=FlapDeBounceNew +1;
  TestFlapValue = FlapsDataRef;
  ElevatorTrimDeBounceOld =ElevatorTrimDeBounceNew+1;

  //Use serial port for testing only
  Serial.begin(9600);
 
  FlightSim.update();

}
 
 
void loop() {
//***
//Flaps code 
//***
//This uses a 10K linear Pot.  The postiion of the Pot is important to the flap value  
  
  if (analogRead(FlapsPin >0.01)){
  RoundFlapValue = (analogRead(FlapsPin)*500)/510;
  FlapDeBounceNew =round(analogRead(FlapsPin)*10/100);
  CurrentFlapValue = RoundFlapValue/1000.;
  
  if (FlapDeBounceNew != FlapDeBounceOld){
    FlapsDataRef = CurrentFlapValue;
    FlapDeBounceOld=FlapDeBounceNew;
    
  }
  } else {FlapsDataRef = 0.0000;}

//Trim wheel, testing mode.
//The trim wheel uses a 500K potentiometer.  
//Elevator Trim wheel values are between -1 & 1
//serial.println(A8);

//analogRead (ElevatorTrimPin);
//float CurrentElevatorTrimValue;
//int RoundElevatorTrimValue;
//int ElevatorTrimDeBounceOld;
//int ElevatorTrimDeBounceNew;

RoundElevatorTrimValue = (analogRead(ElevatorTrimPin)*500)/512;
ElevatorTrimDeBounceNew=round(analogRead(ElevatorTrimPin)*10/100);
CurrentElevatorTrimValue = (RoundElevatorTrimValue /1000.-.5)*2;
if (ElevatorTrimDeBounceNew != ElevatorTrimDeBounceOld){
  ElevatorTrimDataref = CurrentElevatorTrimValue;
  ElevatorTrimDeBounceOld = ElevatorTrimDeBounceNew;
  Serial.println(CurrentElevatorTrimValue);
}



  

  
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
 //Nav 2 OBS   ********************USE THIS TO COPY AND PASTE ***********************************************************************************
 //***
  
  // (+ve clockwise, -ve anticlockwise, normally)
  short Nav2OBSClicks = (Nav2OBSEncoder.read() - Nav2OBSEncoderPrevious) / 4;  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  
  // when encoder 'clicks' into a new detent:
  if (Nav2OBSClicks != 0) {
    
    float Nav2OBSChange = 0; 	// change in degrees from current stored value (can be <0 )
 
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
 
    float Nav2OBSNewValue = Nav2OBSDataref + Nav2OBSChange;   // new value = current value plus changes
    
    while (Nav2OBSNewValue  <   0.0) Nav2OBSNewValue += 360.0;  // make sure new value is valid (i.e. when moving across 0deg/360deg)
    while (Nav2OBSNewValue >= 360.0) Nav2OBSNewValue -= 360.0;
 
    Nav2OBSDataref = Nav2OBSNewValue;  // write validated new heading back to dataref
    Nav2OBSEncoderPrevious = 0;  // reset encoder state and interval timer
    Nav2OBSEncoder.write(0);  	  // (only when encoder has 'clicked' into a new detent!)
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
 
    // new value = current value plus changes
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
    
  }

  /*
  
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
*/
/*  ***************COMMENTED OUT FOR NOT BEING USED**********THIS CAN BE TURNED BACK ON************************
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
*/


/*
//***
 //Elevator Trim  
 //Use this section if using an encoder on trim wheel.
 //Do not use this section if Trim Wheel is on a POT
 //***

 //sim/flightmodel/controls/elv_trim
//Elevator Trim Wheel Encoder
//Encoder ElevatorTrimWheelEncoder(9, 8); // Elevator trim on 8&9
//short ElevatorTrimPrevious = 0;
//elapsedMillis ElevatorTrimClickInterval = 0;
//ElevatorTrimDataref

  // divide by 4 to find how many clicks the encoder's been turned  This value should be changed based on the encoder type.  
  // (+ve clockwise, -ve anticlockwise, normally)
  short ElevatorTrimClicks = (ElevatorTrimWheelEncoder.read() - ElevatorTrimPrevious) / 4;
  //short TestClicks = (ElevatorTrimWheelEncoder.read()-TestValuePrev)/4;//For Testing only 
 
  // when encoder 'clicks' into a new detent:
  if (ElevatorTrimClicks != 0) {
      //if (TestClicks !=0){//For Testing only 
    
    
    // change in degrees from current stored value (can be <0 )
    float ElevatorTrimChange = 0;
     //float TestChange=0;//For Testing only 
 
    // Threshold between FINE and FAST control: 30ms per click  
    //Change this to get a different threshold time  Must change in 2 spots
    if (ElevatorTrimClickInterval > 30) {
      // FINE/slow mode. Change by 0.01  per click.
      ElevatorTrimChange = ElevatorTrimClicks * 0.01;
      
       //TestChange=TestClicks *.05;//For Testing only 
       //Serial.print("The encoder moved slow and incremented  ");//For Testing only 
       //Serial.println(TestChange);//For Testing only 

      
      //This changes if the elapsed time between clicks is less than 30ms but greater than 1 milisecond.  
      //The 1ms is used to process out noise that happens between clicks and prevent unwanted "bounce"
    } else if (ElevatorTrimClickInterval <=30 && ElevatorTrimClickInterval >1) {
      // FAST/coarse mode. Change by 5 degrees per click.
      ElevatorTrimChange = ElevatorTrimClicks * .050;
      
      //TestChange=TestClicks *5.0;//For Testing only 
       //Serial.print("The encoder moved fast and incremented  ");//For Testing only 
       //Serial.println(TestChange);//For Testing only 
    }
 
    // new value = current value plus changes
    float ElevatorTrimNewValue = ElevatorTrimDataref + ElevatorTrimChange;
     //float TestNewValue =TestDataRef+ TestChange;//For Testing only 
  
    // make sure new value is valid Trim value has to be inbetween -1 & 1
    while (ElevatorTrimNewValue  <   -1) ElevatorTrimNewValue = -1;
    while (ElevatorTrimNewValue > 1) ElevatorTrimNewValue = 1;
 
    // write validated new heading back to dataref
    ElevatorTrimDataref = ElevatorTrimNewValue;
    //TestDataRef = TestNewValue;//For Testing only 
 
    // reset encoder state and interval timer
    // (only when encoder has 'clicked' into a new detent!)
    ElevatorTrimPrevious = 0;
    ElevatorTrimWheelEncoder.write(0);
    ElevatorTrimClickInterval = 0;
    
   //Serial.print("The New Value is  ");//For Testing only 
    //Serial.println(TestDataRef);//For Testing only   
  }

*/
   FlightSim.update();
    
 
    
}
