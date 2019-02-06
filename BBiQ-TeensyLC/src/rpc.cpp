#include <Arduino.h>
#include <ArduinoJson.h>

#include "event.hpp"
#include "rpc.hpp"
#include "serial.hpp"

namespace
{
StaticJsonBuffer<UART_BUFFER_SIZE * 2> jsonBuf;

void handler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::UART:
    {
        SerialEvent *se = (SerialEvent *)e;
        JsonObject &call = jsonBuf.parseObject((const char *)se->buffer);
        if (call.success())
            Serial.printf("DEBUG: [%10u] RPC VALID JSON ID %d\n", e->ts, call.get<int>("id"));
        else
            Serial.printf("DEBUG: [%10u] RPC INVALID JSON\n", e->ts);
        jsonBuf.clear();
        break;
    }
    default:;
    }
}

} // namespace

void rpcSetup()
{
    registerHandler(Event::Type::UART, &handler);
}