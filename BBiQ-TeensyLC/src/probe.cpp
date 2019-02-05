#include <Arduino.h>
#include <ADC.h>
#include "button.hpp"
#include "event.hpp"
#include "mode.hpp"
#include "pins.hpp"
#include "probe.hpp"

namespace
{
//ADC library
ADC *adc = new ADC();

// Steinhart-Hart constants for Thermoworks probes
const float STEIN_A = 7.343140544e-4;
const float STEIN_B = 2.157437229e-4;
const float STEIN_C = 9.51568577e-8;
const uint8_t ADC_BITS = 10;
const uint16_t ADC_RES = pow(2, ADC_BITS);
const uint16_t ADC_LOW_THRESHOLD = ADC_RES / 25;
const uint16_t ADC_HIGH_THRESHOLD = ADC_RES - ADC_RES / 25;
const float STATIC_R = 10000.0;

// Loop timers
const uint32_t PROBES_POWERON_DELAY = 1;
const uint32_t PROBES_READ_INTERVAL = 1000;

uint32_t poweredOnTime = 0;
uint32_t lastReadTime = 0;

// Probe count and default names
const char PROBE_DEFAULT_NAMES[(uint8_t)Probe::ID::COUNT][16] = {
    "Pit",
    "Food 1",
    "Food 2",
    //"Food 3",
};

const Pin PROBE_PINS[] = {
    Pin::PROBE_1,
    Pin::PROBE_2,
    Pin::PROBE_3,
    //Pin::PROBE_4,
};

RunMode currentMode;
Probe probes[(uint8_t)Probe::ID::COUNT];

uint32_t
powerOnProbes()
{
    digitalWrite((uint8_t)Pin::PROBE_POWER, HIGH);
    return millis();
}

void powerOffProbes()
{
    digitalWrite((uint8_t)Pin::PROBE_POWER, LOW);
}

uint32_t readProbes()
{
    for (uint8_t i = 0; i < (uint8_t)Probe::ID::COUNT; i++)
        probes[i].read();

    return millis();
}

void handler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::MODE:
    {
        ModeEvent *me = (ModeEvent *)e;
        currentMode = me->mode;
        break;
    }
    case Event::Type::RESET:
        digitalWrite((uint8_t)Pin::PROBE_POWER, LOW);
        poweredOnTime = e->ts;
        lastReadTime = e->ts;

        for (uint8_t i = 0; i < (uint8_t)Probe::ID::COUNT; i++)
        {
            strcpy(probes[i].name, PROBE_DEFAULT_NAMES[i]);
            probes[i].connected = false;
            probes[i].lowAlarm = 32.0;
            probes[i].highAlarm = 500.0;
            probes[i].arm = false;
            pinMode((uint8_t)probes[i].pin, INPUT);
            dispatch(new ProbeEvent(&probes[i], ProbeEvent::Action::FIELD_CHANGE));
        }
    default:;
    }
}

/*void handler(Event *e)
{
    LocalInputEvent *li = (LocalInputEvent *)e;
    if (li->delta == 0)
    {
        return;
    }

    Probe *p = &probes[li->probe];
    switch (li->field)
    {
    case PROBE_FIELD_LOW:
        if (p->lowAlarm % li->delta)
        {
            p->lowAlarm = ((int)p->lowAlarm / li->delta + (li->delta > 0 ? 1 : 0)) * li->delta;
        }
        else
        {
            p->lowAlarm += li->delta;
        }
        break;
    case PROBE_FIELD_HIGH:
        if (p->highAlarm % li->delta)
        {
            p->highAlarm = ((int)p->highAlarm / li->delta + (li->delta > 0 ? 1 : 0)) * li->delta;
        }
        else
        {
            p->highAlarm += li->delta;
        }
    }
}*/

} // namespace

