// This is a test of the robot's motion controls when set ahead of time
// We can check line following, object lifting, etc.
// Useful for debugging motion without comms

#include "mbed.h"
#include "m3pi.h"
#include "Json.h"
#include "MRF24J40.h"
#include <string>
#include "m3pi_grid.h"

DigitalOut A4988STEP(p16);
DigitalOut A4988DIR(p15);

const int total_num_steps = 450;
const float stepper_wait = 0.004;
/**
* Move the object either up or down using the linear actuator 
* driven by the A4988 stepper driver.
* Takes a direction as an input argument and returns the opposite of that
* direction
*/
int move_object (int dir) {
    
    // No inputs but 0 or 1
    if (dir < 0 || dir > 1) {
        return -1;    
    }
    
    A4988DIR = dir;
    A4988STEP = 0;
    int step_cnt = 0;
    
    // Move the stepper all the way up or down
    while (step_cnt < total_num_steps) {
        A4988STEP = 1;
        wait(stepper_wait);
        A4988STEP = 0;
        wait(stepper_wait);
        step_cnt++;
    }
    

    // Return the opposite of direction
    return (1 - dir);
    
}     

// Serial interfaces
// All of these use separate pins
Serial pc(USBTX, USBRX);
m3pi::m3pi m3pi;

int main() {

    pc.baud(9600);

    m3pi.locate(0,1);
    m3pi.printf("Line PID");
    wait_ms(1000);
    m3pi.sensor_auto_calibrate();
    
        // Get to object
        go_forward(0.2);
        go_forward(0.2);
        wait(0.2);
        pivot_left(0.2);
        go_forward(0.2);
        go_forward(0.2);
        
        wait(0.2);
        
        //Pick up the object
        move_object(0);
        wait(0.2);
        
        // Go back to origin
        pivot_left(0.2);
        wait(0.2);
        go_forward(0.2);
        go_forward(0.2);
        wait(0.2);
        pivot_left(0.2);
        wait(0.2);
        go_forward(0.2);
        go_forward(0.2);
        wait(2);
         
        // Go back
        pivot_left(0.2);
        go_forward(0.2);
        go_forward(0.2);
        
        wait(0.2);
        pivot_left(0.2);
        wait(0.2);
         
        go_forward(0.2);
        go_forward(0.2);
        
        wait(0.2);
        pivot_left(0.2);
        wait(0.2);
         
        // Put down
        move_object(1);
        wait(0.2);
         
        // Return
        go_forward(0.2);
        go_forward(0.2);
        
        wait(0.2);
        pivot_left(0.2);
        wait(0.2);
        
        go_forward(0.2);
        go_forward(0.2);
        
  
   //wait(1);
    //}
    
    //int vals[5];

   
           
} 