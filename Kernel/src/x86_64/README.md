<a name="exit"></a>
# Code details

Content:
- [ACPI.cpp](#1)
- [APIC.cpp](#2)
- [CPUID.cpp](#3)
- [IDT.asm](#4)
- [IDT.cpp](#5)
- [PCI.cpp](#6)

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

