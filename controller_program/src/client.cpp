#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<errno.h>
#include<pthread.h>
#include<sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"logger.h"
#include<string.h>
#include<signal.h>
#include "DataBlock.h"

#define DEBUG

using namespace std;
//Add JSON functionality to parse and send messages to minions.

//Global variables for program
int remote_port = 51001; 
int broadcast = 1;
struct sockaddr_in sin;
socklen_t sin_length = sizeof(struct sockaddr_in);
char log_message[100];
int log_to_terminal = 0;
FILE *log_file = NULL;
int shmid;
void *shared_mem;
char *json_str = "{\"velocity\" : \"15\" , \"id\" : 23 , \"name\" : \"m3Pi\"}";

//Add -v to printf logger details to std terminal

void server_close();

//Add SIGINT handler to stop server listen thread.
//Allows for resource deallocation
//Signal SIGINT callback

void sigint_callback(int sig) {
		
	sprintf(log_message, "Inside sigint_callback for sig %d\n", sig);
	logger(log_file, "DEBUG", log_message);
	memset(log_message, 0, sizeof(log_message));
	server_close();
//	server_listen_flag = 0;
	exit(0);

}

//Socket creation
int socket_create() {

   int fd;
   fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

return fd;
}


//Socket connect to name space
int sock_bind(int fd){ 
    int ret;
    sin.sin_family  = AF_INET;
    sin.sin_port = htons(remote_port);
    sin.sin_addr.s_addr = INADDR_ANY;
       
    ret = bind(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
    if (ret < 0) {
    	perror("bind on server_fd failed \n");
    	return -1;
    }
    printf("ret from bind was %d\n", ret);


    return ret;
}


//Initialization thread
int init_connection(int *server_fd_addr) {
	int ret;
//call socket_create
    *server_fd_addr = socket_create();
     if(*server_fd_addr < 0) {
	perror("__LINE__: Socket creation failed \n");
	return -1;
     }

//Bind socket to ip and port.
    ret = sock_bind(*server_fd_addr);
    if(ret < 0) {
	perror("__LINE__: BIND FAILED \n");
	return -1;
    }

   return ret;
}

//close server connection
void server_close() {

//release resources
printf("Detaching shared memory\n");
shmdt(shared_mem);
fclose(log_file);

}

//main function

int main() {

int ret;
int server_fd = -1;
struct HeaderObject header;
struct ItemHeader itemHeader;
struct TrackerObject trackerObj;
struct TrackerObject *bot_location;

//log_file = fopen("/home/ubuntu/experiment_space/server_program/src/log.txt", "w+");
//if(log_file == NULL) {
	log_file = stdout;
//}

signal(SIGINT, &sigint_callback);

shmid = shmget((key_t)1234, sizeof(struct TrackerObject), 0666 | IPC_CREAT);

if(shmid < 0) {
	perror("msgget failed with error");
	exit(0);
}

ret = init_connection(&server_fd);
if (ret < 0) {
	perror("init_connection failed\n");
}

shared_mem = shmat(shmid, (void *)0 , 0);
if (shared_mem == (void *)-1) {
	printf("shmat failed\n");
	exit(0);
}

bot_location = (struct TrackerObject *)shared_mem;
char recv_buf[256];

while(1) {

memset(recv_buf, 0, sizeof(recv_buf));

int received = recvfrom(server_fd, recv_buf, 80, 0, (struct sockaddr *)&sin, &sin_length);
if ( received > 0) {
#ifdef BYTE_DEBUG
	printf("Received %d bytes \n", received);
	printf("Data is : \n");
	for (int i = 0; i < received; i++)
		printf("%02x ", (unsigned char)recv_buf[i]);
	printf("\n");
#endif
	
	memcpy(&header.FrameNumber, recv_buf, 4);
	memcpy(&header.ItemsInBlock, recv_buf + 4, 1);
	memcpy(&itemHeader.ItemID, recv_buf + 5, 1);
	memcpy(&itemHeader.ItemDataSize, recv_buf + 6, 2);
	memcpy(trackerObj.ItemName, recv_buf + 8, 24);
	memcpy(&trackerObj.TransX, recv_buf + 32, 8);
	memcpy(&trackerObj.TransY, recv_buf + 40, 8);
	memcpy(&trackerObj.TransZ, recv_buf + 48, 8);
	memcpy(&trackerObj.RotX, recv_buf + 56, 8);
	memcpy(&trackerObj.RotY, recv_buf + 64, 8);
	memcpy(&trackerObj.RotZ, recv_buf + 72, 8);

	memcpy(bot_location , &trackerObj, 80);
/*	if(msgsnd(msgid, (void *)&bot_location, sizeof(bot_location), 0) == -1) {
		perror("msgsnd failed");
		exit(0);
	}
*/

#ifdef DEBUG
	printf("items in block is :%d\n", header.ItemsInBlock);
	printf("Item Name :%s\n", trackerObj.ItemName);
	printf("Trans X :%lf\n", trackerObj.TransX);
	printf("Trans Y :%lf\n", trackerObj.TransY);
	printf("Trans Z :%lf\n", trackerObj.TransZ);
	printf("Rot X :%lf\n", trackerObj.RotX);
	printf("Rot Y :%lf\n", trackerObj.RotY);
	printf("\n\n");
#endif
} else {
	printf("Receive failed\n");
}

sleep(0.5);
}

server_close();

return 1;
}
