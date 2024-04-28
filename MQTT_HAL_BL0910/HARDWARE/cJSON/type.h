#ifndef __TYPE_H
#define __TYPE_H
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG	1
#if (DEBUG == 1)
	#define debug(format, ...) printf(format, ##__VA_ARGS__)
#else
	#define debug(format, ...) 
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
//typedef unsigned long uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
//typedef signed long int32_t;

typedef struct {
	char *topic;
	int qos;
	int valid;
} TopicFilter;

typedef struct {
	char imei[16];
	int temp;
	int hum;
	int l1;
	int l2;
	float lng;
	float lat;
} ReportPacket;

typedef struct {
	char clientid[64];
	int ts;
	int onoff;
} OnoffPacket;

typedef struct {
	int port;
	int keepalive;
	char domain[32];
	char username[32];
	char userpass[32];
	char clientid[32];
} Server;

#endif
