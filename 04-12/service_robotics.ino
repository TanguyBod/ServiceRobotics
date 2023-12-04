#include "Motor.h"
#include <QTRSensors.h>
#include <Servo.h>

QTRSensors qtr;
Servo servo;

const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

int maze[7][7] = {{4, 1, 4, 4, 1, 2, 4}, 
                  {0, 2, 4, 0, 4, 0, 1},
                  {0, 4, 1, 2, 10, 4, 1},
                  {0, 0, 3, 0, 4, 2, 4},
                  {0, 0, 2, 4, 1, 1, 0},
                  {2, 2, 4, 1, 2, 3, 0},
                  {2, 3, 2, 5, 3, 0, 1}};

int special_case_1[3] = {2,4,3};
int special_case_2[3] = {3,4,2};

// int turns[] = {2,1,2,4};

int i_pos = 6;
int j_pos = 3;
int facing = 1;

bool middle_left;
bool middle_right;
bool left;
bool right;
int cur_state;
int prev_state;

int nb_cylinder = 0;

// int speed = 100 + nb_cylinder*10;
int speed = 80;
int threshold = 700; 

int button_pin = 8;

int trigPin = 12;    // TRIG pin
int echoPin = 13;    // ECHO pin

int motor1pin1 = 5;
int motor1pin2 = 6;
int motor2pin1 = 11;
int motor2pin2 = 10;
Motor m(motor1pin1,motor1pin2,motor2pin1,motor2pin2);

int tmp_dir[51] = {2,1,1,2,4,1,1,2,2,2,1,1,4,3,3,4,1,1,1,3,1,1,4,3,1,3,10,2,2,4,1,11,2,1,1,4,2,2,1,12,3,3,1,2,2,1,1,1,1,2,2};
int dir_tmp = 0;

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){2, 3, 4, 7}, SensorCount); 
  qtr.setEmitterPin(2);
  // Serial.begin(115200);
  // leftHandSearch();
  // m.forward(speed);
}

void loop() {
  bool intersection = searchNextIntersection();
  delay(100);
  advance_center_rotation();
  delay(100);
  // m.leftTurn(speed-40);
  // delay(300);
  if(tmp_dir[dir_tmp]==1) {
    m.leftTurn(speed-40);
    delay(500);
  } else if (tmp_dir[dir_tmp]==2) {
    m.rightTurn(speed-40);
    delay(500);
  } else if (tmp_dir[dir_tmp]==3) {
    m.forward(speed);
    delay(300);
  } else if (tmp_dir[dir_tmp]==4) {
    m.rightTurn(speed-20);
    delay(600);
  }
  int state = get_sensor_state();
  while (state != 0) {
    delay(5);
    state = get_sensor_state();
  } 
  m.stop();
  delay(100);
  dir_tmp +=1;
}

