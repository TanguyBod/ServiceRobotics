#ifndef CORNER_H
#define CORNER_H

#include <Arduino.h>

class Corner {
  private:
    Corner(int corner_num, int n_corner);

  public :
    int corner_number;
    bool visited;
    int next_corner;
}

#endif 