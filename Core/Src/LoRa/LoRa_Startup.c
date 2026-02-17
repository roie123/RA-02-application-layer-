//
// Created by royivri on 12/1/25.
//
#include "LoRa/LoRa_Startup.h"
#include <assert.h>
#include "../../Inc/LoRa/LoRa.h"
#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "spi.h"
LoRa myLoRa;
SemaphoreHandle_t lora_mutex_handle;



/**
 * @brief Initializes the LoRa module and prepares it for reception.
 *
 * Creates a mutex for thread-safe access, configures GPIO and SPI settings,
 * resets and initializes the LoRa module, and starts continuous reception.
 *
 * @return 1 if initialization succeeds, 0 if it fails.
 */

uint8_t LoRa_Startup() {

    myLoRa = newLoRa();


    lora_mutex_handle = xSemaphoreCreateMutex();
    assert(lora_mutex_handle != NULL); // Check for successful mutex creation.

    myLoRa.CS_port = NSS_GPIO_Port;
    myLoRa.CS_pin = NSS_Pin;
    myLoRa.reset_port = RESET_GPIO_Port;
    myLoRa.reset_pin = RESET_Pin;
    myLoRa.DIO0_port = DID0_GPIO_Port;
    myLoRa.DIO0_pin = DID0_Pin;
    myLoRa.power = POWER_17db;
    myLoRa.hSPIx = &hspi2;
    LoRa_reset(&myLoRa);

    if (LoRa_init(&myLoRa) !=LORA_OK) {
        return 0;
    }

    LoRa_startReceiving(&myLoRa);





    return 1;
}
uint8_t LoRa_receive_safe(LoRa *lora, uint8_t *data, uint8_t length, SemaphoreHandle_t lora_mutex_handle) {
    uint8_t bytes = 0;
    if (xSemaphoreTake(lora_mutex_handle, portMAX_DELAY) == pdTRUE) {
        bytes = LoRa_receive(lora, data, length);
        xSemaphoreGive(lora_mutex_handle);
    }
    return bytes;


}


uint8_t LoRa_transmit_safe(uint8_t *data, uint8_t length, uint16_t timeout) {
    uint8_t status = 0;


    if (xSemaphoreTake(lora_mutex_handle, portMAX_DELAY) == pdTRUE) {
        status = LoRa_transmit(&myLoRa, data, length, timeout);
        xSemaphoreGive(lora_mutex_handle);
    }

    return status;
}
