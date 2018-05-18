use64
; syscall parameter registers: rdi, rsi, rdx, r10, r8, r9

;SYS_WRITE	= 0x01

nop
nop
nop

start:
	jmp		message_ptr
@back_again:
	xor		rax, rax		; Zeroize rax
;	mov 	al, SYS_WRITE	; Move syscall number into rax
	mov 	al, 1			; Move syscall number into rax
	mov 	rdi, 1  		; Move stdout fd into arg1
	pop		rsi				; Retrieve the return instruction pointer as arg2
	xor 	rdx, rdx		; Zeroize rdx (arg3)
;	mov		dl, message_len	; Move string length into arg3
	mov		dl, 41			; Move string length into arg3
	syscall					; Make the 'write' syscall
	int3                    ; Software interrupt
	
message_ptr:
	; The processor pushes the value of the EIP register 
	; 	(which contains the offset of the instruction following
	;	the CALL instruction) onto the stack (for use later 
	;	as a return-instruction pointer)
	call @back_again
message: db "That weave gotta go. Pink slip. -Cardi B", 0xA
;message_len = $-message
;message_len = 41