void advance_center_rotation() {
  float distance_to_advance = 5.0;
  float initial_distance = mesure_distance();
  m.forward(speed);
  delay(100); 
  m.stop();
  float distance = mesure_distance();
  m.forward(speed);
  while (initial_distance-distance<=distance_to_advance) {
    delay(10);
    m.stop();
    delay(10);
    distance = mesure_distance();
    m.forward(speed);
  }
  m.stop();
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
    while (state_s != 3 && state_s != 4) {
        if (digitalRead(button_pin) == 0) {
          m.stop();
          pick_cylinder();
        }
        state_s = get_sensor_state();
        if (state_s == 0) {
          m.forward(speed);
        } else if (state_s == 1)  {
          m.forward_right(speed);
        } else if (state_s == 2) {
          m.forward_left(speed);
        }
        
        delay(4);
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
  for (int i = 0; i<20; i++) {
    state_val = get_sensor_state();
    if (sensorValues[0]>threshold) {left = true;}
    if (sensorValues[3]>threshold) {right = true;}
    if (digitalRead(button_pin) == 0) {
      m.stop();
      pick_cylinder();
      m.forward(speed);
    }
    delay(10);
  }
  m.stop();
  delay(50);
  state_val = get_sensor_state();
  if (state_val == 0 || state_val == 1 || state_val ==2) {front = true;}
  if (left) {return 1;}
  else if (front) {return 3;}
  else if (right) {return 2;}
  else {return 4;}
}

void pick_cylinder() {
  nb_cylinder+=1;
  servo.attach(9);
  servo.write(0);
  delay(2000);
  servo.write(180);
  delay(2000);
  servo.detach();
}

// 1 = west, 2 = est, 3 = north, 4 = south
void exitMaze() {
  int direction_exit = maze[i_pos][j_pos];
  while (direction_exit != 5) {
    // Facing west
    if (facing == 1) { 
      if (direction_exit == 3) {
        m.rightTurn(speed-40);
        delay(200);
      } else if (direction_exit == 4) {
        m.leftTurn(speed-40);
        delay(200);
      } else if (direction_exit == 2) {
        m.rightTurn(speed-40);
        //Add delay approximatly 100-120]
      }
    }
    // Facing est
    else if (facing == 2) { 
      if (direction_exit == 4) {
        m.rightTurn(speed-40);
        delay(200);
      } else if (direction_exit == 3) {
        m.leftTurn(speed-40);
        delay(200);
      } else if (direction_exit == 1) {
        m.rightTurn(speed-40);
        //Add delay approximatly 100-120]
      }
    }
    // Facing north
    else if (facing == 3) { 
      if (direction_exit == 2) {
        m.rightTurn(speed-40);
        delay(200);
      } else if (direction_exit == 1) {
        m.leftTurn(speed-40);
        delay(200);
      } else if (direction_exit == 4) {
        m.rightTurn(speed-40);
        //Add delay approximatly 100-120]
      }
    }
    // Facing south
    else if (facing == 4) { 
      if (direction_exit == 1) {
        m.rightTurn(speed-40);
        delay(200);
      } else if (direction_exit == 2) {
        m.leftTurn(speed-40);
        delay(200);
      } else if (direction_exit == 3) {
        m.rightTurn(speed-40);
        //Add delay approximatly 100-120]
      }
    }
    else {
      m.forward(speed);
      delay(100);
    }
    int state = get_sensor_state();
    while (state != 0) {
      delay(5);
      state = get_sensor_state();
    }
    m.stop();
    bool toto = searchNextIntersection();
    if (direction_exit == 4) {
      i_pos -=1;
      while(maze[i_pos][j_pos] == 0) {
        i_pos -=1;
      }
    } else if (direction_exit == 1) {
      j_pos -=1;
      while(maze[i_pos][j_pos] == 0) {
        j_pos -=1;
      }
    } else if (direction_exit == 3) {
      i_pos +=1;
      while(maze[i_pos][j_pos] == 0) {
        i_pos +=1;
      }
    } else if (direction_exit == 2) {
      j_pos +=1;
      while(maze[i_pos][j_pos] == 0) {
        j_pos +=1;
      }
    }
  }
}

float mesure_distance() {
  float duration_us, distance_cm;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration_us = pulseIn(echoPin, HIGH);
  distance_cm = 0.017 * duration_us;
  return distance_cm;
}

void update_facing(int going) {
  if (facing == 3) {
    facing = going;
  } else if (facing == 1) {
    if (going == 1) {
      facing = 4;
    } else if (going == 2) {
      facing = 3;
    } else if (going == 4) {
      facing = 2;
    }
  } else if (facing == 2) {
    if (going == 1) {
      facing = 3;
    } else if (going == 2) {
      facing = 4;
    } else if (going == 4) {
      facing = 1;
    }
  } else if (facing == 4) {
    if (going == 1) {
      facing = 2;
    } else if (going == 2) {
      facing = 1;
    } else if (going == 4) {
      facing = 3;
    }
  }
}

void update_position() {
  if (facing == 4) {
    i_pos +=1;
    while(maze[i_pos][j_pos] == 0) {
      i_pos +=1;
    }
  } else if (facing == 1) {
    j_pos -=1;
    while(maze[i_pos][j_pos] == 0) {
      j_pos -=1;
    }
  } else if (facing == 3) {
    i_pos -=1;
    while(maze[i_pos][j_pos] == 0) {
      i_pos -=1;
    }
  } else if (facing == 2) {
    j_pos +=1;
    while(maze[i_pos][j_pos] == 0) {
      j_pos +=1;
    }
  }
}

// Find break condition
void leftHandSearch() {
  bool update_pos_enable = true;
  int first_1 = 0;
  int first_2 = 0;
  bool intersection;
  while (nb_cylinder < 3) { //add  special case check
    Serial.print("Current position \t");
    Serial.print(i_pos);
    Serial.print("\t");
    Serial.println(j_pos);
    intersection = searchNextIntersection();
    if (intersection) {
      delay(200);
      int dir = decidePath();
      // Serial.print("Direction :\t");
      // Serial.println(dir);
      if (update_pos_enable) {update_position();} // Update position if possible
      // Special cases handler
      // if (i_pos == 5 && j_pos == 1 && first_1 < 3) {
      //   Serial.println("Special case 1");
      //   update_pos_enable = false;
      //   dir = special_case_1[first_1];
      //   first_1 += 1; 
      //   if(first_1 == 3) {update_pos_enable = true;}
      // } else if (i_pos == 3 && j_pos == 4 && first_2 < 3) {
      //   Serial.println("Special case 2");
      //   update_pos_enable = false;
      //   dir = special_case_2[first_2];
      //   first_2 += 1; 
      //   if(first_2 == 3) {update_pos_enable = true;}
      // }
      // Compute new facing and turn
      // Serial.print("Old/New Facing : \t");
      // Serial.print(facing);
      // Serial.print("\t");
      update_facing(dir);
      // Serial.println(facing);
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
    } else { // If no intersection found == dead end
      Serial.println("No intersection found section");
      if (update_pos_enable) {update_position();}
      update_facing(4);
      m.leftTurn(speed-30);
      // delay(200);
      for (int i = 0; i<10; i++) {
        if (digitalRead(button_pin) == 0) {
          m.stop();
          pick_cylinder();
          m.leftTurn(speed-30);
        }
        delay(10);
      }
      int state = get_sensor_state();
      while (state != 0) {
        delay(5);
        state = get_sensor_state();
      } 
      m.stop();
    }
  }
}











