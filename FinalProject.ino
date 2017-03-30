#include <Servo.h>
#include <QSerial.h>
#define RD 6
#define LD 5
#define RS 7
#define LS 4
#define PUSH_PIN 9
#define BUTTON 13
#define LTL A3  //Linetracker Left -- Should be A3
#define LTC A5  //Linetracker Centre -- should be A5
#define LTR A4  //Linetracker Right -- should be A4
#define LTHRES 850  //
#define RTHRES 850  //
#define CTHRES 850  //

#define leftSpeed 100
#define rightSpeed 100

Servo pan, tilt, grip;
int leftVal;
int centreVal;
int rightVal;  
bool wallDetected = false;

//QSerial obj; //QSerial Object
void setup() {
  
  //Attach gripper motors
  pan.attach(8);
  tilt.attach(9);
  grip.attach(10);
  
  //center grip arm
  pan.write(90);
  
  /*attach IRSensor
  obj.attach(8,-1);*/
  
  pinMode(RD,OUTPUT);
  pinMode(LD,OUTPUT);
  pinMode(RS,OUTPUT);
  pinMode(LS,OUTPUT);
  pinMode(BUTTON,INPUT);
  pinMode(LTL, INPUT);
  pinMode(LTC, INPUT);
  pinMode(LTR, INPUT);

  Serial.begin(9600);
  
  delay(50);
  WaitButton();

}

void WaitButton(){
  while(digitalRead(13) == HIGH){
    delay(50);
    Serial.print(digitalRead(13));
    Serial.print("\n");
  }
  while(digitalRead(13) == LOW){
    Serial.print(digitalRead(13));
    Serial.print("\n");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Follow first line towards target
   lineFollow();
  //Check for proximity to wall
   detectWall();
  if(wallDetected){
    //Move forward until all three sensors see black
    while(!black3()){
      inch();
      analogWrite(RS,0);
      analogWrite(LS,0);
      delay(500);
    }
  }
  
  }

boolean black3(){
 leftVal = analogRead(LTL);
 centreVal = analogRead(LTC);
 rightVal = analogRead(LTR);   ////Read linetracker sensor values
 if(leftVal>LTHRES && rightVal>RTHRES && centreVal>CTHRES){
    return true;
 }
 else
    return false;
}
  
void detectWall(){
  Serial.print(analogRead(1));
  Serial.print("\n");
  if(analogRead(1) > 450){
    analogWrite(RS,0);
    analogWrite(LS,0);
    Serial.print("Wall Detected!\n");
    wallDetected = true;
    //return true;
  }
}

/*bool IRSensor() {
  int val = obj.receive(100); //Recieve with 100ms wait
  Serial.println(val);
  if(val == 48 || val == 49 || val == 50 || val == 51){
    return true;
  }else{
    return false;
  }
}*/
void inch(){
 digitalWrite(RD,HIGH);
 digitalWrite(LD,HIGH);
 analogWrite(RS,130);
 analogWrite(LS,130);
 delay(20);
}
void spin(){
 digitalWrite(RD,LOW);
 digitalWrite(LD,HIGH);
 analogWrite(RS,130);
 analogWrite(LS,130);
 delay(20);
}

void lineDetect(){
 leftVal = analogRead(LTL);
 centreVal = analogRead(LTC);
 rightVal = analogRead(LTR);   ////Read linetracker sensor values

   if(leftVal>LTHRES||rightVal>RTHRES||centreVal>CTHRES){
      analogWrite(RS,0);
      analogWrite(LS,0);
      lineFollow();
   }else{
    return;
   }
}

void lineCalibrate(){
  int leftVal = analogRead(LTL);
  int centreVal = analogRead(LTC);
  int rightVal = analogRead(LTR);
  /*Serial.print("Left Value: ");
  Serial.println(leftVal);
  Serial.print("Center Value: ");
  Serial.println(centreVal);
  Serial.print("Right Value: ");
  Serial.println(rightVal);*/
}
void lineFollow() {
  //Motors to drive straight
  digitalWrite(RD,HIGH);
  digitalWrite(LD,HIGH);

  //Read linetracker sensor values
  int leftVal = analogRead(LTL);
  int centreVal = analogRead(LTC);
  int rightVal = analogRead(LTR);
  /*Serial.print("Left Value: ");
  Serial.println(leftVal);
  Serial.print("Center Value: ");
  Serial.println(centreVal);
  Serial.print("Right Value: ");
  Serial.println(rightVal);*/
 //Line centred, drive straight
 if(leftVal<LTHRES && centreVal>CTHRES && rightVal<RTHRES){
 analogWrite(RS,130);
 analogWrite(LS,130);  //Please to use the lowest speed that can make the robot move!!!
 }
 
 //Veering right, move power to right motor
 else if(leftVal>LTHRES && centreVal<CTHRES && rightVal<RTHRES){
 analogWrite(RS,150);
 analogWrite(LS,130);
 }
 
 else if(leftVal>LTHRES && centreVal>CTHRES && rightVal<RTHRES){
 analogWrite(RS,150);
 analogWrite(LS,130);
 }
 
 //Veering left, move power to left motor
 else if(leftVal<LTHRES && centreVal<CTHRES && rightVal>RTHRES){
 analogWrite(RS,130);
 analogWrite(LS,150);
 }
 
 else if(leftVal<LTHRES && centreVal>CTHRES && rightVal>RTHRES){
 analogWrite(RS,130);
 analogWrite(LS,150);
 }
 
else if (leftVal<LTHRES && centreVal<CTHRES && rightVal<RTHRES){ //for other cases, stop driving first.
 analogWrite(RS,0);
 analogWrite(LS,0);
 delay(1000);    //wait a sec, to disdinguish between the going straight move and spin move.
 spin();
 lineDetect();
}

}

