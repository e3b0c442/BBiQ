#ifndef BLYNK_E3B0C442_H
#define BLYNK_E3B0C442_H

#include "mgos.h"

#define BLYNK_HEADER_SIZE 5

/* commands */
#define BLYNK_RESPONSE 0
#define BLYNK_LOGIN 2
#define BLYNK_PING 6
#define BLYNK_BRIDGE 15
#define BLYNK_HARDWARE 20

typedef void (*blynk_handler_t)(struct mg_connection *, const char *cmd, int pin, int val, int id, void *user_data);

extern struct mg_connection *blynk_conn;
extern int blynk_reconnect_interval;

void blynk_connect(blynk_handler_t handler);
void blynk_send(struct mg_connection *c, uint8_t cmd, uint16_t id, const void *data, uint16_t len);

#endif /* BLYNK_E3B0C442_H */