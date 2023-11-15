#include <corner.h>
#include <Motor.h>
#include <Corner.h>
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

Corner c1 = new Corner(1,)



void setup() {
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){2, 3, 4, 5}, SensorCount); 
  // !!! Important !!! 
  // Pin 2 = Left sensor
  // Pin 3 = Middle-Left sensor
  // Pin 4 = Middle-Right sensor
  // Pin 5 = Right sensor
  // !!! Important !!!
  qtr.setEmitterPin(2);
}

// !! Need to check every then and now if button is pressed --> cylinder --> stop everything and grab it


void loop() {
  //Search trough maze for cylinders. cylinder variable tracks the number of cylinders found.
  int cylinder = 0;

  while cylinder < 3 :
    prev_state = cur_state;
    cur_state = get_sensor_state();
    if (cur_state == 0) {
        // Motor.forward()
    } else if (cur_state == 1) {
        // Adjust and turn right
        while (middle_left == false) {
            //Motor.turnRight(a bit)
            //Motor.forward()
            //delay(a bit);
            //int a = get_sensor_state;
        }
    } else if (cur_state == 2) {
        // Adjust and turn left
        while (middle_right == false) {
            //Motor.turnRight(a bit)
            //Motor.forward()
            //delay(a bit);
            //int a = get_sensor_state;
        }
    } else if (cur_state == 3) {
        // Intersection

    } else if (cur_state == 4) {
        // No trail
    }


}

int get_sensor_state() {
    qtr.read(sensorValues);
    int threshold = 1400; 
    left = true; middle_left=true; middle_right =true; right = true;
    if (sensorValues[0] <= threshold) {left = false;}
    if (sensorValues[0] <= threshold) {middle_left = false;}
    if (sensorValues[0] <= threshold) {middle_right = false;}
    if (sensorValues[0] <= threshold) {right = false;}
    if (left == false && middle_left == true && middle_right == true && right == false) {    
        return 0;
    } else if (left == false && middle_left == false && middle_right == true && right == false) {    
        return 1;
    } else if (left == false && middle_left == true && middle_right == false && right == false) {    
        return 2;
    } else if (middle_left == true && middle_right == true && (right == true || left == true)) {    
        return 3;
    } else if (left == false && middle_left == false && middle_right == false && right == false) {    
        return 4;
    }
}