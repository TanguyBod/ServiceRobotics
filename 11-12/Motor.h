#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>
class  Motor{

 private:
    int sp;     /*rotationspeed;*/
    int motor1pin1;
    int motor2pin1;
    int motor1pin2;
    int motor2pin2;

 public:
    Motor(int motor1pin1,int motor1pin2,int motor2pin1,int motor2pin2);
   //void servoInitizatesSetup();
   void forward(int speed);
   void leftTurn(int speed);
   void rightTurn(int speed);
   void stop();
   void forward_right(int speed);
   void forward_left(int speed);
   void special_case(int speed);

};

#endif
