# Code details
Content:
- [ACPI.cpp](#1) <br>
- [APIC.cpp](#2) <br>
- [](#3) <br>

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















