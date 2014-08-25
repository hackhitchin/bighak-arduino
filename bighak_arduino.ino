//#include <PinChangeInt.h> // available at http://www.arduino.cc/playground/Main/PinChangeInt


const int FwdRCPin = 5; //radio control input pin 'elevator', used for forward/reverse
const int SteerRCPin = 6; //radio control input pin, 'aileron', used for steering
const int EnableRCPin = 7; //radio control input pin,'throttle' used to decide when to override the 
const int LaserRCPin = 4; //radio control input pin,, should be 'rudder'
const int LeftMotorOutPin = 3; //output to opamp to generate motor controller voltages
const int RightMotorOutPin =11; //output to opamp to generate motor controller voltages
const int LaserOutPin = 8; //output to relay to fire laser
const int ControllerEnablePin = 2; //output to relay for turning controller on  
const int MotorNeutral = 133; //pwm level for motors off
const int MotorMaxFWD = 194;  //185 equivalent to 3.62
const int MotorMaxREV = 79;   //80 equivalent to 1.56V
const int RCMax = 1550;  //absolute max: 2003; //max microseconds 
const int RCMin = 1350;  //absalute min: 986; //min microseconds 
const int RCEnable = 1700; //cut in level
const int RCDisable = 1200; //cut out level
// variables to keep track of RC signals. 'rising_times' are used to track when signal pusles started. '_Signal' variables used to store last received pulse length
unsigned long risingFWDTime = 0;
unsigned long risingSteeringTime = 0;
unsigned long risingEnableTime = 0;
unsigned long risingLaserTime = 0;
double FWDSignal=1500, SteeringSignal=1500, EnableSignal=1500, LaserSignal=1500;
int LeftMotorSignal, RightMotorSignal; //used to store desired motor speeds
float LeftMotorSpeed, RightMotorSpeed;

void setup() {
          
  pinMode(LeftMotorOutPin, OUTPUT);
  pinMode(RightMotorOutPin, OUTPUT);
  pinMode(LaserOutPin, OUTPUT);
  pinMode(ControllerEnablePin, OUTPUT);
  pinMode(FwdRCPin,INPUT);
  pinMode(SteerRCPin,INPUT);
  pinMode(EnableRCPin,INPUT);
  pinMode(LaserRCPin,INPUT);
  digitalWrite(ControllerEnablePin,LOW);
  analogWrite(LeftMotorOutPin,MotorNeutral);
  analogWrite(RightMotorOutPin,MotorNeutral);
  delay(3000);
  digitalWrite(ControllerEnablePin,HIGH);
  delay(1000);  
}

void loop(){
// channel fitlering. TODO: check for invalid, or outof range values and do something
  FWDSignal = pulseIn(FwdRCPin, HIGH, 25000); // Read the pulse width of 
  SteeringSignal = pulseIn(SteerRCPin, HIGH, 25000); // each channel
  FWDSignal = min(max(FWDSignal, RCMin), RCMax);
  SteeringSignal = min(max(SteeringSignal, RCMin), RCMax);

// todo: check for enable channel level
LeftMotorSignal = FWDSignal; // mixing: FWDSignal+SteeringSignal-3000; //RC signal midpoint assumed 1500, this makes motorspeed value +/-1000
RightMotorSignal = SteeringSignal; //mixing: FWDSignal-SteeringSignal-3000;

  LeftMotorSpeed = map(LeftMotorSignal, RCMin, RCMax, MotorMaxFWD, MotorMaxREV);
  RightMotorSpeed = map(RightMotorSignal, RCMin, RCMax, MotorMaxFWD, MotorMaxREV);

//debugging
  analogWrite(LeftMotorOutPin,LeftMotorSpeed);
  analogWrite(RightMotorOutPin,RightMotorSpeed);
  Serial.print("left = " );                       
  Serial.print(LeftMotorSignal);   
  Serial.print("right = " );                       
  Serial.println(RightMotorSignal);  
  delay(20);
}
