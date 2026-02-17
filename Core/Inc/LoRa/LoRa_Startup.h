//
// Created by royivri on 12/1/25.
//

#ifndef LORA_STARTUP_H
#define LORA_STARTUP_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "LoRa.h"
#include "semphr.h"

extern LoRa myLoRa;
extern SemaphoreHandle_t lora_mutex_handle;

uint8_t LoRa_Startup();
uint8_t LoRa_receive_safe(LoRa *lora, uint8_t *data, uint8_t length, SemaphoreHandle_t lora_mutex_handle);
uint8_t LoRa_transmit_safe(uint8_t *data, uint8_t length, uint16_t timeout);


#endif //LORA_STARTUP_H
