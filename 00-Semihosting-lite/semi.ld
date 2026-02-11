ENTRY(reset_handler)   /* Entry point for the program */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 128K   /* MCU Flash memory */
}

SECTIONS
{
    .text :   /* Place code here */
    {
        KEEP(*(.isr_vector))   /* Keep the vector table in Flash */
        *(.text)                /* All other code sections */
    } >FLASH   /* Map this section into Flash memory */
}
