#include "mgos.h"
#include "blynk.h"

struct mg_connection *blynk_conn = NULL;
int blynk_reconnect_interval = 3000;

static blynk_handler_t blynk_recv_handler = NULL;
static int blynk_ping_interval = 2;

void blynk_send(struct mg_connection *c, uint8_t type, uint16_t id, const void *data, uint16_t len)
{
    static uint16_t cnt;
    uint8_t header[BLYNK_HEADER_SIZE];

    if (id == 0)
        id = ++cnt;

    LOG(LL_DEBUG, ("BLYNK SEND type %hhu, id %hu, len %hu", type, id, len));
    header[0] = type;
    header[1] = (id >> 8) & 0xff;
    header[2] = id & 0xff;
    header[3] = (len >> 8) & 0xff;
    header[4] = len & 0xff;

    mg_send(c, header, sizeof(header));
    mg_send(c, data, len);
}

void blynk_recv(struct mg_connection *c, uint8_t type, uint16_t id, const void *data, uint16_t len)
{
    LOG(LL_DEBUG, ("BLYNK RECV: type %hhu, id %hu, len %hu", type, id, len));

    if (id == 0)
    {
        c->flags |= MG_F_CLOSE_IMMEDIATELY;
        return;
    }
    switch (type)
    {
    case BLYNK_RESPONSE:
        if (len == 200)
            mg_set_timer(c, mg_time() + blynk_ping_interval);
        else
        {
            LOG(LL_ERROR, ("BLYNK ERROR CODE %hu", len));
            c->flags |= MG_F_CLOSE_IMMEDIATELY;
        }
    case BLYNK_HARDWARE:
        (void)data;
        break;
    }
}

static void blynk_ev_handler(struct mg_connection *c, int ev, void *ev_data __attribute__((unused)), void *user_data __attribute__((unused)))
{
    switch (ev)
    {
    case MG_EV_CONNECT:
        LOG(LL_DEBUG, ("BLYNK CONNECT"));
        blynk_send(c, BLYNK_LOGIN, 1, mgos_sys_config_get_blynk_auth(), strlen(mgos_sys_config_get_blynk_auth()));
        break;
    case MG_EV_RECV:
        while (c->recv_mbuf.len >= BLYNK_HEADER_SIZE)
        {
            const uint8_t *buf = (const uint8_t *)c->recv_mbuf.buf;
            uint8_t type = buf[0];
            uint16_t id = buf[1] << 8 | buf[2];
            uint16_t len = buf[3] << 8 | buf[4];

            if (c->recv_mbuf.len < (size_t)BLYNK_HEADER_SIZE + (type == BLYNK_RESPONSE ? 0 : len))
                break;

            blynk_recv(c, type, id, (type == BLYNK_RESPONSE ? 0 : (const void *)(buf + BLYNK_HEADER_SIZE)), len);
            mbuf_remove(&c->recv_mbuf, BLYNK_HEADER_SIZE + (type == BLYNK_RESPONSE ? 0 : len));
        }
        break;
    case MG_EV_CLOSE:
        LOG(LL_DEBUG, ("BLYNK DISCONNECT"));
        blynk_conn = NULL;
        break;
    case MG_EV_TIMER:
        LOG(LL_DEBUG, ("BLYNK PING"));
        blynk_send(c, BLYNK_PING, 0, NULL, 0);
        break;
    }
}

static void blynk_reconnect_cb(void *arg)
{
    if (!mgos_sys_config_get_blynk_enable() ||
        mgos_sys_config_get_blynk_server() == NULL ||
        mgos_sys_config_get_blynk_auth() == NULL || blynk_conn != NULL)
        return;
    blynk_conn = mg_connect(mgos_get_mgr(), mgos_sys_config_get_blynk_server(), blynk_ev_handler, arg);
}

void blynk_connect(blynk_handler_t handler)
{
    blynk_recv_handler = handler;
    mgos_set_timer(blynk_reconnect_interval, MGOS_TIMER_REPEAT, blynk_reconnect_cb, NULL);
}