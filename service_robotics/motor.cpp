#include <Motor.h>
#include<Servo.h>

/*Move the servo in the directions we want*/

Motor::forward(int forward_value,int LedPinRight, int LedPinLeft){
  analogWrite(motor1pin1, 125);
  digitalWrite(motor1pin2, LOW);
  analogWrite(motor2pin1, 125);
  digitalWrite(motor2pin2, LOW);   
 
    
}
Motor::left(int left_rotation,int LedPin){
    for(i= 0; i<=90; i+=1 ){
     analogWrite(motor1pin1, i);
     digitalWrite(motor1pin2, LOW);
     analogWrite(motor2pin1, 125);
     digitalWrite(motor2pin2, LOW);       
      delay(15);

    } 
}
Motor::right(int right_rotation,int LedPin){
     for(i= 0; i<=90; i+=1 ){
     servo.write(i);             
      delay(15);

    }
    
}
Motor::halfRotation(int degree){
    for(i= 0; i<=180; i+=1 ){
     servo.write(i);             
      delay(15);

    }

}
{