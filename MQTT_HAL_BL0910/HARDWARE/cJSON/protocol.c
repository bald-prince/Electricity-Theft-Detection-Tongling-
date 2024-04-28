#include "protocol.h"
#include "cJSON.h"

#define ORDER	"reportDeviceInfo"

int JsonPack(void) {
	cJSON *root;
	char *out;
	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "order", 5);
	cJSON_AddStringToObject(root, "name", "xuting");
	out = cJSON_Print(root);
	printf("output=%s\r\n", out);
	cJSON_Delete(root);
	root = NULL;
	free(out);
	out = NULL;
	return 0;
}

int ReportPacketUnpack(char *text) {
	int ret = -1;
	cJSON *root = cJSON_Parse(text), *order;
	if (!root) {
		return -2;
	}
	order = cJSON_GetObjectItem(root, "params");
	if (order == NULL) {
		return -3;
	}
	if (memcmp(order->valuestring, ORDER, strlen(ORDER)) == 0) {
		cJSON *temp, *hum, *lng, *lat, *l1, *l2;
		ReportPacket *p = (ReportPacket *)malloc(sizeof(ReportPacket));
		memset(p, 0, sizeof(ReportPacket));
		temp = cJSON_GetObjectItem(root, "WorkSwitch");
		p->temp = temp ? temp->valueint : 0;
	/*	hum = cJSON_GetObjectItem(root, "hum");
		p->hum = hum ? hum->valueint : 0;
		l1 = cJSON_GetObjectItem(root, "l1");
		p->l1 = l1 ? l1->valueint : 0;
		l2 = cJSON_GetObjectItem(root, "l2");
		p->l2 = l2 ? l2->valueint : 0;
		lng = cJSON_GetObjectItem(root, "lng");
		p->lng = lng ? lng->valuedouble : 0.0;
		lat = cJSON_GetObjectItem(root, "lat");
		p->lat = lat ? lat->valuedouble : 0.0;
        */
	
	}	
	cJSON_Delete(root);
	return ret;
}


int OnoffPacketUnpack(char *text, int *cmd, void **arg) {
	int ret = -1;
	cJSON *root = cJSON_Parse(text), *username;
	if (!root) {
		return -2;
	}
	username = cJSON_GetObjectItem(root, "username");
	if (username == NULL) {
		cJSON_Delete(root);
		return -3;
	}
	if (memcmp(username->valuestring, "admin", strlen("admin")) == 0) {
		cJSON *clientid, *ts;
		OnoffPacket *p = (OnoffPacket *)malloc(sizeof(OnoffPacket));
		memset(p, 0, sizeof(OnoffPacket));
		clientid = cJSON_GetObjectItem(root, "clientid");
		ts = cJSON_GetObjectItem(root, "ts");
		if (clientid == NULL || ts == NULL) {
			cJSON_Delete(root);
			return -4;
		}
		memcpy(p->clientid, clientid->valuestring, strlen(clientid->valuestring));
		p->ts = ts->valueint;
		*arg = (void *)p;
		*cmd = 1;
		ret = 0;
	}
	cJSON_Delete(root);
	return ret;
}

