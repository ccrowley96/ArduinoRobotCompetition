#include <EEPROM.h>
#include <Servo.h>
#include <QSerial.h>
//IR sensor in pin 8 - digital
//Defining motor pins
#define RD 7
#define LD 4
#define RS 6
#define LS 5

//Button
#define PUSH_PIN 9
#define BUTTON 13

//Linetracker 
#define LTL A3  //Linetracker Left -- Should be A3
#define LTC A5  //Linetracker Centre -- should be A5
#define LTR A4  //Linetracker Right -- should be A4

//Light Thresholds
#define LTHRES 850  
#define RTHRES 850  
#define CTHRES 850  

#define leftSpeed 131
#define leftSpeedFast 151
#define rightSpeed 130
#define rightSpeedFast 150



//Motor Encoder pins
#define LM_ENC 11
#define RM_ENC 12 

//Set up servos
Servo pan, tilt, grip;
//Function Declarations
void prepArm();
void pickUpBall();
void carryBall();
void tripleSpin();
void backUpForPickUp();
void backUp();
void turnAround();
void findBasketLeft();
void findBasketRight();
void dropItLikeItsHot();
//Variable Declarations
int leftVal;
int centreVal;
int rightVal;  
int gripVal;
bool wallDetected = false;
byte left_speed,right_speed;
//Encoder vals
int lm_cnt = 0;
int lm_change = 0;
int rm_cnt = 0;
int rm_change = 0;
//Line counter variables
int lineCount = 0;
boolean wasLight = false;
//Run counter variable
int runCount = 0;

//enum for stored last direction
enum lastDir {toRight,toLeft,center};
lastDir dirChecker = center;

void setup() {
  
  //Attach gripper motors
  pan.attach(8);
  tilt.attach(9);
  grip.attach(10);
 
  pinMode(RD,OUTPUT);
  pinMode(LD,OUTPUT);
  pinMode(RS,OUTPUT);
  pinMode(LS,OUTPUT);
  pinMode(BUTTON,INPUT);
  pinMode(LTL, INPUT);
  pinMode(LTC, INPUT);
  pinMode(LTR, INPUT);

  //Serial.begin(9600);
  prepArm();
  delay(50);
  WaitButton();
  tripleSpin();
}

void WaitButton(){
  while(digitalRead(13) == HIGH){
    delay(50);
  }
  while(digitalRead(13) == LOW){
  }
}

void loop() {
target0();
runCount++;
target1();
runCount++;
target2();
runCount++;

while(true){};//stops program
}

//Target 0
void target0(){
    prepArm();//move arm to raised open position
    //Turn robot torwards target 0
    if(runCount != 0){
    digitalWrite(RD,HIGH);
    digitalWrite(LD,HIGH);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    delay(150);
    }
    //Turn right
    digitalWrite(RD,LOW);
    digitalWrite(LD,HIGH);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    delay(550);
    while(!lineDetectVanilla()){
    digitalWrite(RD,LOW);
    digitalWrite(LD,HIGH);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    }
    
    //Follow first line towards target
    while(!black3()){
      lineFollow();
    }
    analogWrite(RS,0);
    analogWrite(LS,0);
    delay(250);
    backUpForPickUp();//back up tiny bit
    pickUpBall();//drop arm and close gripper on ball (with force sensor)
    delay(500);
    carryBall();//raise arm to carry ball over to basket
    backUp();//back up away from ball position
    turnAround();//turn robot around   
    while(!black3()){
      lineFollow();
    }
    //Needs to find basket
    findBasketLeft();
    //follow line to basket until sensor finds triple black
    while(!black3()){
      lineFollow();
    }
    //backUpForPickUp();//back up a tiny bit
    dropItLikeItsHot();//drop ball in basket
   
    //Return home
    backUp();//back up away from ball position
    turnAround();//turn robot around
    while(!black3()){
      lineFollow();
    }   
    analogWrite(RS,0);
    analogWrite(LS,0);
    delay(500);
}

