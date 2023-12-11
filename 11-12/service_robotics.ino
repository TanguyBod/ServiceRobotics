#include "Motor.h"
#include <QTRSensors.h>
#include <Servo.h>

QTRSensors qtr;
// Servo servo;

const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

int maze[7][7] = {{4, 1, 4, 4, 1, 2, 4}, 
                  {0, 2, 4, 0, 14, 0, 1},
                  {0, 4, 1, 13, 0, 4, 1},
                  {0, 0, 3, 0, 4, 2, 4},
                  {0, 0, 2, 4, 1, 1, 0},
                  {2, 2, 4, 1, 2, 3, 0},
                  {2, 3, 2, 5, 3, 0, 1}};

// First facing then going
int dir_matrix[4][4] = {{3,4,2,1},
                        {4,3,1,2},
                        {1,2,3,4},
                        {2,1,4,3}};

int special_case_1[3] = {2,4,3};
int special_case_2[3] = {3,4,2};

// int turns[] = {2,1,2,4};

// int i_pos = 6;
// int j_pos = 3;
// int facing = 1;

int i_pos = 0;
int j_pos = 4;
int facing = 2;

bool middle_left;
bool middle_right;
bool left;
bool right;
int cur_state;
int prev_state;

int nb_cylinder = 0;

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

int turns[69] = {2,1,1,2,4,1,1,2,2,2,
                 1,1,4,3,3,4,1,1,1,2,
                 1,9,2,2,1,1,4,3,1,
                 3,10,2,2,4,1,1,11,2,1,
                 1,4,2,2,1,12,3,1,2,2,
                 1,1,1,1,2,2,4,1,1,2,
                 2,2,2,1,1,3,1,2,1,1};
// int tmp_dir[34] = {3,1,2,2,1,1,1,1,2,2,4,1,1,2,2,2,2,1,1,3,1,2,1,1};
// int turns[18]= {1,2,2,4,1,1,2,2,2,2,1,1,3,1,2,1,1};
// int turns[19]= {2,3,10,2,2,4,1,1,11,2,1,1,4,2,2,1,12,3,1}; //all cases
int dir_tmp = 0;

void setup(){
  pinMode(motor1pin1,OUTPUT);
  pinMode(motor1pin2,OUTPUT);
  pinMode(motor2pin1,OUTPUT);
  pinMode(motor2pin2,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){2, 3, 4, 7}, SensorCount); 
  qtr.setEmitterPin(2);
  // Serial.begin(9600);
  // delay(3000);
}

void loop() {
  // exitMaze();
  // Serial.print("Position :")
  mazeMovements();
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
    } else if (left == true && middle_left == false && middle_right == false && right == false) {
      return 4;
    } else if (left == false && middle_left == false && middle_right == false && right == true) {
      return 4;
    }
}

bool searchNextIntersection() {
    int state_s = 0; bool inter_found = true; int reduced = 0;
    while (state_s != 3 && state_s != 4) {
        if (digitalRead(button_pin) == 0) {
          m.stop();
          pick_cylinder();
          delay(100);
          state_s = get_sensor_state();
          if(state_s == 1 || state_s == 2) {
            m.forward(speed);
            delay(150);
            m.stop();
          }
        }
        state_s = get_sensor_state();
        if (state_s == 0) {
          if(reduced<100) {m.forward(speed-40);}
          else {m.forward(speed);}
        } else if (state_s == 1)  {
          m.forward_right(speed+10);
        } else if (state_s == 2) {
          m.forward_left(speed+10);
        }
        delay(4);
        reduced+=1;
    }
    if (state_s == 4) {inter_found = false;}
    m.stop();
    return inter_found;
}

