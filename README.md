# UART Driver

A UART driver developed in C, starting from a software-level UART simulation and progressing toward real embedded hardware implementations.

The project focuses on understanding UART communication at the bit and frame level and gradually converting that understanding into a reusable embedded driver.

---

## Project Status

| Stage | Status |
|---|---|
| UART Software Simulation | ✅ Completed |
| Arduino UNO / ATmega328P | 🚧 Next |
| STM32 | 🔜 Planned |

---

## Overview

UART (Universal Asynchronous Receiver/Transmitter) is one of the most commonly used serial communication interfaces in embedded systems.

This project starts with a software-level UART simulation written in standard C. The simulation models UART transmission and reception at the bit and frame level without depending on a physical UART peripheral.

The project will progressively move from the software model to real microcontroller hardware.

The current simulation implements:

- UART frame generation
- Bit-by-bit transmission
- Bit-by-bit reception
- Data reconstruction
- Parity generation and validation
- Stop-bit validation
- RX buffering
- Error detection and frame rejection
- UART timing calculations

The next stage will map these concepts onto real UART hardware.

---

# UART Frame

A UART frame consists of:

```text
START | DATA | PARITY | STOP