#include "mbed.h"
#include "m3pi.h"
#include "Json.h"
#include "MRF24J40.h"
#include <string>

//MRF24J
// PROJECT
#define SDI p5
#define SDO p6
#define SCK p7
#define CS p13
#define RESET p23

// LAB CONNECTIONS
/*#define SDI p11
#define SDO p12
#define SCK p13
#define CS p7
#define RESET p8*/
// Change this to put robots on different channels
#define ROBO_CHANNEL 0

// Debug mode
#define DEBUG   
#define DEBUG1

// Transmissions to Zigbee
char tx_buffer[128];
char latest_msg[128];

//MRF24J40
PinName mosi(SDI);
PinName miso(SDO);
PinName sck(SCK);
PinName cs(CS);
PinName reset(RESET);

// JSON
const int NUM_JSON_KEYS = 6; 
const char* json_keys[NUM_JSON_KEYS] = {"Backward", "Forward", "ID", "Left", "Right", "Stop"};
      

// Serial interfaces
// All of these use separate pins
Serial pc(USBTX, USBRX);
m3pi m3pi;
MRF24J40 mrf(mosi, miso, sck, cs, reset);
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

// Shared variables and buffer paramters
int msg_len = 0;
const char ping[5] = "ping";

int main() {

    pc.baud(115200);
    mrf.SetChannel(ROBO_CHANNEL);
    
    // JSON parsing variables
    int key_index;
    int json_index;
    float json_val = 0.0;
    while(1) {
        msg_len = rf_receive(latest_msg, 128);
        
        if (msg_len > 0) { 
       
        #ifdef DEBUG 
        pc.printf("Got message\r\n");
        m3pi.locate(0,1);
        m3pi.printf("Msg Rcv");
        pc.printf("%s\r\n",latest_msg);
        #endif
        } else {
            
            #ifdef DEBUG 
            m3pi.locate(0,1);
            m3pi.printf("No Msg");
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
                        
                        if(strcmp(json_keys[i], "Backward") == 0) {
                            if (json_val > -1.0) {
                                pc.printf("m3pi.backward\n");
                                m3pi.backward(0.5);
                            }
                        } else if(strcmp(json_keys[i], "Forward") == 0) {
                            if (json_val > -1.0) {
                            pc.printf("m3pi.forward\n");
                            m3pi.forward(0.5);
                            }
                        } else if(strcmp(json_keys[i], "Left") == 0) {
                            if (json_val > -1.0) {
                            pc.printf("m3pi.left\n");
                            m3pi.left(0.5);
                            }
                        } else if(strcmp(json_keys[i], "Right") == 0) {
                            if (json_val > -1.0) {
                            pc.printf("m3pi.right\n");
                            m3pi.right(0.5);  
                            }
                        } else if(strcmp(json_keys[i], "Stop") == 0) {
                            if (json_val > -1.0) {
                            pc.printf("m3pi.stop\n");
                            m3pi.stop(); 
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
    m3pi.stop();       
} 