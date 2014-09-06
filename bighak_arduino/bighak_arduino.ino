// Pins
const int RC_LEFT_PIN = 5; //radio control input pin 'elevator', used for forward/reverse
const int RC_RIGHT_PIN = 6; //radio control input pin, 'aileron', used for steering
const int RC_ENABLE_WIDTH_PIN = 7; //radio control input pin,'throttle' used to decide when to override the 
const int RC_LASER_PIN = 4; //radio control input pin,, should be 'rudder'
const int RC_SWITCH_PIN = 12; //needs comfirming, this is the lien connected to the physical toggle switch for RC

const int MOTOR_LEFT_OUT_PIN = 3; //output to opamp to generate motor controller voltages
const int MOTOR_RIGHT_OUT_PIN =11; //output to opamp to generate motor controller voltages
const int CONTROLLER_ENABLE_PIN = 2; //output to relay for turning controller on

const int LASER_OUT_PIN = 8; //output to relay to fire laser

// PWM levels
const int MotorNeutral = 133; //pwm level for motors off
const int MotorMaxFWD = 194;  //185 equivalent to 3.62
const int MotorMaxREV = 79;   //80 equivalent to 1.56V

// RC Pulsewidths
const int RC_MAX_WIDTH = 1550;  //absolute max: 2003; //max microseconds 
const int RC_MIN_WIDTH = 1350;  //absalute min: 986; //min microseconds 
const int RC_ENABLE_WIDTH = 1700; //cut in level
const int RC_DISABLE_WIDTH = 1200; //cut out level
const int RC_COMMS_TIMEOUT = 2000; // Time since last command

// Direction constants
const char FWD = 'F';
const char BACK = 'B';
const char RIGHT = 'R';
const char LEFT = 'L';
const char STOP = 'S';
const char PEW = 'Z';
const char PEW_STOP = 'z';

// Debug flag
const bool DEBUG = false;

bool LaserOn, NoCommand, SerialOn;
float LastCommand;
char Command;

double LeftSignal=1500, RightSignal=1500, EnableSignal=1500, LaserSignal=1500;
float LeftMotorSpeed, RightMotorSpeed;


void setup() {
  Serial.begin(9600); 

  pinMode(MOTOR_LEFT_OUT_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_OUT_PIN, OUTPUT);
  pinMode(LASER_OUT_PIN, OUTPUT);
  pinMode(CONTROLLER_ENABLE_PIN, OUTPUT);
  pinMode(RC_LEFT_PIN,INPUT);
  pinMode(RC_RIGHT_PIN,INPUT);
  pinMode(RC_ENABLE_WIDTH_PIN,INPUT);
  pinMode(RC_LASER_PIN,INPUT);
  pinMode(RC_SWITCH_PIN,INPUT_PULLUP);
  digitalWrite(CONTROLLER_ENABLE_PIN,LOW);
  analogWrite(MOTOR_LEFT_OUT_PIN,MotorNeutral);
  analogWrite(MOTOR_RIGHT_OUT_PIN,MotorNeutral);
  delay(3000);
  digitalWrite(CONTROLLER_ENABLE_PIN,HIGH);
  delay(1000);  
}

