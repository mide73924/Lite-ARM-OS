#include <stdint.h> // Gives us standard integer types like uint32_t

/*
  The reset handler is the first function the CPU runs after power-on or reset.
  It is responsible for starting our program.
*/
extern void main(void); // Tell the compiler that main() exists somewhere else

void reset_handler(void) {

  // Jump to the main program
  main();

  // If main ever return (it should't), just loop forever
  while (1)
    ;
}

/*
  The interrupt vector table is a special table the CPU looks at when it starts
  and whenever an interrupt occurs. Each entry is an address the CPU jumps to.

  Placement at the start of Flash (0x00000000) is controlled by the linker
  script.
*/

__attribute((
    section(".isr_vector"))) // put this table in its own memory section
uint32_t *isr_vectors[] = {
    0, // Initial stack pointer(0 for new; real startup would set this)
    (uint32_t *)
        reset_handler, // Address of our reset handler (where execution begins)
                       // Normally other interrupt handlers would follow here
};