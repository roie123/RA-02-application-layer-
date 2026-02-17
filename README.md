# RA-02 LoRa Application Layer

## Overview

This repository contains the **application layer** implementation for the **RA-02 LoRa module**, designed using an **Active Object (AO) model**. The goal of this project is to explore high-performance, scalable embedded architectures while keeping **RAM usage low**.

The project is primarily for **learning purposes**, focusing on:

- Event-driven programming  
- Lock-free queues  
- Function-pointer-based state machines  
- Embedded RTOS design patterns  

---

## Architecture

### Active Object Model

The entire module is structured around **Active Objects**, where each AO:

- Encapsulates its state and event queue  
- Manages its own task in FreeRTOS  
- Uses **static allocation** (tasks and queues) to minimize RAM usage  

This design allows for:

- Deterministic memory usage  
- Better modularity and scalability  
- Clear separation of concerns between TX, RX, and idle states  

---

### DV / Vyukov MPSC Queue

Instead of traditional FreeRTOS queues, this project uses a **Vyukov-inspired lock-free MPSC queue**:

- **MPSC** = Multi-Producer, Single-Consumer  
- Designed for **low-latency event passing** from multiple producers to a single consumer  
- Reduces overhead compared to FreeRTOS queues  
- ISR-safe and deterministic  

---

### H-State Machine

The module implements a **hierarchical (H) state machine** with the following structure:

- **Idle** – waiting for events  
- **Active** – handling transmission (TX) or reception (RX)  
  - **TX state** – handles sending LoRa messages  
  - **RX state** – handles receiving LoRa messages  

**State management** is done using **function pointers** instead of a single dispatcher, making the design:

- Lightweight  
- Easy to extend  
- Low RAM footprint  

---

## Goals & Learning Outcomes

This project was created to explore and practice:

- Active Object design in embedded systems  
- Lock-free MPSC queue implementation  
- Hierarchical state machines with function pointers  
- Efficient task and queue management using FreeRTOS  
- General embedded architecture for low-memory, scalable designs  

---

## Project Structure

ra02-application-layer/
├── Core/ # Core STM32 HAL and driver files
├── AO/ # Active Object implementations
│ ├── tx_ao.c
│ ├── rx_ao.c
│ └── ao_base.c
├── Queue/ # Vyukov MPSC queue implementation
├── StateMachine/ # H-state machine functions
├── Middlewares/ # FreeRTOS static tasks and headers
├── .ioc # STM32CubeMX project file
└── README.md



---

## Usage

1. Open the `.ioc` file in **STM32CubeIDE**  
2. Generate code and build with **static task/queue configuration**  
3. Run on RA-02 LoRa hardware  
4. Observe events handled via the Active Object + MPSC queue model  

---

## Notes

- The project is **for learning purposes** and not yet production-ready  
- Only **single-consumer queues** are supported (MPSC)  
- ISR and task producers safely push events into the queues  


