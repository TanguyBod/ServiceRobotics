#include "Motor.h"
#include <QTRSensors.h>


QTRSensors qtr;

const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

bool middle_left;
bool middle_right;
bool left;
bool right;
int cur_state;
int prev_state;

int speed = 80;
int threshold = 900; 

int motor1pin1 = 5;
int motor1pin2 = 6;
int motor2pin1 = 11;
int motor2pin2 = 10;
Motor m(motor1pin1,motor1pin2,motor2pin1,motor2pin2);

// North=1, East=2, South=3, West=4
int facing = 1;

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){2, 3, 4, 7}, SensorCount); 
  qtr.setEmitterPin(2);
  leftHandSearch(0);
}


void loop() {
}

int get_sensor_state() {
    qtr.read(sensorValues);
    left = true; middle_left=true; middle_right =true; right = true;
    if (sensorValues[0] <= threshold) {left = false;}
    if (sensorValues[1] <= threshold) {middle_left = false;}
    if (sensorValues[2] <= threshold) {middle_right = false;}
    if (sensorValues[3] <= threshold) {right = false;}
    if (left == false && middle_left == true && middle_right == true && right == false) {    
      return 0; //on the line
    } else if (left == false && middle_left == false && middle_right == true && right == false) {    
      return 1; //a bit on the left
    } else if (left == false && middle_left == true && middle_right == false && right == false) {    
      return 2; //a bit on the right
    } else if (middle_left == true && middle_right == true && (right == true || left == true)) {    
      return 3; //Intersection
    } else if (left == false && middle_left == false && middle_right == false && right == false) {    
      return 4; // No trail
    } else if (left == true && middle_left == true && middle_right == false && right == false) {
      return 3;
    } else if (left == false && middle_left == false && middle_right == true && right == true) {
      return 3;
    }
}

bool searchNextIntersection() {
    int state_s = 0; bool inter_found = true;
    while (state_s != 3 && state_s !=4) {
        state_s = get_sensor_state();
        if (state_s == 0) {
            m.forward(speed);
        } else if (state_s == 1)  {
            m.forward_right(speed-20);
        } else if (state_s == 2) {
            m.forward_left(speed-20);
        }
        delay(5);
    }
    if (state_s == 4) {inter_found = false;}
    m.stop();
    return inter_found;
}

//left=1 , right=2, front=3, backward = 4
int decidePath() {
  bool left = false; bool right = false; bool front = false; int state_val;
  // state_val = get_sensor_state();
  if (sensorValues[0]>threshold) {left = true;}
  if (sensorValues[3]>threshold) {right = true;}
  m.forward(speed);
  delay(200);
  m.stop();
  delay(50);
  state_val = get_sensor_state();
  if (state_val == 0 || state_val == 1 || state_val ==2) {front = true;}
  if (left) {return 1;}
  else if (front) {return 3;}
  else if (right) {return 2;}
  else {return 4;}
}

// Find break condition
void leftHandSearch(int nb_cyl_found) {
  bool intersection;
  while (nb_cyl_found < 3) {
    intersection = searchNextIntersection();
    if (intersection) {
      delay(200);
      int dir = decidePath();
      if (dir == 1) {m.leftTurn(speed-40);}
      else if (dir == 2) {m.rightTurn(speed-40);}
      delay(200);
      int state = get_sensor_state();
      if (dir == 1 or dir == 2) {
        while (state != 0) {
          delay(5);
          state = get_sensor_state();
        } 
        m.stop();
      }
    } else { 
      m.rightTurn(speed-30);
      delay(200);
      int state = get_sensor_state();
      while (state != 0) {
        delay(5);
        state = get_sensor_state();
      } 
      m.stop();
    }
  }
}
  























