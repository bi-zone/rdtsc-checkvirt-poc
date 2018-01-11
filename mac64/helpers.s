.file "speculate.s"
.intel_syntax noprefix
.text

.globl _speculate
_speculate:
  mfence
  call _herring
  # Speculatively executed (because of herring's ret being mispredicted):
  rdtsc
  and eax, 7
  or eax,32
  shl eax, 12
  movzx eax, byte ptr [rdi+rax]
 

 
_herring:

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

.globl _timed_read
_timed_read:
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


