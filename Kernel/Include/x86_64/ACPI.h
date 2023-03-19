#pragma once
#include <stdint.h>
#include <acpispec/tables.h>
#include <List.h>
typedef struct MADT { acpi_header_t header; uint32_t localAPICAddress; uint32_t flags; } __attribute__ ((packed)) acpi_madt_t;
typedef struct MADTEntry{ uint8_t type; uint8_t length; } __attribute__ ((packed)) acpi_madt_entry_t;
typedef struct LocalAPIC{ acpi_madt_entry_t entry; uint8_t processorID; uint8_t apicID; uint32_t flags; } __attribute__ ((packed)) apic_local_t;
typedef struct IOAPIC{ acpi_madt_entry_t entry; uint8_t apicID; uint8_t reserved; uint32_t address; uint32_t gSIB; } __attribute__ ((packed)) apic_io_t;
typedef struct ISO{ acpi_madt_entry_t entry; uint8_t busSource; uint8_t irqSource; uint32_t gSI; uint16_t flags; } __attribute__ ((packed)) apic_iso_t;
typedef struct NMI{ acpi_madt_entry_t entry; uint8_t processorID; uint16_t flags; uint8_t lINT; } __attribute__ ((packed)) apic_nmi_t;
typedef struct LocalAPICAddressOverride { acpi_madt_entry_t entry; uint16_t reserved; uint64_t address; } __attribute__ ((packed)) apic_local_address_override_t;
typedef struct PCIMCFGBaseAddress{ uint64_t baseAddress; uint16_t segmentGroupNumber; uint8_t startPCIBusNumber; uint8_t endPCIBusNumber; uint32_t reserved; } __attribute__ ((packed)) pci_mcfg_base_adress_t;
typedef struct PCIMCFG { acpi_header_t header; uint64_t reserved; PCIMCFGBaseAddress baseAddresses[]; } __attribute__ ((packed)) pci_mcfg_table_t;
namespace ACPI{ extern uint8_t processors[]; extern int processorCount; extern List<apic_iso_t*>* isos; extern acpi_xsdp_t* desc; extern acpi_rsdt_t* rsdtHeader; extern pci_mcfg_table_t* mcfg; void Init(); void SetRSDP(acpi_xsdp_t* p); void SetRSDP(acpi_xsdp_t* p); uint8_t RoutePCIPin(uint8_t bus, uint8_t slot, uint8_t func, uint8_t pin); void Reset(); }
