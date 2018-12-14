#include "mbed.h"
#include "m3pi.h"
#include "Json.h"
#include "MRF24J40.h"
#include <string>
#include "m3pi_grid.h"

//MRF24J
#define SDI p5
#define SDO p6
#define SCK p7
#define CS p13
#define RESET p23

// Change this to put robots on different channels
#define ROBO_CHANNEL 0

// Stepper motor pins
DigitalOut A4988STEP(p16);
DigitalOut A4988DIR(p15);

// Directions for object
#define UP 1
#define DOWN 2

// Stepper controls
const float stepper_wait = 0.004;
const int total_num_steps = 400;

// Debug mode
#define DEBUG   
#define DEBUG1

// Transmissions to Zigbee
char tx_buffer[128];
char latest_msg[128];
int msg_len = 0;

//MRF24J40
PinName mosi(SDI);
PinName miso(SDO);
PinName sck(SCK);
PinName cs(CS);
PinName reset(RESET);

// JSON
const int NUM_JSON_KEYS = 7; 
const char* json_keys[NUM_JSON_KEYS] = {"Backward", "Forward", "ID", "Left", "Right", "Stop", "Object"};
      

// Serial interfaces
// All of these use separate pins
Serial pc(USBTX, USBRX);
m3pi::m3pi m3pi;
MRF24J40 mrf(mosi, miso, sck, cs, reset);


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
    }
    

    // Return the opposite of direction
    return (1 - dir);
    
}


/**
* Receive data from the MRF24J40.
*
* @param data A pointer to a char array to hold the data
* @param maxLength The max amount of data to read.
*/
int rf_receive(char *data, uint8_t maxLength)
{
    uint8_t len = mrf.Receive((uint8_t *)data, maxLength);
    uint8_t header[8]= {1, 8, 0, 0xA1, 0xB2, 0xC3, 0xD4, 0x00};

    if(len > 10) {
        //Remove the header and footer of the message
        for(uint8_t i = 0; i < len-2; i++) {
            if(i<8) {
                //Make sure our header is valid first
                if(data[i] != header[i])
                    return 0;
            } else {
                data[i-8] = data[i];
            }
        }
        //pc.printf("Received: %s length:%d\r\n", data, ((int)len)-10);
    }
    return ((int)len)-10;
}

/**
* Send data to another MRF24J40.
*
* @param data The string to send
* @param maxLength The length of the data to send.
*                  If you are sending a null-terminated string you can pass strlen(data)+1
*/
void rf_send(char *data, uint8_t len)
{
    //We need to prepend the message with a valid ZigBee header
    uint8_t header[8]= {1, 8, 0, 0xA1, 0xB2, 0xC3, 0xD4, 0x00};
    uint8_t *send_buf = (uint8_t *) malloc( sizeof(uint8_t) * (len+8) );

    for(uint8_t i = 0; i < len+8; i++) {
        //prepend the 8-byte header
        send_buf[i] = (i<8) ? header[i] : data[i-8];
    }
    //pc.printf("Sent: %s\r\n", send_buf+8);

    mrf.Send(send_buf, len+8);
    free(send_buf);
}

int main() {

    pc.baud(9600);
    
    // Assuming you started with the inear actuator retracted
    int object_dir = UP;
    mrf.SetChannel(ROBO_CHANNEL);
    
    // JSON parsing variables
    int key_index;
    int json_index;
    float json_val = 0.0;
    pc.printf("Start\r\n");
    m3pi.sensor_auto_calibrate();
    
    // Robot's just gonna go forever
    while (1) {
        pc.printf("Loop\r\n");
        msg_len = rf_receive(latest_msg, 128);
        
        if (msg_len > 0) { 
        #ifdef DEBUG 
        pc.printf("Got message\r\n");
        m3pi.locate(0,0);
        m3pi.printf("Msg Rcv");
        pc.printf("%s\r\n",latest_msg);
        #endif
        } else {
            #ifdef DEBUG 
            m3pi.locate(0,0);
            m3pi.printf("Msg Rcv");
            pc.printf("No message :(\r\n");
            #endif
        } 
       
       wait(0.3);
        
        Json json(latest_msg, msg_len);
         
        if (json.isValidJson()) {
            //TODO - check your id
            for (int i = 0; i < NUM_JSON_KEYS; i++) {         
                if ((key_index = json.findKeyIndexIn(json_keys[i], 0)) >= 0) {
                    #ifdef DEBUG
                    pc.printf("Found JSON key %s at key index %d \n\r", json_keys[i], key_index);
                    #endif
                    
                    // Get the numerical value for the command and then
                    // send a command to the robot
                    json_index = json.findChildIndexOf(key_index, -1);
                    json.tokenNumberValue(json_index, json_val);
                    #ifdef DEBUG
                    pc.printf("JSON index is %d\n", json_index);
                    pc.printf("JSON value for %s is %f\n\r",json_keys[i],json_val);
                    #endif
                    
                    // LCD used for debugging
                    m3pi.cls();
                    m3pi.locate(0,1);   
                    
                    // Backward needs a little intellegence that the controller
                    // does not have yet. Pls do not... 
                    if(strcmp(json_keys[i], "Backward") == 0) {
                        if (json_val > -1.0) {
                            m3pi.printf("BACKWARD");
                            //m3pi.backward(0.5);
                        }
                    } else if(strcmp(json_keys[i], "Forward") == 0) {
                        if (json_val > -1.0) {
                            m3pi.printf("FORWARD");
                            go_forward(0.5);
                        }
                    } else if(strcmp(json_keys[i], "Left") == 0) {
                        if (json_val > -1.0) {
                            m3pi.printf("LEFT");
                            pivot_left(0.2);
                        }
                    } else if(strcmp(json_keys[i], "Right") == 0) {
                        if (json_val > -1.0) {
                            m3pi.printf("RIGHT");
                            pivot_right(0.2);
                        }   
                    } else if(strcmp(json_keys[i], "Stop") == 0) {
                        if (json_val > -1.0) {
                            m3pi.printf("STOP");
                            m3pi.stop();
                        }                       
                    }  else if (strcmp(json_keys[i], "Object") == 0) {
                        if (json_val > -1.0) {
                            m3pi.printf("OBJ");
                            
                            // Switch direction every tie you call this so
                            // call is always up->down, never up->up
                            // r down->down
                            object_dir = move_object(object_dir);
                        }
                    }
                } 
                
            }
              
        } else {
            #ifdef DEBUG
            pc.printf("Got invalid JSON message!\n\r");
            #endif       
        } 
    }
    // Should never get here     
    m3pi.stop();       
    m3pi.printf("OOPS");
} 