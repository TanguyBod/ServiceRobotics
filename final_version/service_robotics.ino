/*!
 * \file service_robotics.ino
 * \brief Robot maze solving algorithm main file
 * \author Tanguy Bodin / Nasra Omar Ali
 * \version 1.0
 */

#include "Motor.h"
#include <QTRSensors.h>

// Reflection sensor variables
QTRSensors qtr;
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];
bool middle_left; bool middle_right; bool left; bool right;
int threshold = 700; 

// Variables for maze navigation and positioning
int maze[7][7] = {{4, 1, 4, 4, 1, 2, 4}, 
                  {0, 2, 4, 0, 14, 0, 1},
                  {0, 4, 1, 13, 0, 4, 1},
                  {0, 0, 3, 0, 4, 2, 4},
                  {0, 0, 2, 4, 1, 1, 0},
                  {2, 2, 4, 1, 2, 3, 0},
                  {2, 3, 2, 5, 3, 0, 1}};
int dir_matrix[4][4] = {{3,4,2,1},{4,3,1,2},{1,2,3,4},{2,1,4,3}};
int i_pos = 6; int j_pos = 3; int facing = 1; // Initial position in the maze
int speed = 80; // Int between 0 and 255

// Variables for button and cylinder tracking
int button_pin = 8;
int nb_cylinder = 0;

// Variables for motors
int motor1pin1 = 5; int motor1pin2 = 6;
int motor2pin1 = 11; int motor2pin2 = 10;
Motor m(motor1pin1,motor1pin2,motor2pin1,motor2pin2);

// Every turns we need to take. Turns are relative to robot facing. 9/10/11/12 are special cases
int turns[69] = {2,1,1,2,4,1,1,2,2,2,
                 1,1,4,3,3,4,1,1,1,2,
                 1,9,2,2,1,1,4,3,1,
                 3,10,2,2,4,1,1,11,2,1,
                 1,4,2,2,1,12,3,1,2,2,
                 1,1,1,1,2,2,4,1,1,2,
                 2,2,2,1,1,3,1,2,1,1};
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
}

void loop() {
  mazeMovements();
}


/*!
 *  \brief Get line state
 *
 *  Read values from every sensor used, compare them to a specific threshold (values are between 0 and 2500). Store the result in booleans.
 *  Then check every possible combinaisons and return a state :
 *  State 0 : One the line
 *  State 1 : A bit shifted on the left but on the line
 *  State 2 : A bit shifted on the right but on the line
 *  State 3 : On an intersection
 *  State 4 : No line found
 *
 *  \return State between 0 and 4
 */
int get_sensor_state() {
    qtr.read(sensorValues);
    left = true; middle_left=true; middle_right =true; right = true;
    if (sensorValues[0] <= threshold) {left = false;}
    if (sensorValues[1] <= threshold) {middle_left = false;}
    if (sensorValues[2] <= threshold) {middle_right = false;}
    if (sensorValues[3] <= threshold) {right = false;}
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

/*!
 *  \brief Naviguate on the line and go to the next intersection
 *
 *  Move forward to the next intersection adjusting constantly the movement to stay on the line.
 *  Starts slower for 400ms to be sure to correct the case where we aren't aligned with the trail.
 *  
 *  \return true if an intersection has been found, false if it's a dead end
 */
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

/*!
 *  \brief Controls the servo responsible for picking up cylinders
 *
 *  Moves the servo by a certain angle
 *  
 *  \param angle the angle you want the servo to move
 */
void moveServo(int angle) {
  int pulseWidth = map(angle, 0, 180, 500, 2400);
  digitalWrite(9, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(9, LOW);
  delay(20);
}

/*!
 *  \brief The picking cylinder function
 *
 *  Initial gate position should be 180°. We then move the gate down to 0° with a small break to be sure that the cylinder
 *  fall properly in the storage. After that we move back the gate to its initial position.
 *  
 */
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

/*!
 *  \brief Controls the robot to exit the maze
 *
 *  This function uses the current position of the robot in the maze and the maze variable. The logic is the following :
 *  We find the next intersection, we compute the relative turn that we need to make in order to go to the exit,
 *  we start to turn, stop when we find the line, update our position in the maze and our global facing.
 *  Relative and global positions/turns may be a bit confusing but for relative turns 1 means left, 2 means right, 3 means forward and 4 means backward
 *  For global positions the logic is : 1 for west, 2 for east, 3 for north and 4 for south 
 *  
 */
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

/*!
 *  \brief Update the robot facing
 *
 *  Update the robot facing, takes relative going direction. Is also used to update the facing after special cases 9, 10 and 11
 *  
 *  \param going represent the relative turns that the robot did
 */
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

/*!
 *  \brief Update the robot position
 *
 *  Update the robot position using its facing variable
 *  
 */
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

/*!
 *  \brief Controls the robot to search through the maze for cylinders
 *
 *  This function explores the maze using the turns[69] array that represents every turns the robot should do. The logic is the following :
 *  Move to the next intersection, check which direction the robot should take, turn in that direction until the robot finds the line, do it again.
 *  Every time we are looking for the next intersection we also check if there is a cylinder in front of us using the button, if so we pick it and keep 
 *  track of the number of cylinders picked. When we pick the third cylinder we enter the exitMaze() function that takes us to the exit of the maze.
 */
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
  dir_tmp +=1;
}