void loop(){
  // channel filtering. 
  EnableSignal = pulseIn(RC_ENABLE_WIDTH_PIN, HIGH, 25000);

  if (EnableSignal > RC_ENABLE_WIDTH){
    if (!digitalRead(RC_SWITCH_PIN)){  //switch pulls input low
      LeftSignal = pulseIn(RC_LEFT_PIN, HIGH, 25000); // Read the pulse width of 
      RightSignal = pulseIn(RC_RIGHT_PIN, HIGH, 25000); // each channel
      // constrain signals
      LaserSignal =pulseIn(RC_LASER_PIN, HIGH, 25000); // each channel
      LeftSignal = constrain(LeftSignal, RC_MIN_WIDTH, RC_MAX_WIDTH);
      RightSignal = constrain(RightSignal, RC_MIN_WIDTH, RC_MAX_WIDTH);
      if (LaserSignal > 1500){
        LaserOn=true;
      } 
      else {
        LaserOn=false;
      }
      LeftMotorSpeed = map(LeftSignal, RC_MIN_WIDTH, RC_MAX_WIDTH, MotorMaxFWD, MotorMaxREV);
      RightMotorSpeed = map(RightSignal, RC_MIN_WIDTH, RC_MAX_WIDTH, MotorMaxFWD, MotorMaxREV);
    } 
    else {
      //get next character
      Command = ' '; //set command to soemthing incase it doesn't get set later
      if (Serial.available() > 0) { //check if a command has been received
        NoCommand=false; //we've received soemthing, reset the flag!
        while(Serial.available()){ //whileever there's something in the buffer
          Command = Serial.read(); //read it. this effectively clears the buffer, ensuring the arduino keeps up
        }
      } 
      else { //if nothing is availble, then nothing's been sent
        if (NoCommand==false){  //if nocommand==false then this is the first break in recent times
          NoCommand=true;  //set the flag
          LastCommand=millis();  //record the time we first got a 'timeout'
        } 
        else {
          if((millis()-LastCommand) > RC_COMMS_TIMEOUT){  
          //if its been a while - turn everything off
            LaserOn=false;
            LeftMotorSpeed = MotorNeutral;
            RightMotorSpeed = MotorNeutral;
          }
        }
      }

      //set outputs correctly
      switch (Command) { // grab first character only, as ther may have been multiple chars in buffer
        case FWD:        //F=forward
          LaserOn=false;
          LeftMotorSpeed = MotorMaxFWD;
          RightMotorSpeed = MotorMaxFWD;
          break;
        case BACK:        //B=backward
          LaserOn=false;
          LeftMotorSpeed = MotorMaxREV;
          RightMotorSpeed = MotorMaxREV;
          break;
        case STOP:        //S=stop
          LaserOn=false;
          LeftMotorSpeed = MotorNeutral;
          RightMotorSpeed = MotorNeutral;
          break;
        case LEFT:        //L=rotate left
          LaserOn=false;
          LeftMotorSpeed = MotorMaxREV;
          RightMotorSpeed = MotorMaxFWD;
          break;
        case RIGHT:         //R=rotate right
          LaserOn=false;
          LeftMotorSpeed = MotorMaxFWD;
          RightMotorSpeed = MotorMaxREV;
          break;
        case PEW:        //Z=fire laser
          LaserOn=true;
          LeftMotorSpeed = MotorNeutral;
          RightMotorSpeed = MotorNeutral;
          break;
        case PEW_STOP:        //z=turn laser off
          LaserOn=false;
          LeftMotorSpeed = MotorNeutral;
          RightMotorSpeed = MotorNeutral;
          break;
      }
    }
  }
  else {
    // Enable Signal Pulse timed out or low - we have to assume RC has gone away
    // set everything to neutral
    LaserOn=false;
    LeftMotorSpeed = MotorNeutral;
    RightMotorSpeed = MotorNeutral;
    if (DEBUG == true){
      Serial.println("**** Enable Signal lost****");
    }
  }

  // motor control
  analogWrite(MOTOR_LEFT_OUT_PIN,LeftMotorSpeed);
  analogWrite(MOTOR_RIGHT_OUT_PIN,RightMotorSpeed);

  // laser control
  digitalWrite(LASER_OUT_PIN, !LaserOn);
  
  //debugging
  if (DEBUG == true){

    if (!digitalRead(RC_SWITCH_PIN)){
      Serial.print("RC on, " );
    } 
    else {
      Serial.print("RC off, " );
    }
    Serial.print("enable = " ); 
    Serial.print(EnableSignal);                       
    Serial.print(" command = " );                       
    Serial.print(Command);
    Serial.print(" left = " );                       
    Serial.print(LeftSignal);   
    Serial.print(" right = " );                       
    Serial.println(RightSignal);  
  }
}


