#include "m3pi_grid.h"

m3pi::m3pi grid_m3pi;

void read_raw_sensors(int vals[5]) {
    // Get values of all 5 line sensors
    grid_m3pi.putc(SEND_RAW_SENSOR_VALUES);
    vals[0] = grid_m3pi.getc();
    vals[0] += grid_m3pi.getc() << 8;
    vals[1] = grid_m3pi.getc();
    vals[1] += grid_m3pi.getc() << 8;
    vals[2] = grid_m3pi.getc();
    vals[2] += grid_m3pi.getc() << 8;
    vals[3] = grid_m3pi.getc();
    vals[3] += grid_m3pi.getc() << 8;
    vals[4] = grid_m3pi.getc();
    vals[4] += grid_m3pi.getc() << 8;
    
    // If they are getting a high enough value, count them as on
    for (int i = 0;i < 5;i++) {
        if (vals[i] > SENSOR_ON_THRESH) {
            vals[i] = 1;
        } else {
            vals[i] = 0;
        }
    }
}

int sensors_over_line()
{
    int vals[5];
    int cnt = 0;
    
    read_raw_sensors(vals);
    for (int i = 0; i < 5; i++) {
        if (vals[i] == 1)
            cnt++;
    }
    
    return cnt;
}

void pivot_right(float speed) {
    float line_pos = 1.0;
    grid_m3pi.right_motor(speed);
    grid_m3pi.left_motor(speed*-1);
    wait(0.2);
    // Who needs encoders when you can guess instead?
    while (line_pos > 0.1 || line_pos < -0.1)
        line_pos = grid_m3pi.line_position();
    grid_m3pi.stop();
    
}

void pivot_left(float speed) {
    float line_pos = 1.0;
    grid_m3pi.left_motor(speed);
    grid_m3pi.right_motor(speed*-1);
    wait(0.2);
     while (line_pos > 0.1 || line_pos < -0.1)
        line_pos = grid_m3pi.line_position();
    grid_m3pi.stop();
}

void go_forward(float init_speed) {
    
    float right;
    float left;
    float current_pos_of_line = 0.0;
    float previous_pos_of_line = 0.0;
    float derivative,proportional,integral = 0;
    float power;
    float speed = init_speed;
    
    // If you are seeing 3 or more sensors over a line, you have likely hit
    // the end of the grid line.
    while (sensors_over_line() < 3) {
    //while (1){
        // Get the position of the line.
        current_pos_of_line = grid_m3pi.line_position();
        proportional = current_pos_of_line;

        // Compute the derivative
        derivative = current_pos_of_line - previous_pos_of_line;

        // Compute the integral
        integral += proportional;

        // Remember the last position.
        previous_pos_of_line = current_pos_of_line;

        // Compute the power
        power = (proportional * (P_TERM) ) + (integral*(I_TERM)) + (derivative*(D_TERM)) ;

        // Compute new speeds
        right = speed+power;
        left  = speed-power;

        // limit checks
        if (right < MIN)
            right = MIN;
        else if (right > MAX)
            right = MAX;

        if (left < MIN)
            left = MIN;
        else if (left > MAX)
            left = MAX;

        // set speed
        grid_m3pi.left_motor(left);
        grid_m3pi.right_motor(right);

    }
    
    // Scoot over the vertex
    wait(0.15);
    // This is for safety right now.
    // If we get to a point where comms are syncronized really well, 
    // we may not need this
    grid_m3pi.stop();
}

