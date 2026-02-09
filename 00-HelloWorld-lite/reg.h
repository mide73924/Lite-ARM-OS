
#ifndef __REG_H_
#define __REG_H_

#include <stdint.h>

/*
============================================================
  MEMORY-MAPPED REGISTER BASICS
============================================================

On STM32 (ARM Cortex-M), hardware peripherals are controlled
by reading and writing specific memory addresses.

These are NOT normal variables.
They are physical hardware registers.

"volatile" is REQUIRED:
It tells the compiler that the value can change outside the
program’s control and must never be optimized away.
*/
#define __REG_TYPE volatile uint32_t
#define __REG __REG_TYPE *

/*
============================================================
  STM32 MEMORY MAP OVERVIEW
============================================================

0x08000000  → Flash memory (program code lives here)
0x20000000  → SRAM (variables/stack/heap)
0x40000000+ → Peripheral registers (GPIO, UART, RCC, etc.)

Peripherals are controlled by writing to addresses in the
0x40000000 region.
*/

/*
============================================================
  RCC (Reset and Clock Control)
============================================================

The RCC controls clocks to every hardware peripheral.
If a peripheral’s clock is OFF, that peripheral is effectively
dead — reads/writes to its registers will do nothing.

RCC Base Address: 0x40021000
(RCC_BASE doesn’t do anything.
It’s just the starting address of the RCC hardware block in memory.)
*/
#define RCC_BASE ((__REG_TYPE)0x40021000)

/*
APB2ENR — APB2 Peripheral Clock Enable Register
Offset: 0x18 from RCC base

Used to enable clocks for *high-speed* peripherals:
GPIO ports, ADC, TIM1, USART1, etc.
*/
#define RCC_APB2ENR ((__REG)(RCC_BASE + 0x18))

/*
APB1ENR — APB1 Peripheral Clock Enable Register
Offset: 0x1C from RCC base

Used for *lower-speed* peripherals:
USART2/3, I2C, SPI2, timers 2–7, etc.
*/
#define RCC_APB1ENR ((__REG)(RCC_BASE + 0x1c))

/*
============================================================
  GPIOA (General Purpose I/O Port A)
============================================================

GPIO controls pins on the microcontroller.

GPIOA Base Address: 0x40010800
*/
#define GPIOA_BASE ((__REG_TYPE)0x40010800)

/*
CRL — Configuration Register Low (pins 0–7)
CRH — Configuration Register High (pins 8–15)

These registers control:
- Input vs Output
- Push-pull vs Open-drain
- Speed
- Alternate functions (UART, SPI, etc.)
*/
#define GPIOA_CRL ((__REG)(GPIOA_BASE + 0x00))
#define GPIOA_CRH ((__REG)(GPIOA_BASE + 0x04))

/*
============================================================
  USART2 (Universal Synchronous/Asynchronous Receiver/Transmitter)
============================================================

communication peripheral facilitates asynchronous serial communication.

USART2 Base Address: 0x40004400
Connected to APB1 bus (so APB1 clock must be enabled).
*/
#define USART2_BASE ((__REG_TYPE)0x40004400)

/*
SR — Status Register
  Bit flags showing hardware state (TX empty, RX full, errors, etc.)
*/
#define USART2_SR ((__REG)(USART2_BASE + 0x00))

/*
DR - Data Register
    Write to send a byte
    Read to receive a byte
*/
#define USART2_DR ((__REG)(USART2_BASE + 0x04))

/*
CR1 — Control Register 1
  Enables USART, transmitter, receiver, interrupts, etc.
*/
#define USART2_CR1 ((__REG)(USART2_BASE + 0x0C))

/*
============================================================
  WHY THIS MATTERS
============================================================

Before using GPIOA or USART2 you MUST:

1) Turn on their clocks via RCC
2) Configure GPIO pins for alternate function (UART TX)
3) Configure USART registers
4) Then send/receive data

Without clock enable → peripheral does nothing.
*/
#endif