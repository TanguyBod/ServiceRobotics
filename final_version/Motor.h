#ifndef MOTOR_H
#define MOTOR_H

/*!
 * \file Motor.h
 * \brief Motor control
 * \author Tanguy Bodin / Nasra Omar ALi
 * \version 1.0
 */

#include <Arduino.h>
class  Motor{

  private:
    int motor1pin1;
    int motor2pin1;
    int motor1pin2;
    int motor2pin2;

  public:
    /*!
     *  \brief Motor constructor
     *
     *  Constructor of the Motor class
     *
     *  \param motor1pin1 is pin for forward control of left motor
     *  \param motor1pin2 is pin for backward control of left motor
     *  \param motor2pin1 is pin for forward control of right motor
     *  \param motor2pin2 is pin for backward control of right motor
     */
    Motor(int motor1pin1,int motor1pin2,int motor2pin1,int motor2pin2);

    /*!
     *  \brief Forward control
     *
     *  Set the speed and forward direction to both motors
     *
     *  \param speed represents the speed of the motor
     */
    void forward(int speed);

    /*!
     *  \brief Right turn
     *
     *  Set the speed and create a right turn moving right wheel backward and left wheel forward
     *
     *  \param speed represents the speed of the motor
     */
    void leftTurn(int speed);

    /*!
     *  \brief Left turn
     *
     *  Set the speed and create a right turn moving right wheel forward and left wheel backward
     *
     *  \param speed represents the speed of the motor
     */
    void rightTurn(int speed);

    /*!
     *  \brief Stop the motor
     *
     */
    void stop();

    /*!
     *  \brief Create a forward/right movement
     *
     *  \param speed represents the speed of the motor
     */
    void forward_right(int speed);

    /*!
     *  \brief Create a forward/left movement
     *
     *  \param speed represents the speed of the motor
     */
    void forward_left(int speed);

    /*!
     *  \brief Special function for case 13 speed control
     *
     *  \param speed represents the speed of the motor
     */
    void special_case(int speed);

};

#endif
