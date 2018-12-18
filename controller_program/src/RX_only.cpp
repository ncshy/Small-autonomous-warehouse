/**
 * Example code for using a microchip mrf24j40 module to receive only
 * packets using plain 802.15.4
 * Requirements: 3 pins for spi, 3 pins for reset, chip select and interrupt
 * notifications
 * This example file is considered to be in the public domain
 * Originally written by Karl Palsson, karlp@tweak.net.au, March 2011
 */
#include "mrf24j.h"
#include "spidev_transfer.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "DataBlock.h"
#include"JSON.h"
#include"JSONValue.h"
#include"json_handler.h"
#include <stdlib.h>

#define RECV_BUFFER_SIZE 200
#define SEND_BUFFER_SIZE 100
#define DEBUG
struct TrackerObject trackerObj;	
char send_buf[RECV_BUFFER_SIZE];

int main() {
  int shmid;
  //int i = 0;
  void *shared_mem;
  char *json_buf;
  char command[140];
  int action = 0;
  char *id = "m3pi";

  shmid = shmget((key_t)4321, RECV_BUFFER_SIZE, 0666 | IPC_CREAT);

  if(shmid < 0) {
	  perror("msgget failed with error");
	  exit(0);
  }

  shared_mem = shmat(shmid, (void *)0 , 0);

  if (shared_mem == (void *)-1) {
	  printf("shmat failed\n");
	  exit(0);
  }

  json_buf = (char *)shared_mem;

  int fp = open("/dev/ttyS0", 0);
  if (fp < 0)
	perror("file open failed");
char success_buf[SEND_BUFFER_SIZE] = "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":1.0\}";
  
//char buf[20] = "\{Hello, World\}";
  while (1) {
/*
if(action == 10)
	action = 0;
	switch(action) {

		case 0: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":1.0\}");
		break;	
		case 1: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":0.5,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":-1.0\}");
		break;	
		case 2: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":0.5,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":-1.0\}");
		break;	
		case 3: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":0.25,\\\"Stop\\\":-1.0\}");
		break;	
		case 4: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":0.5,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":-1.0\}");
		break;	
		case 5: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":1.0\}");
		break;	
		case 6: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":0.25,\\\"Stop\\\":-1.0\}");
		break;	
		case 7: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":0.5,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":-1.0\}");
		break;	
		case 8: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":1.0\}");
		break;	
		case 9: 
		sprintf(success_buf, "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":1.0\}");
		break;	


		}
action++;
*/
//	printf("bot_location is %f\n", bot_location->TransX);
  //write(fp, buf, sizeof(buf));
  sprintf(command, "sh -c \"echo '%s' > /dev/ttyS0\"", json_buf);
  system(command);
  printf("command is %s\n", command);
  memset(command, 0, sizeof(command));
  sleep(1);
  }
/*  int fd = spi_setup(0, 0); 

  Mrf24j mrf(fd);
  //  mrf.reset();
  mrf.set_pan(0xeafe);
  mrf.init();
  
 // mrf.set_pan(0xcafe);
  // This is _our_ address
  mrf.address16_write(0x6001); 
  mrf.address16_read();
*/
/*
  unsigned int a = mrf.get_pan();
  printf("pan is %x\n", a);
*/
  // uncomment if you want to receive any packet on this channel
  //mrf.set_promiscuous(true);
  
  // uncomment if you want to enable PA/LNA external control
  //mrf.set_palna(true);
/*
  char msg[15] = "Hello World";
  mrf.send16(0xAAAA, msg);
  printf("Message sent\n");
*/  
  // uncomment if you want to buffer all PHY Payload
  //mrf.set_bufferPHY(true);
 /* for(i = 0; i < strlen(msg); i++) {
  mrf.write_long(0xAAAA, msg[i]);
  printf("char %c\n", msg[i]);
  }
*/
//  spi_close(fd);
/*
  while(1) {
			
   printf("Received from shared mem: %s\n", json_buf);
   sleep(1);

  }
*/
  return 0;

}

/*
void loop() {
    mrf.check_flags(&handle_rx, &handle_tx);
}

void handle_rx() {
    Serial.print("received a packet ");Serial.print(mrf.get_rxinfo()->frame_length, DEC);Serial.println(" bytes long");
    
    if(mrf.get_bufferPHY()){
      Serial.println("Packet data (PHY Payload):");
      for (int i = 0; i < mrf.get_rxinfo()->frame_length; i++) {
          Serial.print(mrf.get_rxbuf()[i]);
      }
    }
    
    Serial.println("\r\nASCII data (relevant data):");
    for (int i = 0; i < mrf.rx_datalength(); i++) {
        Serial.write(mrf.get_rxinfo()->rx_data[i]);
    }
    
    Serial.print("\r\nLQI/RSSI=");
    Serial.print(mrf.get_rxinfo()->lqi, DEC);
    Serial.print("/");
    Serial.println(mrf.get_rxinfo()->rssi, DEC);
}

void handle_tx() {
    // code to transmit, nothing to do
}

*/
