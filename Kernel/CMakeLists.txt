project(lori-kernel)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(KERNEL_DEBUG_SYMBOLS OFF)

set(LORI_ARCH "x86_64")

add_compile_options($<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>)

if(NOT CMAKE_SYSTEM_NAME MATCHES Lori)
    message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} should be 'Lori'")
endif()

include_directories(include)
include_directories($ENV{LORI_BUILDROOT}/imp/frigg/include)
include_directories($ENV{LORI_BUILDROOT}/imp/cxxshim/stage2/include)

if("${LORI_ARCH}" STREQUAL "x86_64")
    include_directories(include/Arch/x86_64)

    enable_language(ASM_NASM)
    if(NOT CMAKE_ASM_NASM_COMPILER_LOADED)
        message(FATAL_ERROR "Failed to find NASM assembler!")
    endif()
    set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -i${CMAKE_CURRENT_SOURCE_DIR}/src/Arch/x86_64 \
        -felf64 -w+gnu-elf-extensions -o <OBJECT> <SOURCE>")
else()
    message(FATAL_ERROR "Unsupported architecture ${LORI_ARCH}")
endif()

add_compile_options(-Wall -Wextra -fno-exceptions -fno-rtti)
add_compile_options(-Wno-write-strings -Wno-unused-parameter -Wno-sign-compare -Wno-deprecated-volatile -Wno-non-c-typedef-for-linkage)
add_compile_options(-ffreestanding -nostdlib)
add_compile_options(-mno-mmx -mno-sse -mno-sse2)
add_compile_options(-mcmodel=large -mno-red-zone -fno-pic)

# Inline functions even with O0
add_compile_options(-O0)
add_compile_options(-fno-builtin -finline-functions)

# Clang crashes sometimes when generating debug information
if(KERNEL_DEBUG_SYMBOLS)
    add_compile_options(-g)
endif()

set(KERNEL_SRC
    src/Assert.cpp
    src/CharacterBuffer.cpp
    src/Device.cpp
    src/Debug.cpp
    src/Hash.cpp
    src/Kernel.cpp
    src/Lori.cpp
    src/Lock.cpp
    src/Log.cpp
    src/Math.cpp
    src/Panic.cpp
    src/Runtime.cpp
    src/SharedMemory.cpp
    src/Streams.cpp
    src/String.cpp

    src/Audio/Audio.cpp

    src/Video/Video.cpp
    src/Video/VideoConsole.cpp

    src/FileSystem/Fat32.cpp
    src/FileSystem/Filesystem.cpp
    src/FileSystem/FsNode.cpp
    src/FileSystem/FsVolume.cpp
    src/FileSystem/Pipe.cpp
    src/FileSystem/TAR.cpp
    src/FileSystem/Tmp.cpp
    src/FFileSystem/VolumeManager.cpp

    src/Memory/AddressSpace.cpp
    src/Memory/KMalloc.cpp
    src/Memory/VMObject.cpp

    src/Net/NetworkAdapter.cpp
    src/Net/Socket.cpp
    src/Net/Net.cpp
    src/Net/Interface.cpp
    src/Net/IPSocket.cpp
    src/Net/UDP.cpp
    src/Net/TCP.cpp

    src/Obj/Interface.cpp
    src/Obj/KObject.cpp
    src/Obj/Message.cpp
    src/Obj/Process.cpp
    src/Obj/Service.cpp

    src/Storage/AHCIController.cpp
    src/Storage/AHCIPort.cpp
    src/Storage/ATA.cpp
    src/Storage/ATADrive.cpp
    src/Storage/DiskDevice.cpp
    src/Storage/GPT.cpp
    src/Storage/NVMe.cpp
    src/Storage/NVMeNamespace.cpp
    src/Storage/PartitionDevice.cpp

    src/TTY/PTMX.cpp
    src/TTY/PTY.cpp

    src/USB/XHCI.cpp
)

set(KERNEL_SRC_x86_64
    src/Arch/x86_64/ACPI.cpp
    src/Arch/x86_64/APIC.cpp
    src/Arch/x86_64/CPUID.cpp
    src/Arch/x86_64/ELF.cpp
    src/Arch/x86_64/HAL.cpp
    src/Arch/x86_64/IDT.cpp
    src/Arch/x86_64/PS2.cpp
    src/Arch/x86_64/Modules.cpp
    src/Arch/x86_64/Paging.cpp
    src/Arch/x86_64/PCI.cpp
    src/Arch/x86_64/PhysicalAllocator.cpp
    src/Arch/x86_64/Scheduler.cpp
    src/Arch/x86_64/Serial.cpp
    src/Arch/x86_64/SMP.cpp
    src/Arch/x86_64/SSP.cpp
    src/Arch/x86_64/StringSafe.cpp
    src/Arch/x86_64/Symbols.cpp
    src/Arch/x86_64/Thread.cpp
    src/Arch/x86_64/Timer.cpp
    src/Arch/x86_64/TSS.cpp

    src/Arch/x86_64/Syscalls.cpp
    src/Arch/x86_64/Syscalls/Filesystem.cpp
    src/Arch/x86_64/Syscalls/EPoll.cpp

    src/Arch/x86_64/Entry.asm
    src/Arch/x86_64/IDT.asm
    src/Arch/x86_64/Memcpy.asm
    src/Arch/x86_64/SignalTrampoline.asm
    src/Arch/x86_64/SMPTrampoline.asm
    src/Arch/x86_64/Syscall.asm
    src/Arch/x86_64/TSS.asm
)

add_link_options(-z max-page-size=0x1000 -ffreestanding -nostdlib)

add_executable(kernel.sys ${KERNEL_SRC} ${KERNEL_SRC_x86_64})
target_link_options(kernel.sys PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linkscript-x86_64.ld)

add_subdirectory(Modules)

install(TARGETS kernel.sys
    RUNTIME DESTINATION lori)
