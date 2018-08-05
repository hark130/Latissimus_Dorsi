use64
; syscall parameter registers: rdi, rsi, rdx, r10, r8, r9

nop
nop
nop

_start:
	jmp		message_ptr
@back_again:
	mov 	rdi, 0x1  		; Move stdout fd into arg1
	pop		rsi				; Retrieve the return instruction pointer as arg2
	xor 	rdx, rdx		; Zeroize rdx (arg3)
	mov		dl, 40			; Move string length into arg3
	xor		rax, rax		; Zeroize rax
	mov 	eax, 0x4		; Move syscall number into rax
	; syscall				; Make the 'write' syscall
	; int3                  ; Software interrupt
	int     80h 			; Make the system call
	
message_ptr:
	; The processor pushes the value of the EIP register 
	; 	(which contains the offset of the instruction following
	;	the CALL instruction) onto the stack (for use later 
	;	as a return-instruction pointer)
	call @back_again
message: db "That weave gotta go. Pink slip. -Cardi B"
;message_len = $-message
;message_len = 40
