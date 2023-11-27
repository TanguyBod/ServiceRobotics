#include "Motor.h"
#include <Servo.h> 


/*the pins who are they connected*/
  Motor::Motor(int motor1pin1,int motor1pin2,int motor2pin1,int motor2pin2){
    this->motor1pin1= motor1pin1;
    this->motor1pin2= motor1pin2;
    this->motor2pin1= motor2pin1;
    this->motor2pin2= motor2pin2; 
  }  

/*Move the motor in the directions we want*/

  void Motor::forward(int speed){
    analogWrite(motor1pin1, speed+20);
    digitalWrite(motor1pin2, LOW);
    analogWrite(motor2pin1, speed);
    digitalWrite(motor2pin2, LOW);   
  }

  void Motor::leftTurn(int speed){
    analogWrite(motor1pin1, speed+20);
    digitalWrite(motor1pin2, LOW); 
    digitalWrite(motor2pin1, LOW);
    analogWrite(motor2pin2, speed);
  }

  void Motor::rightTurn(int speed){
    digitalWrite(motor1pin1,LOW) ;
    analogWrite(motor1pin2, speed+20);
    analogWrite(motor2pin1, speed);
    digitalWrite(motor2pin2,LOW);
  }
  
 //turn left twice , need TESTING
 void Motor::halfRotation(int sp ){
     analogWrite(motor2pin2, 125);
    digitalWrite(motor1pin1, LOW);

   // digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    analogWrite(motor1pin2, 125);
    delay(5);
    analogWrite(motor2pin2, 125);
    digitalWrite(motor1pin1, LOW);

   // digitalWrite(motor2pin1, HIGH);
    digitalWrite(motor2pin2, LOW);
    analogWrite(motor1pin2, 125);
  }
   
  void Motor::stop(){
    analogWrite(motor1pin1, 0);
    digitalWrite(motor1pin2, LOW);
    analogWrite(motor2pin1, 0);
    digitalWrite(motor2pin2, LOW); 
  }

  void Motor::forward_right(int speed) {
    analogWrite(motor1pin1, speed-40);
    digitalWrite(motor1pin2, LOW);
    analogWrite(motor2pin1, speed);
    digitalWrite(motor2pin2, LOW); 
  }
   
  void Motor::forward_left(int speed) {
    analogWrite(motor1pin1, speed);
    digitalWrite(motor1pin2, LOW);
    analogWrite(motor2pin1, speed-30);
    digitalWrite(motor2pin2, LOW);
  }
// two new methods for adjusting when leaving

//motortesting.ino
/*#include "Motor.h"

int motor1pin1 = 5;
int motor1pin2 = 6;

int motor2pin1 = 10;
int motor2pin2 = 11;

Motor m(5,6,10,11);

void setup() {
  Serial.begin(115200);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  m.forward(40);
  //delay(1000);
  //m.stop();
  //elay(1000);
  //m.rightTurn();
  //delay(1000);
  //m.forward(40);
  //delay(1000);
  //m.stop();
  ///delay(1000);
  //m.leftTurn();
 // m.stop();



}*/
