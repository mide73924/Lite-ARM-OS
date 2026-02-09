#include <stdint.h>

/*
============================================================
  C ENTRY POINT
============================================================
This is the "main" function for our program. On a bare-metal
STM32, the processor doesn't know about C runtime initialization
or "main" by default — we provide that ourselves.
*/
extern void main(void);

/*
============================================================
  RESET HANDLER
============================================================
This function is called when the MCU resets (power-on or software reset).

On ARM Cortex-M:

- The CPU starts by loading the "stack pointer" from address 0.
- Then it jumps to the "reset handler" address, which we provide.
- Here we simply jump to our C code entry point (`main`).

You could also initialize the .data and .bss sections here,
but for simplicity, we're skipping that.
*/

void reset_handler(void) {
  /* jump to C entry point*/
  main();
}

/*
============================================================
  INTERRUPT VECTOR TABLE
============================================================
On Cortex-M, the first few memory words after the vector table
address define the initial stack pointer and handlers for exceptions/interrupts.

Here, we only define the reset vector:

- index 0 (address 0x00000000): initial stack pointer (optional in this minimal
example, set as 0)
- index 1 (address 0x00000004): reset handler address
- additional entries would be NMI, HardFault, and peripheral ISRs

The `__attribute__((section(".isr_vector")))` tells the linker
to place this array at the start of the flash memory, where
the CPU expects the vector table to live.
*/
__attribute__((section(".isr_vector"))) uint32_t *isr_vectores[] = {
    0, /* initial stack pointer (would normally point to top of SRAM) */
    (uint32_t *)reset_handler /* reset handler — code entry point after reset */
                              /* Additional interrupt vectors could go here */
};