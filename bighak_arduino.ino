const int LeftRCPin = 5; //radio control input pin 'elevator', used for forward/reverse
const int RightRCPin = 6; //radio control input pin, 'aileron', used for steering
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
double LeftSignal=1500, RightSignal=1500, EnableSignal=1500, LaserSignal=1500;
float LeftMotorSpeed, RightMotorSpeed;

void setup() {
          
  pinMode(LeftMotorOutPin, OUTPUT);
  pinMode(RightMotorOutPin, OUTPUT);
  pinMode(LaserOutPin, OUTPUT);
  pinMode(ControllerEnablePin, OUTPUT);
  pinMode(LeftRCPin,INPUT);
  pinMode(RightRCPin,INPUT);
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
  EnableSignal = pulseIn(EnableRCPin, HIGH, 25000);
  if (EnableSignal > 0){
    
    LeftSignal = pulseIn(LeftRCPin, HIGH, 25000); // Read the pulse width of 
    RightSignal = pulseIn(RightRCPin, HIGH, 25000); // each channel
    // constrain signals
    
    LeftSignal = constrain(LeftSignal, RCMin, RCMax);
    RightSignal = constrain(RightSignal, RCMin, RCMax);
  
    LeftMotorSpeed = map(LeftSignal, RCMin, RCMax, MotorMaxFWD, MotorMaxREV);
    RightMotorSpeed = map(RightSignal, RCMin, RCMax, MotorMaxFWD, MotorMaxREV);
  } else {
    // Enable Signal Pulse timed out - we have to assume RC has gone away
    // set everything to neutral
    LeftMotorSpeed = MotorNeutral;
    RightMotorSpeed = MotorNeutral;
   Serial.println("**** Enable Signal lost****");
  }

//debugging
  analogWrite(LeftMotorOutPin,LeftMotorSpeed);
  analogWrite(RightMotorOutPin,RightMotorSpeed);
  Serial.print("left = " );                       
  Serial.print(LeftMotorSignal);   
  Serial.print("right = " );                       
  Serial.println(RightMotorSignal);  
  delay(20);
}
