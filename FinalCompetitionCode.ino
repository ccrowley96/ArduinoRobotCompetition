////////////////////////////
// ELEC299 - Winter2017  
//////////////////////////// 
// “FinalCompetitionCode”  //  //  
// Group #: “17”  //  
// Group Members: Cory Crowley, Akin Shonibare, Yijie Cui 
// Cory Crowley (10180322)  
// Akin Shonibare (10185253)  
// Joey Votto (10170009)    
////////////////////////////
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
int i;
char a;
bool wallDetected = false;
bool foundLine = false;
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
  
  Serial.begin(9600);
  Serial.begin(115200);
  
  //Load speed values from stored calibration memory
  left_speed = EEPROM.read(0);
  right_speed = EEPROM.read(1); 
  int storedLS = (int)left_speed;
  int storedRS = (int)right_speed;
  Serial.print("Stored left speed: ");
  Serial.println(left_speed);
  Serial.print("Stored right speed: ");
  Serial.println(right_speed);
  
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
if(Serial.available()){
    //bluetoothVal = Serial.read();
    a = Serial.read();
    Serial.print("Value Received: ");
    Serial.println(a);
    if(a == 'A'){
      Serial.println("0 Received");
      target0();
      runCount++;
    }else if(a == 'B'){
      Serial.println("1 Received");
      target1();
      runCount++;
    }else if(a == 'C'){
      Serial.println("2 Received");
      target2();
      runCount++;
    }
  }   
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
    
    i = 0;
    while((!analogRead(A2) > 50) && i < 3){
      pickUpBall();
      i++;
    }
    
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
    if(runCount == 0){
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

    i = 0;
    while((!analogRead(A2) > 50) && i < 3){
      pickUpBall();
      i++;
    }
    
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

    i = 0;
    while((!analogRead(A2) > 50) && i < 3){
      pickUpBall();
      i++;
    }
    
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

void prepArm(){
  pan.write(90);
  tilt.write(85);
  grip.write(35);
}

void pickUpBall(){
  pan.write(90);
  tilt.write(80);
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
  delay(500);
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
  delay(15);
  digitalWrite(RS,0);
  digitalWrite(LS,0);
  delay(30);
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
}

void tripleSpinV2(){
  digitalWrite(RD,HIGH);
  digitalWrite(LD,LOW);
  analogWrite(RS,rightSpeed);
  analogWrite(LS,leftSpeed);
  delay(6675);
  analogWrite(RS,0);
  analogWrite(LS,0);
}

void tripleSpin(){
  foundLine = false;
  digitalWrite(RD,HIGH);
  digitalWrite(LD,LOW);
  analogWrite(RS,leftSpeed);
  analogWrite(LS,rightSpeed);
  
  while(!foundLine){
    countLinesCrossed();
    Serial.print("Lines Crossed: ");
    Serial.println(lineCount);
    
    if(lineCount > 11){  
      if(lineDetectVanilla()){
        foundLine = true;      
      }   
    }
    
  }
  analogWrite(RS,0);
  analogWrite(LS,0);
}

void countLinesCrossed(){
  Serial.println(wasLight);
  centreVal = analogRead(LTC);
  if(centreVal<CTHRES){
    wasLight = true;
  }
  else if(centreVal>CTHRES && wasLight == true){
    lineCount++;
    wasLight = false;
  }
}


