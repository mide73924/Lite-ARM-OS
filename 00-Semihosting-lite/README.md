Run QEMU with semihosting enabled:

../../qemu_stm32/arm-softmmu/qemu-system-arm -M stm32-p103 -semihosting -nographic -kernel semi.bin

---

This stage proves **your toolchain, linker script, CPU mode, and emulator setup all work** before touching real hardware.

---

# 🧠 What This Stage Is Teaching

**Goal:** Print _“Hello World”_ from a bare-metal Cortex-M3 **without any UART drivers** by using **semihosting**.

Semihosting lets your embedded program ask the **debugger/emulator** to do I/O (like printing text) on its behalf.

So instead of configuring STM32 serial hardware, you “borrow” the host PC’s console.

---

# 🔑 The Big Ideas (What You Really Need to Remember)

1. **No OS exists yet**
   You are running directly on the CPU after reset.

2. **No standard library**
   `printf()` doesn’t exist. You must manually invoke a debug service.

3. **Semihosting uses a special breakpoint instruction**

   ```asm
   bkpt 0xAB
   ```

   This traps into the debugger (QEMU in this case).

4. **Arguments are passed through registers**
   - `r0` = semihosting service number
   - `r1` = pointer to argument block

---

# 📦 File-by-File Breakdown

## 1️⃣ `semi.c` — Your “Application”

### 🔹 The semihost call wrapper

```c
static int semihost_call(int service, void *opaque)
{
	register int r0 asm("r0") = service;
	register void *r1 asm("r1") = opaque;
	register int result asm("r0");

	asm volatile("bkpt 0xab"
	             : "=r" (result) : "r" (r0), "r" (r1));
	return result;
}
```

**What’s happening:**

| Piece          | Meaning                                                        |
| -------------- | -------------------------------------------------------------- |
| `bkpt 0xab`    | Special ARM breakpoint recognized by debuggers for semihosting |
| `r0`           | Service number                                                 |
| `r1`           | Pointer to parameters                                          |
| Return in `r0` | Result from host                                               |

---

### 🔹 The write service

```c
enum SEMIHOST_SVC {
	SYS_WRITE = 0x05,
};
```

Service **0x05 = write to file descriptor**

File descriptors in semihosting:

| FD  | Meaning |
| --- | ------- |
| 0   | stdin   |
| 1   | stdout  |
| 2   | stderr  |

---

### 🔹 main()

```c
void main(void)
{
	char message[] = "Hello World!\n";
	uint32_t param[] = { 1, (uint32_t) message, sizeof(message) };
	semihost_call(SYS_WRITE, (void *) param);
	while (1);
}
```

**Parameter block layout for SYS_WRITE:**

| Index          | Meaning           |
| -------------- | ----------------- |
| `param[0] = 1` | stdout            |
| `param[1]`     | pointer to string |
| `param[2]`     | length            |

So you’re basically doing:

> “Debugger, please print this buffer to stdout.”

---

## 2️⃣ `startup.c` — Minimal Boot Code

```c
void reset_handler(void)
{
	main();
}
```

There is **no C runtime init**, no `.data` copy, no `.bss` zeroing.
That comes later in the project.

---

### Vector Table

```c
__attribute((section(".isr_vector")))
uint32_t *isr_vectors[] = {
	0,
	(uint32_t *) reset_handler,
};
```

| Entry | Purpose                                    |
| ----- | ------------------------------------------ |
| 0     | Initial Stack Pointer (ignored here)       |
| 1     | Reset vector (where CPU jumps after reset) |

The linker script places this at address **0x00000000** so the CPU finds it on boot.

---

## 3️⃣ Makefile — Important Flags

```make
-mcpu=cortex-m3 -mthumb
-T semi.ld
-nostartfiles
```

| Flag            | Why it matters                              |
| --------------- | ------------------------------------------- |
| `-mthumb`       | Cortex-M only executes Thumb instructions   |
| `-T semi.ld`    | Custom memory layout                        |
| `-nostartfiles` | Prevents GCC from linking its own C runtime |

---

## 4️⃣ Running in QEMU

```bash
qemu-system-arm -M stm32-p103 -semihosting -nographic -kernel semi.bin
```

Key option:

```
-semihosting
```

Without this, the `bkpt 0xAB` would cause a crash.

---

# 🧩 What This Stage Does NOT Do (Yet)

❌ No stack setup
❌ No interrupt handling
❌ No memory initialization
❌ No hardware drivers
❌ No scheduler

This is **pre-OS**, just proving execution + debug I/O.

---

# 🪜 Why This Is Step 00

Before you build:

- UART drivers
- Context switching
- Syscalls
- Scheduler

You must first prove:

✔ Code runs from reset
✔ Linker script works
✔ You understand vector tables
✔ You can communicate with the outside world

Semihosting is the **training wheels** before real hardware drivers.

---

# 🔜 What Changes in the Next Step (`00-HelloWorld`)

Semihosting ❌ removed
UART driver ✅ added

Now **the MCU itself** prints text via USART instead of asking the debugger.

---

# 🧠 Final Mental Model

Right now your “OS” is:

```
CPU reset → vector table → reset_handler → main → bkpt → QEMU prints text
```

You are not writing an OS _yet_ — you’re building the **foundation required to make one possible**.

---

If you'd like, next I can explain:

- How the linker script places the vector table
- What happens electrically on a real STM32 at reset
- Why stack pointer = first vector entry
