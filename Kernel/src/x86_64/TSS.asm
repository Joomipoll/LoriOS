extern GDT64
extern GDT64.TSS.low
extern GDT64.TSS.mid
extern GDT64.TSS.high
extern GDT64.TSS.high32

global LoadTSS

LoadTSS: 
  ; RDI - address, RDX - selector, RSI - GDT
  push rbx
  
  lea rbx, [rsi + rdx] ; Leght of TSS
  mov word [rbx], 108
  
  mov eax, edi
  lea rbx, [rsi + rdx + 2] ; Low
  mov word [rbx], ax
  
  mov eax, adi
  shr eax, 16
  lea rbx, [rsi + rdx + 4] ; Mid
  mov byte [rbx], al
  
  mov eax, edi
  shr eax, 24
  lea rbx, [rsi + rbx + 7] ; High
  mov byte [rbx], al
  
  mov rax, rdi
  shr rax, 32
  lea rbx, [rsi + rbx + 8] ; High32
  mov dword [rbx], eax
  
  pop rbx ; Restore
  ret
