#include <stdint.h> // Gives us standard integer types like uint32_t

/*
 This function asks the debugger (QEMU) to perform an action
 on behalf of our microcontroller program. This mechanism is
 called "semihosting".

 service = what we want the host computer to do
 opaque  = pointer to extra information the service needs
*/
static int semihost_call(int service, void *opaque) {

  // Put the service number into CPU register r0
  register int r0 asm("r0") = service;

  // Put the pointer to our parameters into CPU register r1
  register void *r1 asm("r1") = opaque;

  // This variable will receive the return value (also in r0)
  register int result asm("r0");

  /*
  "bkpt 0xAB" is a special ARM instruction.
  It pauses the CPU and signals the debugger (QEMU).

  QEMU sees this instruction and:
  1. Reads r0 to know which service we want
  2. Reads r1 to find the parameters
  3. Performs the request on the host computer
  4. Puts a result back into r0
  5. Lets the CPU continue running
  */
  asm volatile("bkpt 0xab"          // volatile, don't optimize
               : "=r"(result)       // output: =general register =r for
                                    // output and r for inputs
               : "r"(r0), "r"(r1)); // inputs: out r0 and r1 values

  return result; // give the result back to the caller
}

/*
 List of semihosting service numbers.
 These are like command IDs the debugger understands.
*/
enum SEMIHOST_SVC {
  SYS_WRIE = 0x05, // Request to write text to an output stream
};

void main(void) {
  // This is the text we want to print
  char message[] = "Hello World!\n";

  /*
  The write service expects a block of 3 numbers:

  param[0] = where to write (1 means standard output)
  param[1] = memory address of the text
  param[2] = number of bytes to write
  */
  uint32_t param[] = {
      1,                 // 1 = standard output (your terminal)
      (uint32_t)message, // address of out string in memory
      sizeof(message)    // length of the string in bytes
  };

  // Ask the debugger to print our message
  semihost_call(SYS_WRITE, (void *)param);

  // Infinite loop so the program does not run into empty memory
  while (1)
    ;
}