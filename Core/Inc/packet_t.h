//
// Created by royivri on 1/26/26.
//

#ifndef PACKET_H
#define PACKET_H
#include <stdint.h>


#define MESH_MAX_PAYLOAD 6
#define MESH_MAX_HOPS 5
#define  MESH_BROADCAST_ID 255U

typedef struct  __attribute__((packed)) flags {
uint8_t requires_ack:1;
uint8_t broadcasting:1;
uint8_t needs_forwarding:1;
uint8_t connected_nodes_info:1;

}flags;


typedef struct __attribute__((packed))  packet_t {

uint8_t src_id;
uint8_t dest_id;
flags flags;
uint8_t max_hops;
uint16_t msg_id;
uint8_t payload[MESH_MAX_PAYLOAD];
uint16_t crc;
uint16_t padding;
}packet_t;
uint16_t crc16(const uint8_t *data, uint16_t length);
int build_packet(packet_t *pkt ,const uint8_t src_id ,const uint8_t dest_id ,const flags flags ,const uint8_t max_hops ,const uint16_t msg_id ,const uint8_t *payload );
#endif //PACKET_H
