#include <APIC.h>
#include <CString.h>
#include <HAL.h>
#include <IDT.h>
#include <IOPorts.h>
#include <Lock.h>
#include <Logging.h>
#include <Panic.h>
#include <Scheduler.h>
#include <StackTrace.h>
#include <Syscalls.h>

idt_entry_t idt[256];

idt_ptr_t idtPtr;

struct ISRDataPair {
    isr_t handler;
    void* data;
};

ISRDataPair interruptHandlers[256];

extern "C" 
{
  void isr0();
  void isr1();
  void isr2();
  void isr3();
  void isr4();
  void isr5();
  void isr6();
  void isr7();
  void isr8();
  void isr9();
  void isr10();
  void isr11();
  void isr12();
  void isr13();
  void isr14();
  void isr15();
  void isr16();
  void isr17();
  void isr18();
  void isr19();
  void isr20();
  void isr21();
  void isr22();
  void isr23();
  void isr24();
  void isr25();
  void isr26();
  void isr27();
  void isr28();
  void isr29();
  void isr30();
  void isr31();
  
  void irq0();
  void irq1();
  void irq2();
  void irq3();
  void irq4();
  void irq5();
  void irq6();
  void irq7();
  void irq8();
  void irq9();
  void irq10();
  void irq11();
  void irq12();
  void irq13();
  void irq14();
  void irq15();
  
  void isr0x69();
}

extern uint64_t int_vectors[];

namespace IDT
{
  void IPIHalt(void*, RegisterContext* r) { asm("cli"); asm("hlt"); }
  void InvalidInterruptHandler(void*, RegisterContext* r) { Log::Warning("Invalid interrupt handler called!"); }
  static void SetGate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags, uint8_t ist = 0) 
  {
    idt[num].base_high = (base >> 32);
    idt[num].base_med = (base >> 16) & 0xFFFF;
    idt[num].base_low = base & 0xFFFF;

    idt[num].sel = sel;
    idt[num].null = 0;
    idt[num].ist = ist & 0x7;

    idt[num].flags = flags;
  }
  //TODO: Create Initialize() 
}
