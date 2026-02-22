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



/**
 * @brief Initializes the LoRa module and prepares it for reception.
 *
 * Creates a mutex for thread-safe access, configures GPIO and SPI settings,
 * resets and initializes the LoRa module, and starts continuous reception.
 *
 * @return 1 if initialization succeeds, 0 if it fails.
 */

LoRa * LoRa_Startup(LoRa * lora_instance) {

     // LoRa myLora = newLoRa();
    // lora_instance=&myLora;



    lora_instance->CS_port = NSS_GPIO_Port;
    lora_instance->CS_pin = NSS_Pin;
    lora_instance->reset_port = RESET_GPIO_Port;
    lora_instance->reset_pin = RESET_Pin;
    lora_instance->DIO0_port = DID0_GPIO_Port;
    lora_instance->DIO0_pin = DID0_Pin;
    lora_instance->power = POWER_17db;
    lora_instance->hSPIx = &hspi2;
    LoRa_reset(lora_instance);

    if (LoRa_init(lora_instance) !=LORA_OK) {
        return 0;
    }

    LoRa_startReceiving(lora_instance);





    return lora_instance;
}




