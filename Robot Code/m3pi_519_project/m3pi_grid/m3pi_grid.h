#ifndef M3PI_GRID_H
#define M3PI_GRID_H

#include "m3pi.h"

// Line detection
#define SENSOR_ON_THRESH 1500

// Minimum and maximum motor speeds
#define MAX 1.0
#define MIN 0

// PID terms
#define P_TERM 0.1
#define I_TERM 0.001
#define D_TERM 0

extern m3pi grid_m3pi;

// Reads the raw light sensor data and returns
// them as an array of either 1 or 0, depending on whether the sensor is
// over the line
void read_raw_sensors(int vals[5]);

// Counts the number of line sensors which are currently sensing darkness,
// which means they are over a line and returns them
int sensors_over_line(void);

// Turn right, but trying to turn the robot in an arc 
// instead of rotating in place
void pivot_left(float speed);

// Turn right, but trying to turn the robot in an arc 
// instead of rotating in place
void pivot_right(float speed);

// Go forward with PID control until the robot reaches a junction or cross
// in the grid
void go_forward(float speed);




#endif