void moveServo(int angle) {
  int pulseWidth = map(angle, 0, 180, 500, 2400);
  digitalWrite(9, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(9, LOW);
  delay(20);
}

void pick_cylinder() {
  delay(500);
  nb_cylinder+=1;
  for (int angle = 180; angle >= 90; angle -= 5) {
    moveServo(angle);
  }
  delay(100);
  for (int angle = 90; angle >= 0; angle -= 5) {
    moveServo(angle);
  }
  delay(100);
  for (int angle = 0; angle <= 180; angle += 5) {
    moveServo(angle);
  }
}

// 1 = west, 2 = est, 3 = north, 4 = south
void exitMaze() {
  int direction_exit = maze[i_pos][j_pos]; int relative_turn;
  bool first_time = true; 
  while(direction_exit != 5) {

    if (first_time) {first_time = false;}
    else {
      bool intersection = searchNextIntersection();
    }
    delay(10);
    m.forward(speed-10);
    delay(200);
    m.stop();
    delay(10);
    int direction_exit = maze[i_pos][j_pos];
    if (direction_exit != 13 && direction_exit != 14) {
      relative_turn = dir_matrix[facing-1][direction_exit-1];
    } else if (direction_exit == 13) {
      relative_turn = 13;
    } else if (direction_exit == 14) {
      relative_turn = 14;
    }
    if(relative_turn==1) {
      m.leftTurn(speed-30);
      delay(500);
    } else if (relative_turn==2) {
      m.rightTurn(speed-30);
      delay(500);
    } else if (relative_turn==3) {
      m.forward(speed-20);
      delay(100);
      m.stop();
    } else if (relative_turn==4) {
      m.rightTurn(speed-20);
      delay(600);
    } else if (relative_turn==13){ // special case 3 : straight down/back 3-->4
      m.leftTurn(speed-30);
      delay(500);
      int state = get_sensor_state();
      while(state != 0) {
        delay(5);
        state = get_sensor_state();
      }
      m.stop();
      bool inter = searchNextIntersection();
      m.forward(speed);
      delay(800);
      m.rightTurn(speed-10);  // make left turn
      delay(750);
      m.forward(speed-20);
      delay(1100);
      m.stop();
      state = get_sensor_state();
      if (state !=0 && state != 1 && state !=2) {
        m.leftTurn(speed-20);
        while(true) {
          state = get_sensor_state();
          if (state == 1 || state ==0 || state == 2) {
            m.stop();
            break;
          }
        }
      }
    } else if (relative_turn == 14) {
      m.leftTurn(speed-30);
      delay(500);
      int state = get_sensor_state();
      while(state != 0) {
        delay(5);
        state = get_sensor_state();
      }
      m.stop();
      bool inter = searchNextIntersection();
      m.special_case(speed);
      delay(1600);
      m.stop();
      state = get_sensor_state();
      if (state !=0 && state != 1 && state !=2) {
        m.leftTurn(speed-20);
        while(true) {
          state = get_sensor_state();
          if (state == 1 || state ==0 || state == 2) {
            m.stop();
            break;
          }
        }
      }
    }
    int state = get_sensor_state();
    if(relative_turn!=3 && relative_turn!=13 && relative_turn !=14) {
      while (state != 0) {
        delay(5);
        state = get_sensor_state();
      }
    }
    m.stop();
    update_facing(relative_turn);
    update_position();
    if(relative_turn == 14 || relative_turn == 13) {
      facing = 4; i_pos = 3; j_pos =4;
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
    if(going == 10) {
      facing = 1;
    } else {
      facing = going;
    }
  } else if (facing == 1) {
    if (going == 1) {
      facing = 4;
    } else if (going == 2) {
      facing = 3;
    } else if (going == 4) {
      facing = 2;
    }
  } else if (facing == 2) {
    if (going == 1 || going == 11) {
      facing = 3;
    } else if (going == 2) {
      facing = 4;
    } else if (going == 4 || going == 9) {
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

void mazeMovements(){
  bool intersection = searchNextIntersection();
  if(dir_tmp!=0) {update_facing(turns[dir_tmp-1]);}
  update_position();
  if (turns[dir_tmp-1]==10) {
    i_pos = 2; j_pos = 3; facing = 1;
  } else if (turns[dir_tmp-1]==11) {
    i_pos = 1; j_pos = 4; facing = 3;
  } else if (turns[dir_tmp-1]==12) {
    i_pos = 3; j_pos = 4; facing = 4;
  }
  if(nb_cylinder == 3) {
    exitMaze();
  }
  delay(50);
  m.forward(speed-10);
  delay(200);
  //delay(250);
  m.stop();
  delay(50);
  if(turns[dir_tmp]==1) {
    m.leftTurn(speed-30);
    delay(500);
  } else if (turns[dir_tmp]==2) {
    m.rightTurn(speed-30);
    delay(500);
  } else if (turns[dir_tmp]==3) {
    delay(10);
  } else if (turns[dir_tmp]==4) {
    m.rightTurn(speed-20);
    delay(600);
  } else if (turns[dir_tmp]==10){ // special case 1: left turn  from 4-->1
    m.forward(speed);
    delay(800);
    m.leftTurn(speed-10);  // make left turn
    delay(650);
    m.forward(speed-20);
    delay(650);
    m.stop();
    int state = get_sensor_state();
    if (state !=0 && state != 1 && state !=2) {
      m.leftTurn(speed-20);
      while(true) {
        state = get_sensor_state();
        if (state == 1 || state ==0 || state == 2) {
          m.stop();
          break;
        }
      }
    }
  } else if (turns[dir_tmp]==11){ //special case 2: go straight/right  1-->3
    m.forward(speed);
    delay(700);
    m.leftTurn(speed-10);  // make left turn
    delay(700);
    m.forward(speed-20);
    delay(700);
    m.stop();
    int state = get_sensor_state();
    if (state !=0 && state != 1 && state !=2) {
      m.leftTurn(speed-20);
      while(true) {
        state = get_sensor_state();
        if (state == 1 || state ==0 || state == 2) {
          m.stop();
          break;
        }
      }
    }
  } else if (turns[dir_tmp]==12){ // special case 3 : straight down/back 3-->4
    m.special_case(speed);
    delay(1550);
    m.stop();
    int state = get_sensor_state();
    if (state !=0 && state != 1 && state !=2) {
      m.leftTurn(speed-20);
      while(true) {
        state = get_sensor_state();
        if (state == 1 || state ==0 || state == 2) {
          m.stop();
          break;
        }
      }
    }
  } else if (turns[dir_tmp]==9) {
    delay(3000);
    m.rightTurn(speed-20);
    delay(600);
  }
  int state = get_sensor_state();
  if(turns[dir_tmp]!=3 && turns[dir_tmp]!=10 && turns[dir_tmp]!=11 && turns[dir_tmp]!=12) {
    while (state != 0) {
      delay(5);
      state = get_sensor_state();
    }
  }
  m.stop();
  //Maybe update facing here
  dir_tmp +=1;
}






