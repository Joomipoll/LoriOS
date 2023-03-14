<a name="exit"></a>
# Code details

Content:
- [ACPI.cpp](#1)
- [APIC.cpp](#2)
- [CPUID.cpp](#3)
- [IDT.asm](#4)
- [IDT.cpp](#5)
- [PCI.cpp](#6)
- [syscall.asm](#7)
- [EPoll.cpp](#8)
- [ELF.cpp](#9)

## <a name="1">ACPI.cpp</a>

- `processors` is an array of 256 bytes that will store information about the processors in the system.
- `processorCount` is an integer that will track the number of processors in the system. It is initialized to 1, assuming there is at least one processor.
- `signature` is a string containing the four-character ACPI signature for the RSDP (Root System Description Pointer) table.
- `isos` is a pointer to a List of `apic_iso_t` structures. It is not initialized yet.
- `desc` is a pointer to an `acpi_xsdp_t` structure, which represents the ACPI System Description Pointer (XSDP) table.
- `rsdtHeader` is a pointer to an `acpi_rsdt_t` structure, which represents the Root System Description Table (RSDT).
- `xsdtHeader` is a pointer to an `acpi_xsdt_t` structure, which represents the Extended System Description Table (XSDT).
- `fadt` is a pointer to an `acpi_fadt_t` structure, which represents the Fixed ACPI Description Table (FADT).
- `mcfg` is a pointer to a `pci_mcfg_table_t` structure, which represents the PCI Express Memory Mapped Configuration Space table.

> `void* FindSDT(const char* signature, int index)`: 

This function searches for a System Description Table (SDT) in the ACPI tables. The SDT is identified by a signature, which is a four-character string. If multiple tables with the same signature exist, the function can return the Nth occurrence of the table by specifying an index.

The function first determines the number of SDTs in the Root System Description Table (RSDT) by dividing the length of the table by the size of each SDT entry. The size of an SDT entry is either 4 bytes or 8 bytes, depending on the ACPI revision.

The function then defines a lambda function, getEntry, which returns the address of an SDT entry based on the index and the ACPI revision. If the ACPI revision is 2.0, the function retrieves the entry from the Extended System Description Table (XSDT), otherwise from the RSDT.

If the signature is "DSDT", the function returns the address of the Differentiated System Description Table (DSDT) specified in the Fixed ACPI Description Table (FADT).

The function then iterates over all SDT entries, compares their signatures with the specified signature, and increments a counter for each match. If the counter matches the specified index, the function returns the address of the SDT.

If no SDT is found, the function returns NULL.

> `int ReadMADT()`:

This function reads the MADT (Multiple APIC Description Table) from the ACPI (Advanced Configuration and Power Interface) tables. It first finds the MADT table using the FindSDT function and then iterates through each entry in the table.

For each entry, it checks the type and performs the appropriate action. If the entry is a local APIC, it adds the APIC ID to a list of processors. If the entry is an I/O APIC, it sets the base address of the I/O APIC. If the entry is an interrupt source override, it adds it to a list of interrupt source overrides. If the entry is an NMI (Non-Maskable Interrupt), it logs the LINT (Local Interrupt) number.

The function returns 0 if successful and 1 if the MADT table could not be found.

> `void Init()`:

The ``Init()`` function searches for the Root System Description Pointer (RSDP) in memory and initializes the ACPI subsystem. It first checks if the RSDP has already been found and exits if it has. It then searches for the RSDP in memory by scanning through specific memory addresses in increments of 16 bytes. If it finds the RSDP, it sets the desc pointer to the address and jumps to the success label. If it fails to find the RSDP, it panics the kernel and exits.

If the RSDP is found, the function initializes a list of Interrupt Source Override (ISO) structures and checks the revision of the RSDP. If the revision is 2, it sets the RSDT and XSDT headers to their respective addresses. Otherwise, it sets only the RSDT header. It then copies the OEM string from the RSDT header and logs the ACPI revision if the debug level is set to Normal or higher.

The function then disables interrupts, reads the Multiple APIC Description Table (MADT) and attempts to find the PCI Express Configuration Space (MCFG) table. It then enables interrupts and returns.

> `void SetRSDP()`:

This function takes a pointer to an ACPI Extended System Description Pointer (XSDP) structure and sets it to a global variable "desc" using a ``reinterpret_cast``. The purpose of this function is to allow other parts of the program to access the XSDP structure stored in "desc" without having to pass it as a parameter every time.

[To the begining](#exit)

## <a name="2">APIC.cpp</a>

 ::: **Begin: namespace Local** :::

> `#define APIC_READ(off) *((volatile uint32_t*)(virtualBase + off))`, `#define APIC_WRITE(off, val) (*((volatile uint32_t*)(virtualBase + off)) = val)`

The APIC is a component in modern x86-based computer systems that manages interrupt requests (IRQs) and facilitates communication between processors. 

The first macro, `APIC_READ(off)`, takes an offset `off` as input and returns the value stored in the APIC register at the given offset. The `volatile` keyword indicates that the value may change unexpectedly, so the compiler should not optimize it away. 

The second macro, `APIC_WRITE(off, val)`, takes two inputs: an offset `off` and a value `val`. It writes the value `val` to the APIC register at the given offset. 

Both macros use pointer arithmetic to access the APIC registers via a virtual base address (`virtualBase`). This virtual base address is mapped to the physical address of the APIC registers by the kernel during system initialization.

> `volatile uintptr_t virtualBase`

`uintptr_t` is an unsigned integer type that is guaranteed to be capable of holding a pointer value. It is defined in the `<stdint.h>` header file. 

The `volatile` keyword indicates that the value of the `virtualBase` variable may change unexpectedly. This means that any changes to the variable cannot be optimized away by the compiler. 

In the context of the APIC macros, `virtualBase` is used as the base address for accessing the APIC registers. The kernel maps the physical address of the APIC registers to a virtual address during system initialization, and this virtual address is stored in `virtualBase`.

> `void SpuriousInterruptHandler(void*, RegisterContext* r) { Log::Warning("[APIC] Spurious Interrupt"); }`

The function takes two parameters: a void pointer named `void*` and a pointer to a structure named `RegisterContext` named "r". 

Inside the function, a warning message is logged using the `Log::Warning` function with the message "[APIC] Spurious Interrupt". This indicates that a spurious interrupt has occurred in the APIC (Advanced Programmable Interrupt Controller).

> `uint64_t ReadBase()`

This function reads the value of the MSR (Model Specific Register) with index 0x1B, which is the `IA32_PERF_FIXED_CTR0` register on x86 processors. This register contains the count of retired instructions executed by the processor core since the last reset.

The function uses inline assembly to execute the rdmsr instruction, which reads the value of the MSR into the `EDX:EAX` registers. The low and high parts of the value are then stored in the local variables low and high, respectively.

Finally, the function combines the low and high parts into a 64-bit value using a bitwise OR operation and returns the result.

Note that this function requires privileged access to execute the rdmsr instruction, and may cause a general protection fault if executed in user mode.

> `void WriteBase(uint64_t val)`

This function writes a 64-bit value to a model-specific register (MSR) using the WRMSR instruction.

The WRMSR instruction takes three operands: `EAX`, `EDX`, and `ECX`. `EAX` and `EDX` contain the low and high 32 bits of the 64-bit value, respectively, while `ECX` specifies the MSR address to write to.

The function first splits the input value into two 32-bit values, 'low' and 'high', using bitwise AND and right shift operations. It then passes these values and the MSR address (0x1B) to the WRMSR instruction using inline assembly.

Overall, this function is used to directly write a 64-bit value to a specific MSR address in the processor.

> `void Enable()`

The `Enable()` function first reads the current value of the APIC base address register using the `ReadBase()` function. It then sets the 11th bit of this value to 1 using a bitwise OR operation with the value 1 shifted left by 11 bits. This effectively enables the APIC on the processor.

Next, the function writes a value to the Spurious Interrupt Vector Register (SIVR) of the local APIC. This register controls the behavior of the APIC when a spurious interrupt (an interrupt that is not expected or intended) occurs. The value written to the SIVR register enables the APIC and sets the spurious interrupt vector to 255.

> `void Initialize()`

The `Initialize()` function first reads the APIC base address using `ReadBase()` and stores it in the `Local::base` variable. It then maps the physical address of the APIC base to a virtual address using the `Memory::GetIOMapping()` function and stores this virtual address in the virtualBase variable.

If the debugLevelInterrupts variable is greater than or equal to `DebugLevelNormal`, the function logs the APIC base address and virtual address using the `Log::Info()` function.

The function then registers a handler for the spurious interrupt (vector 0xFF) using the `IDT::RegisterInterruptHandler()` function. This handler simply calls the `SpuriousInterruptHandler()` function.

Finally, the function calls the `Enable()` function to enable the APIC and returns 0 to indicate success.

> `void SendIPI(uint8_t destination, uint32_t dsh /* Destination Shorthand*/, uint32_t type, uint8_t vector) `

Function used to send an Inter-Processor Interrupt (IPI) to a specific processor or group of processors. 

The function takes in four parameters: 
- `destination` is the ID of the processor or group of processors that the IPI should be sent to. 
- `dsh` is a bit field that specifies the destination mode of the IPI. 
- `type` is a bit field that specifies the type of IPI to send. 
- `vector` is the interrupt vector number that the IPI should use. 

The function then constructs the high and low portions of the Interrupt Command Register (ICR) message that will be sent to the Local APIC (Advanced Programmable Interrupt Controller) to initiate the IPI. The high portion is constructed by left shifting the `destination` parameter by 24 bits, while the low portion is constructed by ORing together the `dsh`, `type`, and `vector` parameters with a constant that represents the ICR vector format. 

Finally, the function writes the high and low portions of the ICR message to the appropriate registers on the `Local APIC` using the `APIC_WRITE` macro.

 ::: **End: namespace Local** :::

 ::: **Begin: namespace IO** :::

Briefly,

This code defines a class for handling Input/Output Advanced Programmable Interrupt Controller (I/O APIC) interrupts. The class provides methods for reading and writing to I/O APIC registers, redirecting interrupts to specific interrupt vectors, initializing the I/O APIC, setting the I/O APIC base address, and mapping legacy IRQs. The class also defines variables for storing the I/O APIC base address, the virtual base address, the register select pointer, the I/O window pointer, the number of interrupts available, and the APIC ID. The class uses the ACPI (Advanced Configuration and Power Interface) to obtain interrupt source overrides and map legacy IRQs. The code includes error handling for attempting to initialize the I/O APIC without setting the base address and asserts that reading 64-bit registers is unimplemented. Overall, this code provides a useful abstraction for working with I/O APIC interrupts.

::: **End: namespace IO** :::

> `int Initialize()`

The function `Initialize()` checks if the CPU has the Advanced Programmable Interrupt Controller (APIC) feature enabled. If it is not present, it returns an error and exits. If it is present, it disables the interrupt flag (cli), disables the Programmable Interrupt Controller (PIC), initializes the Local and IO APICs, and sets the interrupt flag (sti). It returns 0 if the initialization was successful.

::: **End: namespace APIC** :::

> `extern "C" void LocalAPICEOI() { APIC_WRITE(LOCAL_APIC_EOI, 0); }`

It defines a function called "LocalAPICEOI" that takes no arguments and returns nothing.

The function calls the "APIC_WRITE" macro with two arguments: `LOCAL_APIC_EOI` and 0. This macro writes the value 0 to some register or memory location related to the local Advanced Programmable Interrupt Controller (APIC).

This function to be responsible for acknowledging an interrupt that was handled by the local APIC. When an interrupt is triggered, the APIC sends a signal to the CPU to handle it. Once the CPU has handled the interrupt, it must send a signal back to the APIC to acknowledge that the interrupt has been handled. This function performs that acknowledgement.

[To the begining](#exit)

## <a name="3">CPUID.cpp</a>

This code defines a function called `CPUID` that returns a struct `cpuid_info_t`. The function uses inline assembly to execute the `cpuid` instruction, which is used to obtain information about the CPU. 

The first `cpuid` instruction is used to get the vendor string, which is stored in the `ebx`, `edx`, and `ecx` registers. The function then copies the string from these registers into the `info.vendorString` buffer. 

The second `cpuid` instruction is used to get the CPU features, which are stored in the `edx` and `ecx` registers. The function stores these values in the `info.features_edx` and `info.features_ecx` fields of the `cpuid_info_t` struct. 

Overall, this function provides a way to obtain information about the CPU, which can be useful for system monitoring, optimization, and other purposes.

[To the begining](#exit)

## <a name="4">IDT.asm</a>

This code defines Interrupt Service Routines (ISRs) and Interrupt Request Handlers (IRQs) for handling hardware and software interrupts in a 64-bit operating system. It also defines an Interrupt Request Handler for system calls `SyscallHandler`. The code creates a table of interrupt vectors `int_vectors` that contain the addresses of the ISRs and IRQs. The `idt_flush` function is used to load the Interrupt Descriptor Table (IDT) with the interrupt vectors. The `LocalAPICEOI` function is used to send an End of Interrupt (EOI) signal to the Local APIC (Advanced Programmable Interrupt Controller) after an interrupt has been handled.

[To the begining](#exit)

## <a name="5">IDT.cpp</a>

This code defines the Interrupt Descriptor Table (IDT) and Interrupt Service Routines (ISRs) for handling interrupts and exceptions in an operating system. 

The IDT is an array of 256 entries, each representing a different interrupt or exception. Each entry contains information about how to handle the interrupt, such as the address of the ISR associated with it. 

The `ISRDataPair` struct is used to store a pair of an ISR function pointer and an additional data pointer that can be passed to the ISR when it is called. 

The `interruptHandlers` array is an array of 256 `ISRDataPairs`, representing the handlers for each interrupt or exception. 

> `extern "C"`

- `isr0` - `isr31` are processor interrupts that are called when certain events occur, such as division by zero, memory protection violation, etc.

- `irq0` - `irq15` are hardware interrupts that are called when a signal occurs from input / output devices such as keyboard, mouse, hard disk, etc.

- `isr0x69` is a specific processor interrupt that is called when a processor error occurs due to the incorrect use of a processor instruction. This interrupt can be triggered, for example, when trying to execute an instruction that cannot be executed on the given processor.

> `extern uint64_t int_vectors[];`

The variable contains a list of memory addresses that correspond to interrupt service routines (ISRs) for a particular processor architecture. When an interrupt occurs, the processor jumps to the memory address stored in the appropriate slot of the `int_vectors` array to execute the corresponding ISR.

::: **Begin: namespace IDT** :::

This code initializes the Interrupt Descriptor Table (IDT) for an x86 processor. The IDT is a data structure that contains entries for each interrupt vector, with each entry containing the memory address of the interrupt handler function to be executed when the interrupt occurs. 

The code defines two interrupt handler functions: `IPIHalt` and `InvalidInterruptHandler`. The former function disables interrupts and halts the processor, while the latter function simply logs a warning message indicating that an invalid interrupt handler was called. 

The `SetGate` function is used to set the values of an IDT entry. It takes as input the interrupt vector number, the memory address of the interrupt handler function, the selector for the code segment, the flags for the IDT entry, and an optional Interrupt Stack Table (IST) index. The function sets the appropriate values in the IDT entry. 

The `Initialize` function sets up the IDT for use. It sets the limit and base address of the IDT pointer, and then calls `SetGate` for each IDT entry, setting the appropriate values for the interrupt handler function address, selector, and flags. The first 48 IDT entries are set to zero (reserved for processor exceptions), while the remaining entries are set to the memory addresses of the interrupt service routines stored in the `int_vectors` array.

```C++
SetGate(0, (uint64_t)isr0, 0x08, 0x8E);
SetGate(1, (uint64_t)isr1, 0x08, 0x8E);
SetGate(2, (uint64_t)isr2, 0x08, 0x8E);
SetGate(3, (uint64_t)isr3, 0x08, 0x8E);
SetGate(4, (uint64_t)isr4, 0x08, 0x8E);
SetGate(5, (uint64_t)isr5, 0x08, 0x8E);
SetGate(6, (uint64_t)isr6, 0x08, 0x8E);
SetGate(7, (uint64_t)isr7, 0x08, 0x8E);
SetGate(8, (uint64_t)isr8, 0x08, 0x8E, 2); 
SetGate(9, (uint64_t)isr9, 0x08, 0x8E);
SetGate(10, (uint64_t)isr10, 0x08, 0x8E);
SetGate(11, (uint64_t)isr11, 0x08, 0x8E);
SetGate(12, (uint64_t)isr12, 0x08, 0x8E);
SetGate(13, (uint64_t)isr13, 0x08, 0x8E);
SetGate(14, (uint64_t)isr14, 0x08, 0x8E);
SetGate(15, (uint64_t)isr15, 0x08, 0x8E);
SetGate(16, (uint64_t)isr16, 0x08, 0x8E);
SetGate(17, (uint64_t)isr17, 0x08, 0x8E);
SetGate(18, (uint64_t)isr18, 0x08, 0x8E);
SetGate(19, (uint64_t)isr19, 0x08, 0x8E);
SetGate(20, (uint64_t)isr20, 0x08, 0x8E);
SetGate(21, (uint64_t)isr21, 0x08, 0x8E);
SetGate(22, (uint64_t)isr22, 0x08, 0x8E);
SetGate(23, (uint64_t)isr23, 0x08, 0x8E);
SetGate(24, (uint64_t)isr24, 0x08, 0x8E);
SetGate(25, (uint64_t)isr25, 0x08, 0x8E);
SetGate(26, (uint64_t)isr26, 0x08, 0x8E);
SetGate(27, (uint64_t)isr27, 0x08, 0x8E);
SetGate(28, (uint64_t)isr28, 0x08, 0x8E);
SetGate(29, (uint64_t)isr29, 0x08, 0x8E);
SetGate(30, (uint64_t)isr30, 0x08, 0x8E);
SetGate(31, (uint64_t)isr31, 0x08, 0x8E);
SetGate(0x69, (uint64_t)isr0x69, 0x08, 0xEE, 0);
```

The code above sets up Interrupt Service Routines (ISRs) for the CPU. These ISRs are used to handle interrupts and exceptions that occur during program execution. 

Each line of code sets up a single ISR, identified by its interrupt number (0-31 and 0x69 for syscall). The `SetGate` function is called with four arguments: 

1. The interrupt number
2. The address of the ISR function
3. The segment selector (0x08, which is the code segment for kernel mode)
4. The interrupt gate descriptor (0x8E for normal ISRs and 0xEE for syscall ISR)

For the Double Fault ISR (interrupt number 8), there is an additional fifth argument of 2, which sets the interrupt gate descriptor to 0x8E and sets the stack pointer to the kernel stack.

The syscall ISR (interrupt number 0x69) is given a special interrupt gate descriptor (0xEE) that allows it to be called from user mode.

This code is setting up the Interrupt Descriptor Table (IDT) for handling hardware interrupts on an x86 processor. 

```C++
asm volatile("lidt %0;" ::"m"(idtPtr));

outportb(0x20, 0x11);
outportb(0xA0, 0x11);
outportb(0x21, 0x20);
outportb(0xA1, 0x28);
outportb(0x21, 0x04);
outportb(0xA1, 0x02);
outportb(0x21, 0x01);
outportb(0xA1, 0x01);
outportb(0x21, 0x0);
outportb(0xA1, 0x0);

SetGate(32, (uint64_t)irq0, 0x08, 0x8E);
SetGate(33, (uint64_t)irq1, 0x08, 0x8E);
SetGate(34, (uint64_t)irq2, 0x08, 0x8E);
SetGate(35, (uint64_t)irq3, 0x08, 0x8E);
SetGate(36, (uint64_t)irq4, 0x08, 0x8E);
SetGate(37, (uint64_t)irq5, 0x08, 0x8E);
SetGate(38, (uint64_t)irq6, 0x08, 0x8E);
SetGate(39, (uint64_t)irq7, 0x08, 0x8E);
SetGate(40, (uint64_t)irq8, 0x08, 0x8E);
SetGate(41, (uint64_t)irq9, 0x08, 0x8E);
SetGate(42, (uint64_t)irq10, 0x08, 0x8E);
SetGate(43, (uint64_t)irq11, 0x08, 0x8E);
SetGate(44, (uint64_t)irq12, 0x08, 0x8E);
SetGate(45, (uint64_t)irq13, 0x08, 0x8E);
SetGate(46, (uint64_t)irq14, 0x08, 0x8E);
SetGate(47, (uint64_t)irq15, 0x08, 0x8E);

RegisterInterruptHandler(IPI_HALT, IPIHalt);
```

The first line sets the IDT pointer to the address of the IDT structure. 

The next 10 lines set up the Programmable Interrupt Controller (PIC) to handle hardware interrupts. The PIC is a chip that manages interrupts from hardware devices and sends them to the processor. 

The last 16 lines set up the IDT entries for the 16 hardware interrupts `IRQ0-IRQ15`. Each entry specifies the interrupt number, the address of the interrupt handler function, and the privilege level and gate type. 

Finally, the `IPI_HALT` interrupt is registered with the `IPIHalt` function as its handler. It's a custom interrupt used for halting the processor.

> `void RegisterInterruptHandler(uint8_t interrupt, isr_t handler, void* data)`

This function registers an interrupt handler for a given interrupt number. The interrupt number is passed as the first argument, and the handler function and associated data are passed as the second and third arguments, respectively.

The function first checks if a handler has already been registered for the given interrupt number. If so, it logs a warning message and does not register the new handler. If no handler has been registered, it logs an information message and proceeds to register the new handler by storing it and its associated data in the interruptHandlers array at the corresponding index.

Note that the `interruptHandlers` array is assumed to be a global array of structures, where each structure contains a handler function pointer and a data pointer. The `InvalidInterruptHandler` constant is assumed to be defined elsewhere as a sentinel value to indicate that no handler has been registered for a given interrupt number.

> `uint8_t ReserveUnusedInterrupt()`

This function reserves an unused interrupt by searching for an available interrupt starting from `IRQ0 + 16` up to 100. If an available interrupt is found, the corresponding interrupt handler is set to an invalid handler function and the interrupt number is returned. If no available interrupt is found, it returns 0xFF indicating that no interrupt is available. 

The function takes no input parameters and returns an 8-bit unsigned integer representing the interrupt number.

> `void DisablePIC()`

This function disables the Programmable Interrupt Controller (PIC) on the x86 architecture. The PIC is responsible for handling interrupts from hardware devices, and by disabling it, the system will not respond to hardware interrupts.

The function achieves this by sending specific commands to the PIC through the x86 I/O ports. The first two commands (0x11) tell the PIC that we want to set up its configuration. The next two commands (0xF0) set the interrupt mask for each PIC. The next two commands (0x04 and 0x02) set the PIC's operating mode. The final two commands (0x01 and 0xFF) tell the PIC to finish its configuration and set the interrupt mask to its default value.

Overall, this function is useful in situations where we want to disable hardware interrupts, such as in certain types of debugging or testing scenarios.

::: **End: namespace IDT** :::

> `extern "C" void isr_handler(int intNum, RegisterContext* regs)`

This function is an interrupt service routine (ISR) that handles hardware interrupts in the kernel. It takes two parameters: the interrupt number `intNum` and a pointer to the register context `regs` at the time the interrupt occurred.

The first thing the function does is check if there is a registered handler for this interrupt number. If there is, it calls the handler function with the provided data and the register context.

If there is no registered handler, the function checks if the segment's privilege level (CPL) is 0, indicating that the interrupt occurred in kernel mode. If it did, the function logs a kernel panic message and halts other processors. It then prints out information about the exception that occurred, including the RIP (instruction pointer), error code, and register dump. Finally, it prints a stack trace and calls the `KernelPanic` function to halt the system.

If the interrupt did not occur in kernel mode, the function acquires a lock and checks the current process's state. If it is not running, the function enters an infinite loop. Otherwise, it logs a warning message about the crashed process, prints out information about the exception, including a stack trace, and terminates the process by calling the `Die` function.

> `extern "C" void irq_handler(int int_num, RegisterContext* regs)`

This is a function definition for an interrupt service routine (ISR). The function is called `irq_handler` and takes two parameters: an integer `int_num` representing the interrupt number, and a pointer to a `RegisterContext` struct which contains the register values at the time of the interrupt.

The first line of the function calls `LocalAPICEOI()`, which is likely a function that acknowledges the interrupt at the local APIC (Advanced Programmable Interrupt Controller) level. This is necessary to prevent the interrupt from being re-triggered.

The next block of code checks if there is an interrupt handler registered for the given interrupt number. If there is, the corresponding handler function is called with the provided `data` parameter (which could be any type of data), as well as the register context struct.

If there is no registered handler for the interrupt, a warning message is logged indicating that the interrupt was unhandled.

Overall, this function is used to handle interrupts in a system by calling registered handler functions.

> `extern "C" void ipi_handler(int int_num, RegisterContext* regs)`

This is a function definition for a handler function called `ipi_handler`, which is designed to handle Inter-Processor Interrupts (IPIs). 

The function takes two parameters: an integer representing the interrupt number `int_num` and a pointer to a `RegisterContext` object (regs). 

The first line of the function calls a function called `LocalAPICEOI()`, which is responsible for acknowledging the interrupt and clearing the interrupt flag. 

The function then checks if there is a registered handler for the interrupt number using an if statement and the `__builtin_expect()` function, which is a compiler optimization that helps predict the outcome of a branch. If there is a registered handler, the function retrieves the handler and its associated data from an array called `interruptHandlers` and calls the handler function, passing in the data and the `RegisterContext` pointer as arguments. 

If there is no registered handler, the function logs a warning message indicating that the IPI was unhandled.

[To the begining](#exit)

## <a name="6">PCI.cpp</a>

::: **Begin: namespace PCI** :::

- `devicesLock`: a lock object of type `lock_t`
- `devices`: a pointer to a `Vector` object that contains `PCIInfo` elements
- `unknownDevice`: a pointer to a `PCIInfo` object that represents an unknown device
- `mcfgTable`: a pointer to a `PCIMCFG` object that represents the MCFG table
- `configMode`: a variable of type `PCIConfigurationAccessMode` that specifies the mode of PCI configuration access (either Legacy or Enhanced)
- `enhancedBaseAddresses`: a pointer to a `Vector` object that contains `PCIMCFGBaseAddress` elements.

> `uint32_t ConfigReadDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)`

This function reads a 32-bit value from a configuration register of a PCI device. The function takes four input parameters: bus, slot, function, and offset. These parameters specify the location of the configuration register in the PCI configuration space. The function constructs a 32-bit address by combining these parameters and sets the address in the PCI configuration address register (0xCF8). It then reads the 32-bit value from the configuration register using the PCI configuration data register (0xCFC) and returns the value. 

Note that the function sets the most significant bit of the address (bit 31) to 1, which indicates that it is a configuration access. The function also masks the two least significant bits of the offset parameter (bits 1:0) to ensure that the address is aligned to a 4-byte boundary.

> `uint16_t ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)`

This function reads a 16-bit word from the configuration space of a PCI device, identified by its bus number, slot number, function number, and offset within the configuration space.

The function first constructs a 32-bit address by combining the bus, slot, function, and offset parameters, and setting the "enable bit" in the most significant bit position. This address is then written to the PCI configuration address port (0xCF8) using the outportl() function.

Next, the function reads a 32-bit value from the PCI configuration data port (0xCFC) using the inportl() function. The relevant 16-bit word is extracted from this value using bitwise operations based on the offset parameter, and returned to the caller.

Note that the function assumes that the PCI configuration address and data ports are mapped to the standard I/O port addresses used by the x86 architecture. The function also assumes that the caller has appropriate permissions to access the PCI configuration space.

> `uint8_t ConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)`

This function is similar to the previous one, but it reads a single byte (8 bits) from the PCI configuration space instead of a 16-bit word.

The function constructs the same 32-bit address as before, and writes it to the PCI configuration address port. It then reads a 32-bit value from the PCI configuration data port, and extracts the relevant byte using bitwise operations based on the offset parameter.

Function uses a different mask (0xfc) to ensure that the offset parameter is aligned to a 4-byte boundary, as the PCI configuration space only allows access to 4-byte aligned addresses. The function also uses a different bitwise operation to extract the relevant byte from the 32-bit value, based on the offset parameter modulo 4.

As with the previous function, the function assumes that the PCI configuration address and data ports are mapped to the standard I/O port addresses used by the x86 architecture, and that the caller has appropriate permissions to access the PCI configuration space.

> `void ConfigWriteDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data)`

This function writes a 32-bit (4-byte) value to the configuration space of a PCI device, identified by its bus number, slot number, function number, and offset within the configuration space.

The function constructs the same 32-bit address as the previous functions, and writes it to the PCI configuration address port. It then writes the 32-bit value to the PCI configuration data port using the outportl() function.

Also note that the function only writes a 16-bit value to the PCI configuration space, as the outportl() function writes a 32-bit value. The upper 16 bits of the 32-bit value are ignored by the PCI device.

> `void ConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t data)`

The function first constructs a 32-bit address by combining the bus, slot, function, and offset parameters. The address is then written to the PCI configuration address port (0xCF8) using the outportl function.

The data parameter is written to the PCI configuration data port (0xCFC) using the outportl function. Before writing the data, the function first reads the current value of the register from the data port and masks out the bits that correspond to the 16-bit word being written. The new data is then shifted to the appropriate position and ORed with the masked value to produce the final data to be written.

> `void ConfigWriteByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t data)`

Like the previous function, the function constructs a 32-bit address by combining the bus, slot, function, and offset parameters, and writes it to the PCI configuration address port (0xCF8) using the outportl function.

The data parameter is written to the PCI configuration data port (0xCFC) using the outportb function. Before writing the data, the function first reads the current value of the register from the data port and masks out the bits that correspond to the byte being written. The new data is then shifted to the appropriate position and ORed with the masked value to produce the final data to be written.

> Two functions


```C++
uint16_t GetVendor(uint8_t bus, uint8_t slot, uint8_t func) 
{
    uint16_t vendor;

    vendor = ConfigReadWord(bus, slot, func, PCIVendorID);
    return vendor;
}

uint16_t GetDeviceID(uint8_t bus, uint8_t slot, uint8_t func) 
{
    uint16_t id;

    id = ConfigReadWord(bus, slot, func, PCIDeviceID);
    return id;
}
```

The code defines two functions, namely `GetVendor` and `GetDeviceID`, that take three arguments each: `bus`, `slot`, and `func`, all of type `uint8_t`. 

The `GetVendor` function reads the Vendor ID of a PCI device using the `ConfigReadWord` function and returns it as a `uint16_t` variable. Similarly, the `GetDeviceID` function reads the Device ID of a PCI device using the `ConfigReadWord` function and returns it as a uint16_t variable. 

It is assumed that the `ConfigReadWord` function is defined elsewhere in the code and takes four arguments: `bus`, `slot`, `func`, and `offset`, and returns a uint16_t value. The function is used to read the configuration space of a PCI device.

> `uint8_t GetClassCode(uint8_t bus, uint8_t slot, uint8_t func) { return ConfigReadByte(bus, slot, func, PCIClassCode); }`
> `uint8_t GetSubclass(uint8_t bus, uint8_t slot, uint8_t func) { return ConfigReadByte(bus, slot, func, PCISubclass); }`
> `uint8_t GetProgIf(uint8_t bus, uint8_t slot, uint8_t func) { return ConfigReadByte(bus, slot, func, PCIProgIF); }`
> `uint8_t GetHeaderType(uint8_t bus, uint8_t slot, uint8_t func) { return PCI::ConfigReadByte(bus, slot, func, PCIHeaderType); }`

**The first function**, `GetClassCode`, takes three arguments: bus, slot, and func, and returns the Class Code of the PCI device as a uint8_t variable. It uses the ConfigReadByte function to read the Class Code field from the configuration space. 

**The second function**, `GetSubclass`, takes the same arguments as GetClassCode and returns the Subclass of the PCI device as a uint8_t variable. It uses the ConfigReadByte function to read the Subclass field from the configuration space. 

**The third function**, `GetProgIf`, takes the same arguments as the previous two functions and returns the Programming Interface of the PCI device as a uint8_t variable. It uses the ConfigReadByte function to read the Programming Interface field from the configuration space. 

**The fourth function**, `GetHeaderType`, takes the same arguments as the previous three functions and returns the Header Type of the PCI device as a uint8_t variable. It uses the PCI::ConfigReadByte function to read the Header Type field from the configuration space. 

Overall, the functions are used to extract specific information about a PCI device's configuration and can be used in a larger program that deals with PCI devices.

> `bool CheckDevice(uint8_t bus, uint8_t device, uint8_t func)`
> `bool FindDevice(uint16_t deviceID, uint16_t vendorID)`
> `bool FindGenericDevice(uint16_t classCode, uint16_t subclass)`

These functions check and find devices on a PCI bus. The `CheckDevice` function checks if a device is present on the specified bus, device, and function. The `FindDevice` function searches for a specific device with a given device ID and vendor ID. The `FindGenericDevice` function searches for a device with a specific class code and subclass. All three functions use a `ScopedSpinLock` to protect the devices list from concurrent access.

> `const PCIInfo& GetPCIDevice(uint16_t deviceID, uint16_t vendorID)`
> `const PCIInfo& GetGenericPCIDevice(uint8_t classCode, uint8_t subclass)`

**The first function**, `GetPCIDevice`, takes in two parameters: `deviceID` and `vendorID`, both of type `uint16_t`. It returns a constant reference to a `PCIInfo` object, which contains information about the PCI device with the specified device and vendor IDs. The function first acquires a lock on the `devicesLock` mutex, which ensures that only one thread can access the `devices` vector at a time. It then loops through all the `PCIInfo` objects in the `devices` vector, comparing their `deviceID` and `vendorID` fields to the specified values. If it finds a match, it returns a constant reference to that `PCIInfo` object. If it doesn't find a match, it logs an error message and returns a constant reference to the `unknownDevice` object.

**The second function**, `GetGenericPCIDevice`, takes in two parameters: `classCode` and `subclass`, both of type `uint8_t`. It also returns a constant reference to a `PCIInfo` object, which contains information about a PCI device with the specified class and subclass codes. Like the previous function, it acquires a lock on the `devicesLock` mutex and loops through all the `PCIInfo` objects in the `devices` vector, comparing their `classCode` and `subclass` fields to the specified values. If it finds a match, it returns a constant reference to that `PCIInfo` object. If it doesn't find a match, it logs an error message and returns a constant reference to the `unknownDevice` object. 

**Both functions** use a `ScopedSpinLock` object to ensure that the `devices` vector is accessed safely and that no other thread can modify it while they are looping through its contents. If a matching `PCIInfo` object is found, a constant reference to that object is returned, which prevents the caller from modifying it. If no matching object is found, a constant reference to the `unknownDevice` object is returned, which ensures that the caller always receives a valid `PCIInfo` object.

> `void EnumeratePCIDevices(uint16_t deviceID, uint16_t vendorID, void (*func)(const PCIInfo&))`
> `void EnumerateGenericPCIDevices(uint8_t classCode, uint8_t subclass, void (*func)(const PCIInfo&))`

These functions are used to enumerate PCI devices based on their device ID, vendor ID, class code, and subclass. 

**The first function**, `EnumeratePCIDevices()`, takes in a device ID and vendor ID and a function pointer to a function that takes in a `PCIInfo` object as an argument. It then locks the `devicesLock` mutex to prevent multiple threads from accessing the devices vector at the same time. It then iterates through the devices vector and calls the function pointer on any device that matches the given device ID and vendor ID. 

**The second function**, `EnumerateGenericPCIDevices()`, takes in a class code and subclass and a function pointer to a function that takes in a `PCIInfo` object as an argument. It also locks the `devicesLock` mutex before iterating through the devices vector and calling the function pointer on any device that matches the given class code and subclass. 

**Both functions** use a `ScopedSpinLock` object to automatically lock and unlock the devicesLock mutex when the function goes out of scope. This ensures that the mutex is always released, even if an exception is thrown.

> `int AddDevice(int bus, int slot, int func)`

This function is used to add a new PCI device to the devices vector. 

It takes in the bus, slot, and function numbers of the device and uses them to retrieve the device's vendor ID, device ID, class code, subclass, and programming interface using various Get functions. 

It then creates a new PCIInfo object and fills in its fields with the retrieved information and the given bus, slot, and function numbers. 

The function then locks the `devicesLock` mutex using a `ScopedSpinLock` object to prevent multiple threads from accessing the devices vector at the same time. It retrieves the current length of the devices vector and adds the new device to the back of the vector using the `add_back()` function. Finally, it returns the index of the new device in the vector. 

**This function assumes that the devices vector has already been initialized and is not null. If it is null, the function will cause a segmentation fault.**

> `void Init()`

The `Init()` function initializes the PCI subsystem by performing the following tasks:

1. Creates a new PCIInfo object for an unknown device with vendor ID and device ID set to 0xFFFF.
2. Creates a new Vector of PCIInfo objects to store the information of all the detected PCI devices.
3. Creates a new Vector of `PCIMCFGBaseAddress` objects to store the base addresses of devices in Enhanced Configuration Access Mode.
4. Retrieves the MCFG (PCI Express Memory Mapped Configuration Space) table from ACPI (Advanced Configuration and Power Interface).
5. If the MCFG table is present, sets the configuration access mode to Enhanced and adds the base addresses of all devices with segment group number 0 to the `enhancedBaseAddresses` Vector.
6. Iterates through all possible combinations of bus, slot, and function numbers to detect all PCI devices present in the system.
7. For each detected device, adds its information to the devices Vector and checks if it has multiple functions (header type bit 7 set).
8. If the device has multiple functions, iterates through all the functions and adds their information to the devices Vector.

The Init() function initializes the PCI subsystem by detecting all the PCI devices present in the system and storing their information in the devices Vector. It also sets the configuration access mode to Enhanced if the MCFG table is present.

::: **End: namespace PCI** :::

> `PCIDevice::PCIDevice(uint8_t _bus, uint8_t _slot, uint8_t _func) : bus(_bus), slot(_slot), func(_func)`

This code defines the constructor for the PCIDevice class, which represents a PCI device. The constructor takes three arguments: the bus number, the slot number, and the function number of the device. These values are used to read the device's configuration space using the `PCI::ConfigRead*()` functions, which are provided by the system's PCI driver.

The constructor initializes several member variables of the PCIDevice object, including the device ID and vendor ID, which identify the device and its manufacturer, respectively. It also reads the device's class code, subclass, and programming interface, which describe the device's function.

The constructor checks whether the device supports MSI (Message Signaled Interrupts) by checking the `PCI_STATUS_CAPABILITIES` bit in the device's status register. If the device supports MSI, the constructor reads the MSI capability structure from the device's configuration space and stores it in the msiCap member variable. The constructor also populates the capabilities vector with the IDs of all capabilities supported by the device.

> `uint8_t PCIDevice::AllocateVector(PCIVectors type)`

This function is a method of the PCIDevice class that allocates an interrupt vector for the device. The input parameter is a PCIVectors enum that specifies the type of interrupt vector to allocate, either MSI or Legacy.

If the input parameter is MSI, the function first checks if the device is capable of using MSIs. If not, it logs an error message and returns 0xFF. If the device is MSI capable, the function reserves an unused interrupt from the Interrupt Descriptor Table (IDT) using the `IDT::ReserveUnusedInterrupt()` method. If no unused interrupt is found, the function logs an error message and returns 0xFF.

Next, the function configures the MSI Capability structure of the device to enable MSIs and set the interrupt vector for the message. It sets the MSI address to CPU 0 and writes the MSI Capability structure registers to the device's configuration space using the `PCI::ConfigWriteDword()` method.

If the input parameter is Legacy, the function gets the interrupt line assigned to the device and maps it to the corresponding IRQ line using the `APIC::IO::MapLegacyIRQ()` method. The function returns the IRQ line number with the IRQ0 bit set.

If the input parameter is neither MSI nor Legacy, the function logs an error message and returns 0xFF.

[To the begining](#exit)

## <a name="7">syscall.asm</a>

This code is the entry point for handling system calls in the kernel. 

It begins by swapping the GS register with the kernel's own GS base, which allows access to per-CPU state. 

It then saves the current stack pointer and loads the TSS stack pointer for the current CPU. It sets up the user-mode stack by pushing the user-mode stack segment selector and the saved stack pointer onto the new stack. 

It then pushes all the general-purpose registers onto the stack, sets up the necessary arguments for the `SyscallHandler` function, and calls it.

After the function returns, it pops all the general-purpose registers off the stack, adjusts the stack pointer, and returns to user mode using the sysret instruction.

## <a name="8">EPoll.cpp</a>

> `long SysEpollCreate(RegisterContext* r)`

The function `SysEpollCreate` creates an epoll instance and returns a file descriptor that can be used to refer to this instance in subsequent system calls. The function takes a single argument `flags`, which is a bit mask that can be used to set various options for the epoll instance. The only option currently supported is `EPOLL_CLOEXEC`, which sets the `O_CLOEXEC` flag on the file descriptor returned by the function.

First, the function retrieves the current process using the `Process::Current()` method. It then checks if the `flags` argument contains any unsupported flags by performing a bitwise AND operation with the complement of `EPOLL_CLOEXEC` (i.e., all bits except the one corresponding to `EPOLL_CLOEXEC`). If any bits are set in the result, it returns an error code `-EINVAL`.

Next, the function creates a new `fs::EPoll` instance using the `new` operator. This creates a new instance of an epoll object that can be used to manage file descriptors. It then calls the `Open` method on the epoll object with a file descriptor value of `0`. This is used to create a new file descriptor that refers to the epoll object. The resulting file descriptor is stored in a `FancyRefPtr<UNIXOpenFile>` object named `handle`.

The `node` member of the `UNIXOpenFile` object is set to the epoll object, and the `mode` member is set to `0`. If the `EPOLL_CLOEXEC` flag is set in the `flags` argument, the `O_CLOEXEC` flag is set in the `mode` member of the `UNIXOpenFile` object.

Finally, the function allocates a new file descriptor for the process using the `AllocateHandle` method of the `Process` object, passing in the `handle` object as an argument. The resulting file descriptor is returned as the result of the function.

> `long SysEPollCtl(RegisterContext* r)`

The `SysEPollCtl` function is a system call handler that implements the `epoll_ctl` system call, which is used to add, modify, or remove file descriptors from an epoll instance. The function first retrieves the arguments passed by the user and checks if the file descriptors are valid. If either of them is not valid, it returns the error code `-EBADF`.

The function then checks if the file descriptor being added or modified is an epoll instance. If it is, it returns an error code since it is not supported to add an epoll instance to another epoll instance. If the file descriptor is the same as the epoll file descriptor, it returns `-EINVAL` since it is not allowed to add or remove the epoll file descriptor itself.

If the operation is to add a file descriptor, the function retrieves the event data from the user and checks if the `EPOLLEXCLUSIVE` and `EPOLLET` flags are set. If they are, it returns an error code since they are not supported. It then checks if the file descriptor is already in the epoll instance, and if it is, it returns `-EEXIST`. If not, it adds the file descriptor and event data to the epoll instance.

If the operation is to remove a file descriptor, the function searches for the file descriptor in the `epoll` instance and removes it if found. If not found, it returns `-ENOENT`.

If the operation is to modify a file descriptor, the function retrieves the current event data for the file descriptor and checks if the `EPOLLEXCLUSIVE` flag is set. If it is, it returns an error code. It then searches for the file descriptor in the epoll instance and modifies its event data if found. If not found, it returns `-ENOENT`.

If the operation is not recognized, the function returns `-EINVAL`.

> `long SysEpollWait(RegisterContext* r)`

```C++
Process* proc = Process::Current();
Thread* thread = Thread::Current();

int epfd = SC_ARG0(r);
UserBuffer<struct epoll_event> events = SC_ARG1(r);
int maxevents = SC_ARG2(r);
long timeout = SC_ARG3(r) * 1000;

auto epHandle = SC_TRY_OR_ERROR(proc->GetHandleAs<UNIXOpenFile>(epfd));
if (!epHandle) 
{
    return -EBADF;
} else if (!epHandle->node->IsEPoll()) {
    Log::Debug(debugLevelSyscalls, DebugLevelVerbose, "Not an epoll device!");
    return -EINVAL;
}

if (maxevents <= 0) 
{
    Log::Debug(debugLevelSyscalls, DebugLevelVerbose, "maxevents cannot be <= 0");
    return -EINVAL;
}

fs::EPoll* epoll = (fs::EPoll*)epHandle->node;

int64_t sigmask = SC_ARG4(r);
int64_t oldsigmask = 0;
if (sigmask) 
{
    oldsigmask = thread->signalMask;
    thread->signalMask = sigmask;
}

OnCleanup([sigmask, oldsigmask, thread]() 
{
    if (sigmask) 
    {
        thread->signalMask = oldsigmask;
    }
});

ScopedSpinLock lockEp(epoll->epLock);
```

The code retrieves the current process and thread and the arguments passed to the syscall.

It then retrieves the file descriptor for the epoll device and checks if it is a valid device. If it is not valid, it returns an error. If it is valid, it retrieves the EPoll object associated with the file descriptor.

It checks if the maxevents argument is valid and if it is not, it returns an error. If a signal mask is provided, it sets the current thread's signal mask to it and stores the old mask.

It then registers a cleanup function to restore the old signal mask after the syscall completes.

Finally, it acquires a lock on the EPoll object.

> > `Vector<int> removeFds;`

This code defines a lambda function named "getEvents" that takes no input parameters and returns a uint32_t value.

> > `auto getEvents = [removeFds](FancyRefPtr<UNIXOpenFile>& handle, uint32_t requested, int& evCount) -> uint32_t`

The function checks if the requested events include `EPOLLIN` or `EPOLLOUT` and if the corresponding handle node can read or write respectively. If the handle node is a socket and is not connected or listening, `EPOLLHUP` event is set. If there are pending connections and requested events include `EPOLLIN`, EPOLLIN event is set. 

The function also increments the event count if any event is set. The purpose of this function is to determine the events that should be removed from the epoll instance. 

The function is called on each file descriptor in the removeFds vector to determine the events that should be removed.

> > `auto epollToPollEvents = [](uint32_t ep) -> int`

This is a lambda function that takes an epoll event mask as input and converts it into a poll event mask. 

The function first initializes a variable `evs` to 0. It then checks each bit in the input `ep` using bitwise AND (`&`) with the corresponding epoll event flag. If the bit is set, it sets the corresponding bit in `evs` using bitwise OR (`|`) with the corresponding poll event flag. 

The function finally returns the resulting poll event mask. 

This conversion is useful when using a poll-based event loop with a file descriptor that has been added to an epoll instance, as the poll event mask can be used to detect the same events as the epoll event mask.

> > `struct EPollFD`

This is a struct definition for a data structure called EPollFD. It contains three members:

1. A `FancyRefPtr` object named handle, which is a smart pointer to a `UNIXOpenFile` object. This suggests that the `EPollFD` struct is used to represent a file descriptor that has been opened using the `UNIX open()` system call.

2. An integer named fd, which is the file descriptor number associated with the opened file.

3. An `epoll_event` object named ev, which is used to register events with the Linux epoll API. This suggests that the `EPollFD` struct is used to manage I/O events on the opened file descriptor.

```C++
Vector<EPollFD> files;
int evCount = 0; // Amount of fds with events
for (const auto& pair : epoll->fds) 
{
    auto result = proc->GetHandleAs<UNIXOpenFile>(pair.item1);
    if (result.HasError()) { continue; } // Ignore closed fds
    auto handle = std::move(result.Value());

    if (uint32_t ev = getEvents(handle, pair.item2.events, evCount); ev) 
    {
        if (events.StoreValue(evCount - 1, {
                                               .events = ev,
                                               .data = pair.item2.data,
        })) {
                return -EFAULT;
            }
    }

    if (evCount > 0) 
    {
        pair.item2.events & EPOLLONESHOT ? removeFds.add_back(pair.item1) : files.add_back ({ handle, pair.item1, pair.item2 });

        if (evCount >= maxevents) { goto done; }
    }

    if (evCount > 0) { goto done; }
    if (!evCount && timeout) 
    {
        FilesystemWatcher fsWatcher;
        for (auto& file : files) 
        {
            fsWatcher.WatchNode(file.handle->node, epollToPollEvents(file.ev.events));
        }

        while (!evCount) 
        {
            if (timeout > 0) 
            {
                if (fsWatcher.WaitTimeout(timeout)) 
                {
                    return -EINTR; // Interrupted
                } else if (timeout <= 0) {
                    return 0; // Timed out
                }
            } else if (fsWatcher.Wait()) {
                return -EINTR; // Interrupted
            }

            for (auto& handle : files) 
            {
                if (uint32_t ev = getEvents(handle.handle, handle.ev.events, evCount); ev) 
                {
                    if (events.StoreValue(evCount - 1, {
                                                           .events = ev,
                                                           .data = handle.ev.data,
                    })) {
                        return -EFAULT;
                        }
                 }

                 if (evCount > 0) 
                 {
                     if (handle.ev.events & EPOLLONESHOT) 
                     {
                         removeFds.add_back(handle.fd);
                     }
                 }

                 if (evCount >= maxevents) 
                 {
                     goto done;
                 }
             }
         }
     }
}
```

This code is part of a larger implementation of the epoll system call in a Unix-like operating system. The code is responsible for monitoring a set of file descriptors for events using the `FilesystemWatcher` class and returning the set of events that occur within a specified timeout period.

The code first initializes an empty vector of `EPollFD` structures called "files" and sets the `evCount` variable to zero, which will be used to keep track of the number of file descriptors with events. It then loops through each file descriptor in the epoll instance and retrieves its handle using the `proc->GetHandleAs` function. If the handle is invalid (i.e., the file descriptor is closed), the code skips to the next iteration of the loop.

If the handle is valid, the code calls the `getEvents` function with the handle and the events associated with the file descriptor. This function returns a bitmask of events that have occurred on the file descriptor since the last time it was checked. If any events have occurred, the code stores them in the `events` vector using the `StoreValue` function.

If the number of events stored in "events" is greater than zero, the code checks whether the `EPOLLONESHOT` flag is set for the file descriptor. If it is, the code adds the file descriptor to the `removeFds` vector, which will be used later to remove the file descriptor from the epoll instance. Otherwise, the code adds the file descriptor and its associated handle and epoll event structure to the "files" vector.

If the `evCount` variable is greater than or equal to the `maxevents` parameter passed to the epoll system call, the code jumps to the `done` label and exits the loop.

If the `evCount` variable is still zero and a timeout has been specified, the code creates a new instance of the `FilesystemWatcher` class and adds each file descriptor in the "files" vector to it using the `WatchNode` function. The code then enters a loop that waits for events to occur on any of the watched file descriptors.

If the timeout is greater than zero and the `WaitTimeout` function of the `FilesystemWatcher` instance returns true, the code returns `-EINTR`, indicating that the system call was interrupted. If the timeout is less than or equal to zero, the code returns zero, indicating that the timeout period has expired.

If the "Wait" function of the `FilesystemWatcher` instance returns true, the code returns `-EINTR`, indicating that the system call was interrupted.

If any events occur on a watched file descriptor, the code again calls the `getEvents` function to retrieve the bitmask of events, stores them in the "events" vector, and checks whether the `EPOLLONESHOT` flag is set for the file descriptor. If it is, the code adds the file descriptor to the `removeFds` vector. If the `evCount` variable is greater than or equal to the "maxevents" parameter, the code jumps to the "done" label and exits the loop.

After the loop exits, the code returns the number of events stored in the "events" vector. If any file descriptors were added to the "removeFds" vector, the code removes them from the epoll instance using the `RemoveFds` function.

> > `done`

This code removes the file descriptors specified in the `removeFds` list from the `epoll` object. It does this by iterating over each file descriptor in `removeFds`, and for each one, iterating over the list of file descriptors in the `epoll` object until it finds a matching file descriptor. Once a matching file descriptor is found, it is removed from the list and the loop for that file descriptor in `removeFds` continues. The function then returns the number of events that were processed.

[To the begining](#exit)

## <a name="9">ELF.cpp</a>

The code is responsible for loading ELF (Executable and Linkable Format) files into a process's address space. ELF is a standard file format used by many operating systems for executables, object code, shared libraries, and core dumps.

The code first verifies that the ELF header is valid by checking the first 4 bytes for the string "ELF". If it is not found, the loading process is aborted.

Next, the code extracts relevant information from the ELF header, such as the entry point and the number of program headers.

For each program header, the code checks if it is a loadable segment and if it has a non-zero memory size. If both conditions are true, the code maps the segment into the process's address space using a virtual memory object (VMO) and increases the process's memory usage accordingly.

The code then iterates through the program headers again and loads each loadable segment into memory using `memcpy()`. If the segment has an uninitialized data section, the code uses `memset()` to zero out the remaining memory.

If the ELF file has a `PT_INTERP` program header, the code extracts the path of the dynamic linker from the ELF file and stores it for later use.

Finally, the code returns an `elf_info_t` struct containing relevant information about the loaded ELF file, such as the entry point and the path of the dynamic linker.

[To the begining](#exit)
