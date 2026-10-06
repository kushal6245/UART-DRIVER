# UART Driver

A UART driver developed in Embedded C, starting from a software-level UART simulation and progressing toward implementation on real embedded hardware.

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

This project implements UART functionality from the ground up rather than relying entirely on high-level communication functions.

The simulation models:

- UART frame generation
- Bit-by-bit transmission
- Bit-by-bit reception
- Data reconstruction
- Parity generation and validation
- Stop-bit validation
- RX buffering
- Error detection and frame rejection
- UART timing calculations

The next stage of the project will move the same concepts to real UART hardware.

---

## UART Frame

A UART frame consists of:

```text
START | DATA | PARITY | STOP