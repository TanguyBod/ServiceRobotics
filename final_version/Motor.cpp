/*!
 * \file Motor.h
 * \brief Motor control
 * \author Tanguy Bodin / Nasra Omar ALi
 * \version 1.0
 */

#include "Motor.h"
#include <Servo.h> 

Motor::Motor(int motor1pin1,int motor1pin2,int motor2pin1,int motor2pin2){
  this->motor1pin1= motor1pin1;
  this->motor1pin2= motor1pin2;
  this->motor2pin1= motor2pin1;
  this->motor2pin2= motor2pin2; 
}  

void Motor::forward(int speed){
  analogWrite(motor1pin1, speed+15);
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

void Motor::stop(){
  analogWrite(motor1pin1, 0);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor2pin1, 0);
  digitalWrite(motor2pin2, LOW); 
}

void Motor::forward_right(int speed) {
  analogWrite(motor1pin1, speed-50);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor2pin1, speed+15);
  digitalWrite(motor2pin2, LOW); 
}
  
void Motor::forward_left(int speed) {
  analogWrite(motor1pin1, speed+5);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor2pin1, speed-40);
  digitalWrite(motor2pin2, LOW);
}

void Motor::special_case(int speed) {
  analogWrite(motor1pin1, speed+5);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor2pin1, speed);
  digitalWrite(motor2pin2, LOW); 
}