//Target across from net
void target1(){
    prepArm();//move arm to raised open position
    //Move forward to avoid triple back at origin
    if(runCount = 0){
    //Just finished spinning -- overshot -- Turn right
    while(!lineDetectVanilla()){
    digitalWrite(RD,LOW);
    digitalWrite(LD,HIGH);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    }
    }else{
    digitalWrite(RD,HIGH);
    digitalWrite(LD,HIGH);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    delay(150);
    }
    
    //Follow first line towards target
    while(!black3()){
      lineFollow();
    }
    analogWrite(RS,0);
    analogWrite(LS,0);
    delay(250);
    backUpForPickUp();//back up tiny bit
    pickUpBall();//drop arm and close gripper on ball (with force sensor)
    delay(500);
    carryBall();//raise arm to carry ball over to basket
    backUp();//back up away from ball position
    turnAround();//turn robot around   
   
    while(!black3()){
      lineFollow();
    }
    //inch forward repeat lineFollow and black3
      digitalWrite(RD,HIGH);
      digitalWrite(LD,HIGH);
      analogWrite(RS,rightSpeed);
      analogWrite(LS,leftSpeed);
      delay(150); 
    //follow line to basket until sensor finds triple black
    while(!black3()){
      lineFollow();
    }
    //backUpForPickUp();//back up a tiny bit
    dropItLikeItsHot();//drop ball in basket
   
    //Return home 
    backUp();//back up away from ball position
    turnAround();//turn robot around
    while(!black3()){
      lineFollow();
    }   
    analogWrite(RS,0);
    analogWrite(LS,0);
    delay(500);
}

//Target 2
void target2(){
    prepArm();//move arm to raised open position
    //Turn robot torwards target 2
    if(runCount != 0){
    digitalWrite(RD,HIGH);
    digitalWrite(LD,HIGH);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    delay(150);
    }
    //Turn left
    digitalWrite(RD,HIGH);
    digitalWrite(LD,LOW);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    delay(300);
    while(!lineDetectVanilla()){
    digitalWrite(RD,HIGH);
    digitalWrite(LD,LOW);
    analogWrite(RS,rightSpeed);
    analogWrite(LS,leftSpeed);
    }
    
    //Follow first line towards target
    while(!black3()){
      lineFollow();
    }
    analogWrite(RS,0);
    analogWrite(LS,0);
    delay(250);
    backUpForPickUp();//back up tiny bit
    pickUpBall();//drop arm and close gripper on ball (with force sensor)
    delay(500);
    carryBall();//raise arm to carry ball over to basket
    backUp();//back up away from ball position
    turnAround();//turn robot around   
    while(!black3()){
      lineFollow();
    }
    //Needs to find basket
    findBasketRight();
    //follow line to basket until sensor finds triple black
    while(!black3()){
      lineFollow();
    }
    //backUpForPickUp();//back up a tiny bit
    dropItLikeItsHot();//drop ball in basket
   
    //Return home
    backUp();//back up away from ball position
    turnAround();//turn robot around
    while(!black3()){
      lineFollow();
    }   
    analogWrite(RS,0);
    analogWrite(LS,0);
    delay(500);
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
 
boolean lineDetectVanilla(){
 leftVal = analogRead(LTL);
 centreVal = analogRead(LTC);
 rightVal = analogRead(LTR);   ////Read linetracker sensor values

   if(leftVal>LTHRES||rightVal>RTHRES||centreVal>CTHRES){
      analogWrite(RS,0);
      analogWrite(LS,0);
      return true;
   }else{
    return false;
   }
}

void lineFollow() {
  //Motors to drive straight
  digitalWrite(RD,HIGH);
  digitalWrite(LD,HIGH);

  //Read linetracker sensor values
  int leftVal = analogRead(LTL);
  int centreVal = analogRead(LTC);
  int rightVal = analogRead(LTR);
  
 //Line centred, drive straight
 if(leftVal<LTHRES && centreVal>CTHRES && rightVal<RTHRES){
 analogWrite(RS,rightSpeed);
 analogWrite(LS,leftSpeed);  //Please to use the lowest speed that can make the robot move!!!
 dirChecker = center;
 }
 
 //Veering right, move power to right motor
 else if(leftVal>LTHRES && centreVal<CTHRES && rightVal<RTHRES){
 analogWrite(RS,rightSpeedFast);
 analogWrite(LS,leftSpeed);
 dirChecker = toRight;
 }
 
 else if(leftVal>LTHRES && centreVal>CTHRES && rightVal<RTHRES){
 analogWrite(RS,rightSpeedFast);
 analogWrite(LS,leftSpeed);
 dirChecker = toRight;
 }
 
 //Veering left, move power to left motor
 else if(leftVal<LTHRES && centreVal<CTHRES && rightVal>RTHRES){
 analogWrite(RS,rightSpeed);
 analogWrite(LS,leftSpeedFast);
 dirChecker = toLeft;
 }
 
 else if(leftVal<LTHRES && centreVal>CTHRES && rightVal>RTHRES){
 analogWrite(RS,rightSpeed);
 analogWrite(LS,leftSpeedFast);
 dirChecker = toLeft;
 }
 
else if (leftVal<LTHRES && centreVal<CTHRES && rightVal<RTHRES){ //for other cases, stop driving first.
// analogWrite(RS,0);
// analogWrite(LS,0);
 //delay(500);    //wait a sec, to disdinguish between the going straight move and spin move.
 //spin();
 if(dirChecker == toLeft){
 digitalWrite(RD,LOW);
 digitalWrite(LD,HIGH);
 analogWrite(RS,rightSpeed-20);
 analogWrite(LS,leftSpeed-20);
 //delay(50);
 }else{
 digitalWrite(RD,HIGH);
 digitalWrite(LD,LOW);
 analogWrite(RS,rightSpeed-20);
 analogWrite(LS,leftSpeed-20);
 //delay(50);
 }
//TODO
//  while(!lineDetectVanilla()){}
  }
}

//Method to count and increment motor encoder values
void countEncoder()
{
    //For the left encoder
  if( digitalRead(LM_ENC) == HIGH )
  {//Read left encoder 
     lm_change = 1;
  }
    if( digitalRead(LM_ENC) == LOW && lm_change == 1 )
  {//Sensing left encoder transition from HIGH to LOW
    lm_cnt += 1;
    lm_change = 0;
  }
  
  //For the right encoder
  if( digitalRead(RM_ENC) == HIGH )
  {//Read right encoder
    rm_change = 1;
  }
  if( digitalRead(RM_ENC) == LOW && rm_change == 1 )
  {//Sensing right encoder transition from HIGH to LOW
    rm_cnt += 1;
    rm_change = 0;
  }
  
  return;
}

void prepArm(){
  pan.write(90);
  tilt.write(85);
  grip.write(35);
}

void pickUpBall(){
  pan.write(90);
  tilt.write(75);
  while(analogRead(A2) > 50 && gripVal < 180){
      gripVal+= 7;
      grip.write(gripVal);
      delay(50);
   }
}
void carryBall(){
  pan.write(90);
  tilt.write(130);
}

void backUp(){
  digitalWrite(RD,LOW);
  digitalWrite(LD,LOW);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);

  delay(1000);
}

