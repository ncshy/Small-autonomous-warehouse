#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<errno.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"logger.h"
#include<string.h>
#include<signal.h>
#include"JSON.h"
#include"JSONValue.h"
#include"json_handler.h"

#define MAX_CONNECTIONS 10
//#define HOST_IP "192.168.43.138"
#define HOST_IP "192.168.0.10"
#define SEND_BUFFER_SIZE 200	//bytes
#define RECV_BUFFER_SIZE 500	//bytes

using namespace std;
//Add JSON functionality to parse and send messages to minions.

//Global variables for program
int server_port = 8087; 
int success_buf_flag = 1;
int max_connections = MAX_CONNECTIONS;
struct sockaddr_in sin;
struct sockaddr_in client_sockaddr[MAX_CONNECTIONS];
char log_message[100];
int log_to_terminal = 0;
int server_listen_flag = 1;
int bytes_read[MAX_CONNECTIONS];	
int send_ret[MAX_CONNECTIONS];
char success_buf[SEND_BUFFER_SIZE] = "\{ \"Backward\":-1.0,\"Forward\":-1.0,\"ID\":109,\"Left\":-1.0,\"Right\":-1.0,\"Stop\":1.0\}";
FILE *log_file = NULL;
char *json_str = "{\"velocity\" : \"15\" , \"id\" : 23 , \"name\" : \"m3Pi\"}";

//Add -v to printf logger details to std terminal

//Thread definitions
pthread_t recv_thread[MAX_CONNECTIONS];
pthread_t send_thread[MAX_CONNECTIONS];
pthread_t bot_thread[MAX_CONNECTIONS];
void server_close();

//Add SIGINT handler to stop server listen thread.
//Allows for resource deallocation
//Signal SIGINT callback

void sigint_callback(int sig) {
		
	sprintf(log_message, "Inside sigint_callback for sig %d\n", sig);
	logger(log_file, "DEBUG", log_message);
	memset(log_message, 0, sizeof(log_message));
	server_listen_flag = 0;
	exit(0);

}

//Socket creation
int socket_create() {

   int fd;
   fd = socket(AF_INET, SOCK_STREAM, 0);

return fd;
}

//Socket bind to name space
int sock_bind(int fd){ 
    int ret;
    sin.sin_family  = AF_INET;
    sin.sin_port = htons(server_port);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

/*    ret = inet_aton(HOST_IP, &sin.sin_addr);
 
    if(ret < 0) {
	sprintf(log_message, "Supplied IP string not accepted by inet_aton\n");
	logger(log_file, "ERROR", log_message);
	memset(log_message, 0, sizeof(log_message));
	return -2;
    }
  */
 
    ret = bind(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));

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
  return 0;

}

//close server connection
void server_close() {

int i;
//release resources
for (i = 0 ; i < MAX_CONNECTIONS; i++) {
    pthread_cancel(recv_thread[i]);
    pthread_cancel(recv_thread[i]);
}
fclose(log_file);

}

void *bot_control_thread(void *arg) {

	int client_fd = *(int *)arg;
	int action = 0;	
	char id[10] = "m3pi1";

	while(1) {
		if(send_ret[client_fd] <= 0) {
			sleep(1);
			continue;
		} else {
			switch(action) {

				case 0: 
					json_create(success_buf, id, 0.5, -1.0, -1.0, -1.0, -1.0);
					break;
				case 1: 	
					json_create(success_buf, id, 0.5, -1.0, -1.0, -1.0, -1.0);
					break;	
				case 2:
					json_create(success_buf, id, -1.0, -1.0, -1.0, 0.25, -1.0);
					break;
				case 3:	
					json_create(success_buf, id, 0.5, -1.0, -1.0, -1.0, -1.0);
					break;
				case 4:	
					json_create(success_buf, id, -1.0, -1.0, -1.0, -1.0, 1.0);
					break;
				case 5:
					json_create(success_buf, id, -1.0, 0.5, -1.0, -1.0, -1.0);
					break;	
				case 6:	
					json_create(success_buf, id, -1.0, -1.0, -1.0, 0.25, -1.0);
					break;
				case 7:	
					json_create(success_buf, id, 0.5, -1.0, -1.0, -1.0, -1.0);
					break;	
				case 8:	
					json_create(success_buf, id, 0.5, -1.0, -1.0, -1.0, -1.0);
					break;	
				default: 
					json_create(success_buf, id, -1.0, -1.0, -1.0, -1.0, 1.0);
					break;
			}	
			action++;
			success_buf_flag = 1;
			sleep(1);
		}

	}	

}

