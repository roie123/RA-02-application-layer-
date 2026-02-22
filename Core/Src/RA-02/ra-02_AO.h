//
// Created by royivri on 2/18/26.
//

#ifndef RA_02_AO_H
#define RA_02_AO_H
#include <stdbool.h>

#include "FreeAct.h"

#define TRANSMISSION_TIMEOUT 300

#define RA02_STACK_SIZE 128
#define RA02_PRIORITY 2

typedef enum {
    INIT_EVT = 2,
    RECEIVED_TRANSMISSION_EVENT,
    RX_DONE_EVT,
    TRANSMISSION_REQ_EVT,
    TX_DONE_EVT
} RA_02_EventTypes;

typedef struct RA02 {
    Active super;
    TimeEvent te;
    bool is_initialized;
};

typedef struct {
    Event super;
    uint8_t payload[14]; /*  should be the size of a packet_t   */
} RA02_TRANSMISSION_REQ_Event_t;

typedef struct {
    Event super;
    bool is_successful;
} RA02_RX_DONE_Event_t;

typedef struct {
    Event super;
    bool is_successful;
} RA02_TX_DONE_Event_t;

typedef struct {
    Event super;
    uint8_t payload[14]; /*  should be the size of a packet_t   */
} RA02_RECEIVED_TRANSMISSION_Event_t;





/*...................................................................................*/
//TODO : add state funcs
// void (*RA02_state_func)(struct RA02 * const me, Event const * const e);

void IDLE(Active *const me, Event const *const e);


void RA02_ACTIVE_STATE(Active *const me, Event const *const e);

void RA02_RX_MODE(Active *const me, Event const *const e);

void RA02_TX_MODE(Active *const me, Event const *const e);


/*...................................................................................*/

void RA02_ctor(struct RA02 *const me);


#endif //RA_02_AO_H
