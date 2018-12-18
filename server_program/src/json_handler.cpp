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
	root[L"ID"] = new JSONValue(*id);
	root[L"Forward"] = new JSONValue((double)fwd);
	root[L"Backward"] = new JSONValue((double)backward);
	root[L"Left"] = new JSONValue((double)left);
	root[L"Right"] = new JSONValue((double)right);
	root[L"Stop"] = new JSONValue((double)stop);
	
	JSONValue *value = new JSONValue(root);
	
	wstrValue = value->Stringify();
	strValue.assign(wstrValue.begin(), wstrValue.end());

//	buf = strValue.c_str();
	printf("strValue.length is %d\n", strValue.length());	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, strValue.c_str(), strValue.length());	
	printf("JSON is %s \n", buf);

}
