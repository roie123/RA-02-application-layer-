//
// Created by royivri on 1/26/26.
//

#include "packet_t.h"

#include <assert.h>
#include <string.h>

uint16_t crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFFU; // initial value
    for (uint16_t i = 0; i < length; i++) {
        crc ^= ((uint16_t)data[i] << 8);
        for (uint8_t j = 0U; j < 8U; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

int build_packet(packet_t *pkt, const uint8_t src_id,const  uint8_t dest_id, const flags flags,const  uint8_t max_hops,const  uint16_t msg_id,
               const   uint8_t *payload) {


    assert(pkt != NULL);                         // Packet pointer must be valid
    assert(payload != NULL);                     // Payload pointer must be valid
    assert(max_hops <= (uint8_t)MESH_MAX_HOPS);                     // Example: max_hops fits in 4 bits
    assert(src_id != dest_id);                     // Example: src_id != dest_id


    pkt->src_id = src_id;
    pkt->dest_id = dest_id;
    pkt->flags = flags;
    pkt->max_hops = max_hops;
    pkt->msg_id = msg_id;
    (void)memcpy(pkt->payload, payload, MESH_MAX_PAYLOAD); // (void) => for MISRA
    pkt->crc = crc16((uint8_t*)pkt,offsetof(packet_t,crc));



    return 1;
}
