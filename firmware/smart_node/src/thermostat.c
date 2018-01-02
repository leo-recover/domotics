/**
 * @file thermostat.c
 *
 * @date 02 gen 2018
 * @author Leonardo Ricupero
 */

#include "micro.h"
#include "temp_sensor.h"
#include "relays.h"
#include "parameters.h"
#include "thermostat.h"

#define THERMOSTAT_SAMPLE_RATE_100MS 50 // 5 seconds

typedef enum {
    STATE_INIT = 0,
    STATE_IDLE,
    STATE_WAIT_FOR_TEMPERATURE,
    STATE_WAIT_FOR_SET,
    STATE_WAIT_FOR_RESET,
    STATE_SET,
    STATE_RESET,
    STATE_ERROR_FOUND,
} THERMOSTAT_STATE_T;

typedef enum {
    EVENT_NO_EVENT,
    EVENT_ON_REQUESTED,
    EVENT_OFF_REQUESTED,
    EVENT_TIMEOUT_EXPIRED,
} THERMOSTAT_EVENT_T;

typedef enum {
    MODE_WINTER,
    MODE_SUMMER,
} THERMOSTAT_MODE_T;


static uint8_t Thermostat_Counter;
static THERMOSTAT_STATE_T Thermostat_State;
static THERMOSTAT_EVENT_T Thermostat_Event;
static THERMOSTAT_MODE_T Thermostat_Mode;
static int16_t Last_Temperature; // Q12.4 format

void Thermostat__Initialize(void)
{
    Thermostat_Counter = 0;
    Thermostat_State = STATE_INIT;
    Thermostat_Event = EVENT_NO_EVENT;
    Thermostat_Mode = MODE_WINTER;

    Last_Temperature = 0xFFFF;
}

void Thermostat__On(void)
{
    if (Thermostat_Event == EVENT_NO_EVENT)
    {
        Thermostat_Event = EVENT_ON_REQUESTED;
    }
}

void Thermostat__Off(void)
{
    if (Thermostat_Event == EVENT_NO_EVENT)
    {
        Thermostat_Event = EVENT_OFF_REQUESTED;
    }
}

void Thermostat__Handler(void)
{
    THERMOSTAT_STATE_T next_state;

    next_state = Thermostat_State;

    switch (Thermostat_State)
    {
        case STATE_INIT:
        {
            if (Thermostat_Counter == THERMOSTAT_SAMPLE_RATE_100MS)
            {
                Thermostat_Counter = 0;
                TempSensor__StartAcquisition();
                next_state = STATE_WAIT_FOR_TEMPERATURE;
            }
            break;
        }
        case STATE_IDLE:
        {
            break;
        }
        case STATE_WAIT_FOR_TEMPERATURE:
        {
            // todo timeout
            if (TempSensor__IsTemperatureReady())
            {
                Last_Temperature = TempSensor__GetTemperature();
                if (Thermostat_Mode == MODE_WINTER)
                {

                }
                else
                {

                }
            }
            break;
        }
        default:
        {
            break;
        }
    }

    Thermostat_State = next_state;
}
