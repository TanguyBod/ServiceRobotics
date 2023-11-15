#ifndef MOTOR_H
#define MOTOR_H
class  Motor{

private:
    /* data */
    int forward; 
    int left;
    int right;
    int halfRotation;
    int LedPinRight;
    int LedPinLeft;
public:
    forward(int forward_value,int LedPinRight,int LedPinLeft);
    left(int left_rotation,int LedPinRight, int LedPinLeft);
    right(int right_rotation,int LedPinRight, int LedPinLeft);
    halfRotation(int degree,int LedPinRight, int LedPinLeft)



#endif