void turnAround(){
  digitalWrite(RD,HIGH);
  digitalWrite(LD,LOW);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  delay(800);
  while(!lineDetectVanilla()){
 }
  
}

void backUpForPickUp(){
  digitalWrite(RD,LOW);
  digitalWrite(LD,LOW);
  digitalWrite(RS,rightSpeed);
  digitalWrite(LS,leftSpeed);
  delay(20);
  digitalWrite(RS,0);
  digitalWrite(LS,0);
  delay(50);
}

void findBasketRight(){
  delay(100);
  digitalWrite(RD,HIGH);
  digitalWrite(LD,HIGH);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  delay(150);
  //Turn right
  digitalWrite(RD,LOW);
  digitalWrite(LD,HIGH);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  delay(300);
  while(!lineDetectVanilla()){
  digitalWrite(RD,LOW);
  digitalWrite(LD,HIGH);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  }
}

void findBasketLeft(){
  delay(100);
  digitalWrite(RD,HIGH);
  digitalWrite(LD,HIGH);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  delay(150);
  //Turn left
  digitalWrite(RD,HIGH);
  digitalWrite(LD,LOW);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  delay(300);
  while(!lineDetectVanilla()){
  digitalWrite(RD,HIGH);
  digitalWrite(LD,LOW);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  }
}

void dropItLikeItsHot(){
  //Move forward a little bit
  digitalWrite(RD,HIGH);
  digitalWrite(LD,HIGH);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  delay(30);
  //Prepare arm
  pan.write(90);
  tilt.write(120);
  delay(200);
  grip.write(35);
  delay(300);
  prepArm();
}

void tripleSpin(){
  lm_cnt = 0;
  rm_cnt = 0;
  int leftSTemp = leftSpeed;
  int rightSTemp = rightSpeed;
  
  digitalWrite(RD,HIGH);
  digitalWrite(LD,LOW);
  analogWrite(RS,rightSTemp);
  analogWrite(LS,leftSTemp);
  boolean foundLine = false;
  
  while((lm_cnt < 80 && rm_cnt < 80) && !foundLine){
    countLinesCrossed();
    countEncoder();
    if(lineCount > 11){  
      if(lineDetectVanilla())
        foundLine = true;
        Serial.print("FOUND LINE = TRUE: ");
        Serial.print("lm_count = ");
        Serial.print(lm_cnt);
        Serial.print(", rm_count = ");
        Serial.println(rm_cnt);
        
    }
    if(lineCount > 11){
       rightSTemp = rightSTemp - 5;
       leftSTemp = leftSTemp - 5;
    }
  }
  analogWrite(RS,0);
  analogWrite(LS,0);
}

void countLinesCrossed(){
  Serial.println(wasLight);
  centreVal = analogRead(LTC);
  if(centreVal<CTHRES)
    wasLight = true;
  if(centreVal>CTHRES && wasLight == true){
    lineCount++;
    wasLight = false;
  }
}



