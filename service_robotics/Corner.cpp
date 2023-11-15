#include <Corner.h>

Corner::Corner(int corner_num, int n_corner) {
    corner_number = corner_num;
    visited = false;
    next_corner = n_corner;
}