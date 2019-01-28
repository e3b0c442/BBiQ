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
const uint16_t ADC_LOW_THRESHOLD = ADC_RES / 50;
const uint16_t ADC_HIGH_THRESHOLD = ADC_RES - ADC_RES / 50;
const float STATIC_R = 10000.0;

// Loop timers
const uint32_t PROBES_POWERON_DELAY = 1;
const uint32_t PROBES_READ_INTERVAL = 1000;
uint32_t PROBES_POWERED_ON_TIME;
uint32_t PROBES_LAST_READ_TIME;

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

ButtonState lastButtonState = ButtonState::ALL_UP;
RunMode currentMode;
Probe probes[(uint8_t)Probe::ID::COUNT];
Probe::ID active = Probe::ID::COUNT;

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

void readProbe(Probe *probe)
{
    int x = adc->analogRead((uint8_t)probe->pin);
#ifdef DEBUG
    Serial.printf("Probe %d reading %d\n", probe->id, x);
#endif
    bool oldConn = probe->connected;
    float oldTemp = probe->temperature;
    if ((x <= ADC_LOW_THRESHOLD) || (x >= ADC_HIGH_THRESHOLD))
    {
        probe->connected = false;
        probe->temperature = 0;
        if (oldConn)
        {
            Serial.printf("--DEBUG DEBUG: Disconnected!!!\n");
            dispatch(new ProbeEvent(probe, ProbeEvent::Action::DISCONNECT));
            if (active == probe->id)
            {
                Serial.printf("--DEBUG DEBUG: Active probe disconnected\n");
                for (int i = 0; i < (uint8_t)Probe::ID::COUNT; i++)
                {
                    if (probes[i].connected)
                    {
                        active = probes[i].id;
                        dispatch(new ProbeEvent(&probes[i], ProbeEvent::Action::SELECT));
                        return;
                    }
                    active = Probe::ID::COUNT;
                    dispatch(new ProbeEvent(NULL, ProbeEvent::Action::SELECT));
                }
            }
        }
        return;
    }
    float res = float(STATIC_R) / ((float(ADC_RES - 1) / float(x - 1)) - 1);
    float kelvins = 1.0 / (float(STEIN_A) + float(STEIN_B * log(res)) + float(STEIN_C * pow(log(res), 3.0)));
    float temp = 1.8 * (kelvins - 273.0) + 32.0;
    probe->connected = true;
    probe->temperature = temp;
    if (!oldConn)
    {
        dispatch(new ProbeEvent(probe, ProbeEvent::Action::CONNECT));
        if (active == Probe::ID::COUNT)
        {
            active = probe->id;
            dispatch(new ProbeEvent(probe, ProbeEvent::Action::SELECT));
        }
    }
    if (probe->temperature != oldTemp)
    {
        dispatch(new ProbeEvent(probe, ProbeEvent::Action::TEMP_CHANGE));
        if (probe->arm)
        {
            if (probe->temperature < probe->highAlarm - 1.0 && probe->temperature > probe->lowAlarm + 1.0)
                probe->arm = false;

            else if (probe->temperature > probe->highAlarm || probe->temperature < probe->lowAlarm)
            {
                dispatch(new ProbeEvent(probe, ProbeEvent::Action::ALARM));
                probe->arm = false;
            }
        }
        else
        {
            if ((probe->temperature > probe->highAlarm - 1.0 && oldTemp < probe->highAlarm - 1.0) ||
                (probe->temperature < probe->lowAlarm + 1.0 && oldTemp > probe->lowAlarm + 1.0))
                probe->arm = true;
        }
    }
}

uint32_t readProbes()
{
    for (uint8_t i = 0; i < (uint8_t)Probe::ID::COUNT; i++)
        readProbe(&probes[i]);

    return millis();
}

void selectNextProbe()
{
    uint8_t index = (uint8_t)active + 1;
    while (index != (uint8_t)active)
    {
        if (Probe::ID(index) == Probe::ID::COUNT)
            index = 0;

        if (probes[index].connected)
        {
            dispatch(new ProbeEvent(&probes[index], ProbeEvent::Action::SELECT));
            active = Probe::ID(index);
            return;
        }
        index++;
    }
}

void selectPrevProbe()
{
    uint8_t index = (uint8_t)active - 1;
    while (index != (uint8_t)active)
    {
        if (index == UINT8_MAX)
            index = (uint8_t)Probe::ID::COUNT - 1;

        if (probes[index].connected)
        {
            dispatch(new ProbeEvent(&probes[index], ProbeEvent::Action::SELECT));
            active = Probe::ID(index);
            return;
        }
        index--;
    }
}

void handler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::BUTTON:
    {
        ButtonEvent *be = (ButtonEvent *)e;
        switch (be->state)
        {
        case ButtonState::ONE_DOWN:
            if (lastButtonState == ButtonState::ALL_UP)
                selectNextProbe();

            break;
        case ButtonState::TWO_DOWN:
            if (lastButtonState == ButtonState::ALL_UP)
                selectPrevProbe();

            break;
        default:;
        }
        lastButtonState = be->state;
        break;
    }
    case Event::Type::MODE:
    {
        ModeEvent *me = (ModeEvent *)e;
        currentMode = me->mode;
        break;
    }
    case Event::Type::RESET:
        digitalWrite((uint8_t)Pin::PROBE_POWER, LOW);
        PROBES_POWERED_ON_TIME = e->ts;
        PROBES_LAST_READ_TIME = e->ts;
        active = Probe::ID::COUNT;
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
    registerHandler(Event::Type::BUTTON, &handler);
}

void probeLoop(uint32_t *ts)
{
    if (currentMode == RunMode::NORMAL)
    {
        if (int32_t(PROBES_POWERED_ON_TIME) - int32_t(PROBES_LAST_READ_TIME) > int32_t(0))
        {
            if (int32_t(*ts) - int32_t(PROBES_POWERED_ON_TIME) > int32_t(PROBES_POWERON_DELAY))
            {
                PROBES_LAST_READ_TIME = readProbes();
                powerOffProbes();
            }
        }
        else if (int32_t(*ts) - int32_t(PROBES_LAST_READ_TIME) > int32_t(PROBES_READ_INTERVAL))
            PROBES_POWERED_ON_TIME = powerOnProbes();
    }
}