void Probe::read()
{
    int x = adc->analogRead((uint8_t)pin);
#ifdef DEBUG
    Serial.printf("DEBUG: Probe %d reading %d\n", id, x);
#endif
    bool oldConn = connected;
    float oldTemp = temperature;
    if ((x <= ADC_LOW_THRESHOLD) || (x >= ADC_HIGH_THRESHOLD))
    {
        connected = false;
        temperature = 0;
        if (oldConn)
        {
            Serial.printf("---DEBUG DEBUG: Disconnected!!!\n");
            dispatch(new ProbeEvent(this, ProbeEvent::Action::DISCONNECT));
        }
        return;
    }
    float res = float(STATIC_R) / ((float(ADC_RES - 1) / float(x - 1)) - 1);
    float kelvins = 1.0 / (float(STEIN_A) + float(STEIN_B * log(res)) + float(STEIN_C * pow(log(res), 3.0)));
    float temp = 1.8 * (kelvins - 273.0) + 32.0;
    connected = true;
    temperature = temp;
    if (!oldConn)
        dispatch(new ProbeEvent(this, ProbeEvent::Action::CONNECT));

    if (temperature != oldTemp)
    {
        dispatch(new ProbeEvent(this, ProbeEvent::Action::TEMP_CHANGE));
        if (arm)
        {
            if (temperature < highAlarm - 1.0 && temperature > lowAlarm + 1.0)
                arm = false;

            else if (temperature > highAlarm || temperature < lowAlarm)
            {
                dispatch(new ProbeEvent(this, ProbeEvent::Action::ALARM));
                arm = false;
            }
        }
        else
        {
            if ((temperature > highAlarm - 1.0 && oldTemp < highAlarm - 1.0) ||
                (temperature < lowAlarm + 1.0 && oldTemp > lowAlarm + 1.0))
                arm = true;
        }
    }
}

Probe *Probe::next()
{
    Probe *pr = this;
    Serial.printf("---DEBUG--- MY ID %d\n", id);
    do
    {
        uint8_t nextID = ((uint8_t)pr->id) + 1;
        Serial.printf("---DEBUG--- NEXT CHECKING %d\n", nextID);
        if ((Probe::ID)nextID == Probe::ID::COUNT)
            nextID = 0;

        pr = &probes[nextID];
        if (pr->connected)
            break;
    } while (pr != this);
    return pr == this ? NULL : pr;
}

Probe *Probe::prev()
{
    Probe *pr = this;
    do
    {
        uint8_t prevID = ((uint8_t)pr->id);
        if (prevID == 0)
            prevID = (uint8_t)Probe::ID::COUNT;
        prevID--;
        pr = &probes[prevID];
        if (pr->connected)
            break;
    } while (pr != this);
    return pr == this ? NULL : pr;
}

void probeSetup()
{
    //Initialize the probe array
    adc->setResolution(ADC_BITS);
    adc->setAveraging(16);
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);

    pinMode((uint8_t)Pin::PROBE_POWER, OUTPUT);

    for (uint8_t i = 0; i < (uint8_t)Probe::ID::COUNT; i++)
    {
        strcpy(probes[i].name, PROBE_DEFAULT_NAMES[i]);
        probes[i].id = (Probe::ID)i;
        probes[i].pin = PROBE_PINS[i];
        probes[i].connected = false;
        probes[i].temperature = 0.0;
        probes[i].lowAlarm = 32.0;
        probes[i].highAlarm = 500.0;
        probes[i].arm = false;
        pinMode((uint8_t)probes[i].pin, INPUT);
    }
    registerHandler(Event::Type::RESET, &handler);
    registerHandler(Event::Type::MODE, &handler);
}

void probeLoop(uint32_t ts)
{
    if (currentMode == RunMode::NORMAL)
    {
        if (int32_t(poweredOnTime) - int32_t(lastReadTime) > int32_t(0))
        {
            if (int32_t(ts) - int32_t(poweredOnTime) > int32_t(PROBES_POWERON_DELAY))
            {
                lastReadTime = readProbes();
                powerOffProbes();
            }
        }
        else if (int32_t(ts) - int32_t(lastReadTime) > int32_t(PROBES_READ_INTERVAL))
            poweredOnTime = powerOnProbes();
    }
}

#ifdef DEBUG
void ProbeEvent::log(Stream &s)
{
    const char *probes[] = {
        "PROBE_0"
        "PROBE_1",
        "PROBE_2",
        //"PROBE_3",
        "COUNT",
    };

    Event::prelog(s);
    s.printf("TYPE: PROBE; PROBE: %s; CONN: %d\n", probes[(uint8_t)probe->id], probe->connected);
}
#endif // DEBUG