#include <CPU.h>
#include <CString.h>
#include <IOPorts.h>
#include <Lock.h>
#include <Serial.h>

namespace Serial 
{
  enum Ports { COM1 = 0x3F8 };

  #define PORT COM1

  lock_t lock = 0;

  void Initialize() 
  {
    outportb(PORT + 1, 0x00); 
    outportb(PORT + 3, 0x80); 
    outportb(PORT + 0, 0x03); 
    outportb(PORT + 1, 0x00); 
    outportb(PORT + 3, 0x03); 
    outportb(PORT + 2, 0xC7); 
    outportb(PORT + 4, 0x0B); 
  }

  void Unlock() { releaseLock(&lock); }

  void Write(const char c) 
  {
    while (!(inportb(PORT + 5) & 0x20))
        ; // Make sure we are not transmitting data

    outportb(PORT, c);
  }

  void Write(const char* s) { Write(s, strlen(s)); }

  void Write(const char* s, unsigned n) 
  {
    if (CheckInterrupts()) 
    {
        acquireLockIntDisable(&lock); // Make the serial output readable

        while (n--) 
        {
            while (!(inportb(PORT + 5) & 0x20))
                ; // Make sure we are not transmitting data
            outportb(PORT, *s++);
        }

        releaseLock(&lock);
        asm("sti");
    } else while (n--) {
        while (!(inportb(PORT + 5) & 0x20))
            ; // Make sure we are not transmitting data
        outportb(PORT, *s++);
    }
  }
}
