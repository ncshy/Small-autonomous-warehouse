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

#define MAX_CONNECTIONS 10
#define SEND_BUFFER_SIZE 100


char stop_buf[SEND_BUFFER_SIZE] = "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":1.0\}";
char forward_buf[SEND_BUFFER_SIZE] =  "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":0.5,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":-1.0\}";
char right_buf[SEND_BUFFER_SIZE] = "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":0.25,\\\"Stop\\\":-1.0\}";
char left_buf[SEND_BUFFER_SIZE] = "\{\\\"Backward\\\":-1.0,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":0.5,\\\"Right\\\":-1.0,\\\"Stop\\\":-1.0\}";
char backward_buf[SEND_BUFFER_SIZE] = "\{\\\"Backward\\\":0.5,\\\"Forward\\\":-1.0,\\\"ID\\\":109,\\\"Left\\\":-1.0,\\\"Right\\\":-1.0,\\\"Stop\\\":-1.0\}";


using namespace std;
//Add JSON functionality to parse and send messages to minions.

void json_parser(char *json_str) {

	JSONValue *value = JSON::Parse(json_str);
	
	if (value == NULL) {
		printf("JSON String couldn't be parsed\n");
	} else {
		JSONObject root;
		if(value->IsObject() == false) {
			printf("Root element not an Object\n");
		} else {
			root = value->AsObject();
			if(root.find(L"velocity") != root.end() && root[L"velocity"]->IsString()) {
				printf("Velocity: ");
				wcout << root[L"velocity"]->AsString().c_str();
				printf("\r\n");
			}
			if(root.find(L"id") != root.end() && root[L"id"]->IsNumber()) {
				printf("ID: ");
				wcout << root[L"id"]->AsNumber();
				printf("\r\n");
			}
			if(root.find(L"name") != root.end() && root[L"name"]->IsString()) {
				printf("Name: ");
				wcout << root[L"name"]->AsString().c_str();
				printf("\r\n");
			}
		}
	}
	delete value;	
}

void json_create(char *buf, char *id, double fwd, double backward, double left, double right, double stop ) {

	JSONObject root;
	wstring wstrValue;
	string strValue;

/*	root[L"ID"] = new JSONValue(*id);
	root[L"Forward"] = new JSONValue((double)fwd);
	root[L"Backward"] = new JSONValue((double)backward);
	root[L"Left"] = new JSONValue((double)left);
	root[L"Right"] = new JSONValue((double)right);
	root[L"Stop"] = new JSONValue((double)stop);	
	JSONValue *value = new JSONValue(root);
	
	wstrValue = value->Stringify();
	strValue.assign(wstrValue.begin(), wstrValue.end());
*/
//char success_buf[100];
//sprintf(success_buf, "\{\\\"Backward\\\":%f,\\\"Forward\\\":%f,\\\"ID\\\":%d,\\\"Left\\\":%f,\\\"Right\\\":%f,\\\"Stop\\\":%f\}", backward, fwd, *id, left, right, stop);


/*	buf = strValue.c_str();
	#ifdef DEBUG
	printf("strValue.length is %d\n", strValue.length());	
	#endif
*/

	memset(buf, 0, sizeof(buf));

	if(fwd > 0.0) {
		memcpy(buf, forward_buf, SEND_BUFFER_SIZE);
	} else if(backward > 0.0) {
		memcpy(buf, backward_buf, SEND_BUFFER_SIZE);
	
	} else if(left > 0.0) {
		memcpy(buf, left_buf, SEND_BUFFER_SIZE);

	} else if (right > 0.0) {
		memcpy(buf, right_buf, SEND_BUFFER_SIZE);

	} else if (stop > 0.0) {
		memcpy(buf, stop_buf, SEND_BUFFER_SIZE);

	} else {
		//Do nothing
	}
//	sprintf(buf, strValue.c_str(), strValue.length());	
	#ifdef DEBUG
	printf("JSON is %s \n", buf);
	#endif

}