//recv thread callback
void *recv_thread_func(void *arg) {
	char read_buf[RECV_BUFFER_SIZE];
	int client_fd = *(int *)arg;
	struct timeval tv;

	tv.tv_sec = 20;
	tv.tv_usec = 0;
	
	//Consider how multi-threading can affect the logger
	sprintf(log_message, "Inside recv_thread_func\n");
	logger(log_file, "DEBUG", log_message);
	memset(log_message, 0, sizeof(log_message));
	

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(client_fd, &readfds);

//How to know client has lost connection. How to exit this thread?	
   while(1) {

	select(client_fd + 1, &readfds, NULL, NULL, &tv);	
	
	bytes_read[client_fd] = recv(client_fd, read_buf, sizeof(read_buf), MSG_DONTWAIT);	
	
	if(bytes_read[client_fd] < 0) {
		sprintf(log_message, "%d : Thread failed to read", __LINE__);
		logger(log_file, "CONNECTION LOST", log_message);
		memset(log_message, 0, sizeof(log_message));
		break;
	} else if(bytes_read[client_fd] == 0) {
		sprintf(log_message, "%d : Connection to client closed", __LINE__);
		logger(log_file, "CONNECTION LOST", log_message);
		memset(log_message, 0, sizeof(log_message));
		break;
	} else	{
		sprintf(log_message, "Bytes read in recv is %d\n", bytes_read[client_fd]);
		logger(log_file, "DEBUG", log_message);
		memset(log_message, 0, sizeof(log_message));	
	}
	tv.tv_sec = 20;
	tv.tv_usec = 0;

//	sprintf(log_message, "Read Buffer contains %s\n", read_buf);
//	logger(log_file, "DEBUG", log_message);
//	memset(log_message, 0, sizeof(log_message));

   }

sprintf(log_message, "EXITING RECV THREAD \n");
logger(log_file, "DEBUG", log_message);
memset(log_message, 0, sizeof(log_message));
pthread_cancel(bot_thread[client_fd]);
pthread_cancel(send_thread[client_fd]);
close(client_fd);
pthread_exit(NULL);

}


//send thread callback

void *send_thread_func(void *arg) {
	
	int client_fd = *(int *)arg;
	wstring wstrValue;
	string strValue;

	sprintf(log_message, "Inside send_thread_func\n");
	logger(log_file, "DEBUG", log_message);
	memset(log_message, 0, sizeof(log_message));
/*	sprintf(success_buf, "HTTP/1.1 200 OK\r\n \
	server: nginx\r\n \
	date: Sun, 25 Nov 2018 00:42:38 GMT\r\n \
	content-type: text/html; charset=UTF-8\r\n\r\n");
*/	
	while(1) {
		if(bytes_read[client_fd] > 0 && success_buf_flag == 1) {
			send_ret[client_fd] = send(client_fd, success_buf, sizeof(success_buf), 0);
			bytes_read[client_fd] = 0;
			if(send_ret[client_fd] < 0) {
				perror("send thread FAILURE:");
			} else if (send_ret[client_fd] == 0) {
				break;
			} else {
				success_buf_flag = 0;
				sleep(1);
				send_ret[client_fd] = 0;
			}
		}
		sleep(1);
	}

	sprintf(log_message, "EXITING SEND THREAD \n");
	logger(log_file, "DEBUG", log_message);
	memset(log_message, 0, sizeof(log_message));
	pthread_cancel(bot_thread[client_fd]);
	pthread_cancel(recv_thread[client_fd]);
	close(client_fd);
	pthread_exit(NULL);
}

//main function

int main() {

int ret;
int server_fd = -1;
int client_count = 0;
int client_fd;
socklen_t sock_len = sizeof(struct sockaddr_in);

//log_file = fopen("/home/ubuntu/experiment_space/server_program/src/log.txt", "w+");
//if(log_file == NULL) {
	log_file = stdout;
//}

signal(SIGINT, &sigint_callback);

//printf("Parsing JSON string\n");
//json_parser(json_str);
//json_create();

ret = init_connection(&server_fd);

if (ret < 0)
	return -1;

//listen on server port for incoming connections
//Queue connections upto MAX_CONNECTIONS value

sprintf(log_message, "Server listening for incoming connections on port %d\n", server_port);
logger(log_file, "INFO", log_message);
memset(log_message, 0, sizeof(log_message));

ret = listen(server_fd, MAX_CONNECTIONS);
if (ret < 0) {
    perror("listen on server_fd failed \n");
    return -1;
}

//Maintain a while 1 loop, acting as server listening/accept thread
while(1) {

//**Please check the multiple client case, how to reuse pthread_t variable**
//If connection less than max_connection, service the new incoming connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_sockaddr[client_count], &sock_len);

	if (client_fd < 0) {
		perror("accept call failed __LINE__ \n");
 		return -1;
	} else {
		sprintf(log_message, "Client %d\n successfully connected to server with ip %s\n", client_fd, inet_ntoa(client_sockaddr[client_count].sin_addr));

		logger(log_file, "INFO", log_message);
		memset(log_message, 0, sizeof(log_message));
		ret = pthread_create(&bot_thread[client_fd], NULL, &bot_control_thread, &client_fd);
		ret = pthread_create(&recv_thread[client_fd], NULL, &recv_thread_func, &client_fd);
		ret = pthread_create(&send_thread[client_fd], NULL, &send_thread_func, &client_fd);
		if(ret < 0) {
			perror("Thread creation failed __LINE__\n");
		}
	}
}


server_close();

return 1;
}
