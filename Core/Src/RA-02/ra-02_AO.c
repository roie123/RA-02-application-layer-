//
// Created by royivri on 2/18/26.
//

#include "FreeAct.h"
#include "ra-02_AO.h"

#include <string.h>

#include "packet_t.h"
#include "LoRa/LoRa_Startup.h"

static LoRa myLoRa;

static uint8_t rx_buffer[64] = {0};
static uint8_t tx_buffer[64] = {0};


/*..........................................................................................*/

void IDLE(Active *const me, Event const *const e) {
    switch (e->sig) {
        case RA02_INIT_EVT: {
            myLoRa = newLoRa();

            if (LoRa_Startup(&myLoRa) != NULL) {
                me->dispatch = RA02_ACTIVE_STATE;
            } else {
                //TODO : handle failure
            }
        }
    }
}


/*..........................................................................................*/

void RA02_ACTIVE_STATE(Active *const me, Event const *const e) {
    switch (e->sig) {
        case RECEIVED_TRANSMISSION_EVENT: {
            me->dispatch = RA02_RX_MODE;
            break;
        }

        case TRANSMISSION_REQ_EVT: {
            RA02_TRANSMISSION_REQ_Event_t *p = (RA02_TRANSMISSION_REQ_Event_t *) e;
            memset(tx_buffer, 0, sizeof(packet_t));
            memcpy(tx_buffer, p->payload, sizeof(packet_t));


            me->dispatch = RA02_TX_MODE;
        }
    }
}

/*..........................................................................................*/
void RA02_RX_MODE(Active *const me, Event const *const e) {
    switch (e->sig) {
        case RECEIVED_TRANSMISSION_EVENT: {
            uint8_t received_bytes = LoRa_receive(&myLoRa, &rx_buffer, sizeof(packet_t));

            if (received_bytes > (uint8_t) 0) {
                //TODO : forward to router
                me->dispatch = RA02_ACTIVE_STATE;
            } else {
                //TODO : handle rx failure
            }

            break;
        }
    }
}

void RA02_TX_MODE(Active *const me, Event const *const e) {
    if (LoRa_transmit(&myLoRa,
        tx_buffer,
        sizeof(packet_t),
        TRANSMISSION_TIMEOUT)) {
        me->dispatch = RA02_ACTIVE_STATE;
    }else {
        //TODO : handle failure
        me->dispatch= RA02_ACTIVE_STATE;
    }

}
