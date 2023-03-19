#pragma once

#include <stdint.h>

typedef struct ELF64Header { unsigned char id[16]; uint16_t type; uint16_t machine; uint32_t version; uint64_t entry; uint64_t phOff; uint64_t shOff; uint32_t flags; uint16_t hdrSize; uint16_t phEntrySize; uint16_t phNum; uint16_t shEntrySize; uint16_t shNum; uint16_t shStrIndex; } __attribute__((packed)) elf64_header_t;
typedef struct ELF64ProgramHeader { uint32_t type; uint32_t flags; uint64_t offset; uint64_t vaddr; uint64_t paddr; uint64_t fileSize; uint64_t memSize; uint64_t align; } __attribute__((packed)) elf64_program_header_t;
typedef struct ELF64DynamicEntry { int64_t tag; union { uint64_t val; uint64_t ptr; }; } __attribute__((packed)) elf64_dynamic_t;
typedef struct ELF64Symbol { uint32_t name; uint8_t info; uint8_t other; uint16_t shIndex; uint64_t value; uint64_t size; } __attribute__((packed)) elf64_symbol_t;
typedef struct ELF64Section { uint32_t name; uint32_t type; uint64_t flags; uint64_t addr; uint64_t off; uint64_t size; uint32_t link; uint32_t info; uint64_t align; uint64_t entSize; } __attribute__((packed)) elf64_section_t;
typedef struct ELF64Relocation { uint64_t offset; uint64_t info; } __attribute__((packed)) elf64_rel_t;
typedef struct ELF64RelocationA { uint64_t offset; uint64_t info; int64_t addend; } __attribute__((packed)) elf64_rela_t;
typedef struct ELFInfo { uint64_t entry; uint64_t pHdrSegment; uint64_t phEntrySize; uint64_t phNum; char* linkerPath; } elf_info_t;
#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xffffffffL))
#define DT_NULL 0
#define DT_NEEDED 1
#define DT_PTRELSZ 2
#define DT_PLTGOT 3
#define DT_HASH 4
#define DT_STRTAB 5
#define DT_SYMTAB 6
#define DT_RELA 7
#define DT_RELASZ 8
#define DT_RELAENT 9
#define DT_STRSZ 10
#define DT_SYMENT 11
#define DT_INIT 12
#define DT_FINI 13
#define DT_SONAME 14
#define DT_RPATH 15
#define DT_SYMBOLIC 16
#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19
#define DT_PLTREL 20
#define DT_DEBUG 21
#define DT_TEXTREL 22
#define DT_JMPREL 23
#define DT_BIND_NOW 24
#define DT_INIT_ARRAY 25
#define DT_FINI_ARRAY 26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_DYNSYM 11
#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define ELF64_SYM_TYPE(s) ((s) & 0xf)
#define ELF64_SYM_BIND(s) (((s) >> 4) & 0xf)
#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL)
#define ELF64_R_X86_64_NONE 0
#define ELF64_R_X86_64_64 1
#define ELF64_R_X86_64_32 10
#define ELF64_R_X86_64_32S 11
using ELFHeader = ELF64Header;
using ELFProgramHeader = ELF64ProgramHeader;
using ELFDynamicEntry = ELF64DynamicEntry;
using ELFSymbol = ELF64Symbol;
using ELFSection = ELF64Section;
using ELFRelocation = ELF64Relocation;
using ELFRelocationA = ELF64RelocationA;
class Process;
int VerifyELF(void* elf);
elf_info_t LoadELFSegments(Process* proc, void* elf, uintptr_t base);
