# LoriOS Kernel #
**Kernel modules** are located in [modules](https://github.com/Joomipoll/LoriBirdOS/tree/main/Kernel/Modules)

`modules.cfg` is copied to the initrd. At boot the kernel reads a list of filepaths from `modules.cfg` and loads the respective modules.

# Code details
Content:
- [linkscript-x86.ld](#1)
- [linkscript-x86_64.ld](#2)

## <a name="1"> linkscript-x86.ld</a>

This is a **linker script** for an ELF32-i386 kernel image. It defines the layout of the kernel's memory sections and their corresponding virtual addresses. 

``The KERNEL_VIRTUAL_BASE`` variable sets the virtual address at which the kernel will be loaded in memory. 

The script defines several sections for the multiboot header, data, and text, followed by the main kernel sections: .text, .data, and .bss. 

The ``.text`` section contains the kernel's executable code, and is aligned to a page boundary (4096 bytes). The .data section contains initialized data, and the ``.bss`` section contains uninitialized data. 

The script also discards certain sections, such as the exception handling frame and compiler comments. 

Finally, the script defines kernel_end as the end of the kernel image.
## <a name="2"> linkscript-x86_64.ld</a>

This is a **linker script** for a kernel image. It specifies the memory layout of the kernel and how the various sections of the kernel should be loaded into memory.

The entry point of the kernel is specified as "entry". The memory address of the kernel's virtual memory area (VMA) is set to 0xFFFFFFFF80000000. The ``.boot.text`` section contains the code that will be loaded and executed by the bootloader. The ".boot.data" section contains data that the bootloader needs to load into memory. Both of these sections are loaded at the address 0x100000.

The ``.stivale2hdr`` section contains the header information for the Stivale2 bootloader. This section is loaded at the address specified by **ADDR(.stivale2hdr) - KERNEL_VMA**.

The ``.text`` section contains the kernel code. The ``_code`` symbol is set to the current memory address. The ``.rodata`` section contains read-only data. The ``_rodata`` symbol is set to the current memory address. The ``.data`` section contains initialized data. The ``_data`` symbol is set to the current memory address. The ``.eh_frame`` section contains exception handling information. The ``_ehframe`` symbol is set to the current memory address. The ``.bss`` section contains uninitialized data. The ``_bss`` symbol is set to the current memory address.

The linker script also defines some other symbols that can be used by the kernel code, such as ``_ctors_start``, ``_ctors_end``, and ``_bss_end``. Finally, the linker script discards the ``.comment`` section, which contains debugging information.
