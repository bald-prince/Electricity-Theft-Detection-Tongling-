#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include "type.h"

int ReportPacketUnpack(char *text);
int OnoffPacketUnpack(char *text, int *cmd, void **arg);
int JsonPack(void);
#endif

