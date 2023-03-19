#pragma once
#include <stdint.h>
#define LOCAL_APIC_ID 0x20
#define LOCAL_APIC_VERSION 0x30
#define LOCAL_APIC_TPR 0x80
#define LOCAL_APIC_APR 0x90
#define LOCAL_APIC_PPR 0xA0
#define LOCAL_APIC_EOI 0xB0
#define LOCAL_APIC_RRD 0xC0
#define LOCAL_APIC_LDR 0xD0
#define LOCAL_APIC_DFR 0xE0
#define LOCAL_APIC_SIVR 0xF0
#define LOCAL_APIC_ISR 0x100
#define LOCAL_APIC_TMR 0x180
#define LOCAL_APIC_IRR 0x200
#define LOCAL_APIC_ERROR_STATUS 0x280
#define LOCAL_APIC_ICR_LOW 0x300
#define LOCAL_APIC_ICR_HIGH 0x310
#define LOCAL_APIC_LVT_TIMER 0x320
#define LOCAL_APIC_LVT_THERMAL 0x330
#define LOCAL_APIC_LVT_PERF_MONITORING 0x340
#define LOCAL_APIC_LVT_LINT0 0x350
#define LOCAL_APIC_LVT_LINT1 0x360
#define LOCAL_APIC_LVT_ERROR 0x370
#define LOCAL_APIC_TIMER_INITIAL_COUNT 0x380
#define LOCAL_APIC_TIMER_CURRENT_COUNT 0x390
#define LOCAL_APIC_TIMER_DIVIDE 0x3E0
#define LOCAL_APIC_BASE 0xFFFFFFFFFF000
#define ICR_VECTOR(x) (x & 0xFF)
#define ICR_MESSAGE_TYPE_FIXED 0
#define ICR_MESSAGE_TYPE_LOW_PRIORITY (1 << 8)
#define ICR_MESSAGE_TYPE_SMI (2 << 8)
#define ICR_MESSAGE_TYPE_REMOTE_READ (3 << 8)
#define ICR_MESSAGE_TYPE_NMI (4 << 8)
#define ICR_MESSAGE_TYPE_INIT (5 << 8)
#define ICR_MESSAGE_TYPE_STARTUP (6 << 8)
#define ICR_MESSAGE_TYPE_EXTERNAL (7 << 8)
#define ICR_DSH_DEST 0 
#define ICR_DSH_SELF (1 << 18)
#define ICR_DSH_ALL (2 << 18)
#define ICR_DSH_OTHER (3 << 18)
#define IO_APIC_REGSEL 0x00
#define IO_APIC_WIN 0x10
#define IO_APIC_REGISTER_ID 0x0
#define IO_APIC_REGISTER_VER 0x1
#define IO_APIC_REGISTER_ARB 0x2
#define IO_APIC_RED_TABLE_START 0x10
#define IO_APIC_RED_TABLE_ENT(x) (0x10 + 2 * x)
#define IO_RED_TBL_VECTOR(x) (x & 0xFF)
namespace APIC { namespace Local { int Initialize(); void Enable(); void SendIPI(uint8_t apicID, uint32_t dsh, uint32_t type, uint8_t vector); } namespace IO { int Initialize(); void SetBase(uintptr_t newBase); void MapLegacyIRQ(uint8_t irq); } int Initialize(); }
extern "C" void LocalAPICEOI();
