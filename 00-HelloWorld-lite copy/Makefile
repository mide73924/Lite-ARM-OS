# CROSS_COMPILE defines a prefix added before tool names.
# This lets us use a compiler that builds code for ARM
# even though we are compiling on an x86 Linux PC.
#
# arm-none-eabi- means:
#   arm        → target CPU architecture
#   none       → no operating system (bare metal)
#   eabi       → Embedded ABI (calling convention rules)
CROSS_COMPILE ?= arm-none-eabi-

# CC becomes "arm-none-eabi-gcc"
# This is the C compiler that produces ARM machine code.
CC := $(CROSS_COMPILE)gcc

# Compiler flags:
# -fno-common     → prevents duplicate global variables from silently merging
# -O0             → no optimization (easier to debug and inspect)
# -mcpu=cortex-m3 → generate instructions for Cortex-M3 CPU
# -mthumb         → use Thumb instruction set (required for Cortex-M)
# -T hello.ld     → use our custom linker script (controls memory layout)
# -nostartfiles   → do NOT use standard C runtime startup files
#                    (we provide our own startup.c and vector table)
CFLAGS = -fno-common -O0 -mcpu=cortex-m3 -mthumb -T hello.ld -nostartfiles

# Final file we want to produce
TARGET = hello.bin

# Default make target → builds the firmware binary
all: $(TARGET)

# Build steps:
# Target: $(TARGET)
# Prerequisites: hello.c and startup.c
$(TARGET): hello.c startup.c
	# Compile and link into an ELF file (contains symbols + debug info)
	$(CC) $(CFLAGS) $^ -o hello.elf

	# Convert ELF → raw binary image that can be flashed or run in QEMU
	$(CROSS_COMPILE)objcopy -O binary hello.elf hello.bin

	# Produce a human-readable disassembly listing
	# Useful for learning how C turns into ARM assembly
	$(CROSS_COMPILE)objdump -S hello.elf > hello.list

# Run the program in QEMU emulator
qemu: $(TARGET)
	# Check that the STM32 board model exists in this QEMU build
	@qemu-system-arm -M ? | grep stm32-p103 >/dev/null || exit

	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo

	# Start ARM emulator
	# -M stm32-p103 → emulate STM32 development board
	# -nographic    → no GUI, use terminal only
	# -kernel       → load our firmware binary into flash memory
	qemu-system-arm -M stm32-p103 -nographic -kernel hello.bin

# Remove all build outputs so we can rebuild cleanly
clean:
	rm -f *.o *.elf *.bin *.list
