.file "speculate.s"
.intel_syntax noprefix
.text

.globl speculate
.type speculate, @function
speculate:
  mfence
  call herring
  # Speculatively executed (because of herring's ret being mispredicted):
  rdtsc
  and eax, 7
  or eax,32
  shl eax, 12
  movzx eax, byte ptr [rdi+rax]
 

 
herring:
  # Lots of slow dependent instructions:
  xorps xmm0, xmm0
  sqrtpd xmm0, xmm0
  sqrtpd xmm0, xmm0
  sqrtpd xmm0, xmm0
  sqrtpd xmm0, xmm0
  sqrtpd xmm0, xmm0
  sqrtpd xmm0, xmm0
  sqrtpd xmm0, xmm0
  sqrtpd xmm0, xmm0
  # Using result of dependent instructions, adjust rsp to trick prediction of ret
  movd eax, xmm0
  lea rsp, [rsp+rax+8]
  ret # Actually returns from speculate, but predicted as returning from herring 

.globl timed_read
.type timed_read, @function
timed_read:
  rdtscp
  shl rdx, 32
  or rax, rdx
  mov rsi, rax
  mov al, [rdi]
  rdtscp
  shl rdx, 32
  or rax, rdx
  sub rax, rsi
  ret

.section .note.GNU-stack, "", @progbits
