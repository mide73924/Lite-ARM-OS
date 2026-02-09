#include "reg.h"
#include <stdint.h>

/* TX register empty flag for USART2
   - This is set by the hardware when USART2 is ready to send a new byte.
   - We'll wait on this before sending each character. */
#define USART_FLAG_TXE ((uint16_t)0x0080) // Bit 7 in USART block

/* Simple function to send a string over USART2 */
int puts(const char *str) {
  while (*str) {
    /* Wait until TX register is empty (hardware ready to send) */
    while (!(*(USART2_SR)&USART_FLAG_TXE))
      ;
    /* Send the next char(7bit) to the data register (limit to 8 bit chunked
     * communication) */
    *(USART2_DR) = *str++ & 0xFF;
  }
  return 0;
}

/*

00000000 00000000 00000001 00101010   (value of *str)
&
00000000 00000000 00000000 11111111   (value of 0xFF)
-------------------------------------
00000000 00000000 00000000 00101010   (result)

& 0xFF
produces binary with everything other than the 8
any bits over 7 from the right LSD are flipped off
*/

void main(void) {
  /*
    All addresses are “predefined switches” inside the chip firmware.

    All numbers are “bit masks” telling the chip which switches to flip.

   * 1) Enable clocks for the peripherals we want to use
   *    - If a peripheral's clock is OFF, it won't do anything
   *    - APB2ENR = high-speed peripherals (GPIO, AFIO)
   *    - APB1ENR = low-speed peripherals (USART2)

   */
  // in in hex, each digit is the base10 value of (0000) bites
  *(RCC_APB2ENR) |=
      (uint32_t)(0x00000001 | 0x00000004); // enable GPIO + AFIO bits
  *(RCC_APB1ENR) |= (uint32_t)0x00020000;  // enable USART2

  /*
   * 2) Set up GPIO pins for USART2
   *    - PA2 (TX) needs to be an output for sending
   *    - PA3 (RX) can be input (floating)
   *    - CRL configures pins 0–7, CRH configures pins 8–15
   */
  *(GPIOA_CRL) = 0x00004B00; // configure PA2 as TX, PA3 as RX
  *(GPIOA_CRH) = 0x44444444; // leave upper pins as default input

  /*
   * 3) Configure USART2
   *    - Set transmitter and receiver ON
   *    - Enable USART2 itself
   */
  *(USART2_CR1) = 0x0000000C; // enable TE + RE
  *(USART2_CR1) |= 0x2000;    // set UE (bit 13) without touching TE/RE

  /* 4) Send the string "Hello World!" over USART2 */
  puts("HELLO WORLD!\n");

  /* 5) Keep the program running forever */
  while (1)
    ;
  /**/
}
