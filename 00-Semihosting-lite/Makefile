# CROSS_COMPILE is a prefix for ARM tools. 
# This allows using arm-none-eabi-gcc instead of just gcc.
CROSS_COMPILE ?= arm-none-eabi-

# The compiler to use
CC := $(CROSS_COMPILE)gcc

# Compiler flags
CFLAGS = \
    -fno-common \         # Avoid putting uninitialized global variables in a common area
    -O0 \                 # No optimization, easier to debug
    -std=gnu99 \          # Use the GNU C99 standard
    -mcpu=cortex-m3 \     # Generate code for Cortex-M3 CPU
    -mthumb \             # Use Thumb instruction set (required on Cortex-M)
    -T semi.ld \          # Linker script that defines memory layout
    -nostartfiles         # Don’t include default C startup files (we have our own)

# Name of the output binary
TARGET = semi.bin

# Default rule: build the target binary
all: $(TARGET)

# Rule to build semi.bin from our C files
$(TARGET): semi.c startup.c
	# Step 1: compile and link to ELF format (semi.elf)
	$(CC) $(CFLAGS) $^ -o semi.elf

	# Step 2: convert ELF file to raw binary for the MCU
	$(CROSS_COMPILE)objcopy -O binary semi.elf semi.bin

	# Step 3: optional: create a human-readable assembly listing for inspection
	$(CROSS_COMPILE)objdump -S semi.elf > semi.list

# Run the program in QEMU (simulated MCU)
qemu: $(TARGET)
	# Check that QEMU supports the STM32 board
	@qemu-system-arm -M ? | grep stm32-p103 >/dev/null || exit

	# Give user instructions to exit QEMU
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo

	# Launch QEMU with:
	# - stm32-p103 board
	# - semihosting enabled (so our bkpt 0xAB works)
	# - no graphical window (use terminal)
	# - kernel is our binary
	qemu-system-arm -M stm32-p103 -semihosting -nographic -kernel semi.bin

# Clean up build artifacts
clean:
	rm -f *.o *.bin *.elf *.list
