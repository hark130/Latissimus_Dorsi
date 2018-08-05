use64
; syscall cdecl parameter: Arguments are pushed onto the call stack by the caller in right-to-left lexical order
; ssize_t write(int fd, const void *buf, size_t count);

nop
nop
nop

start:
	jmp		message_ptr
@back_again:
	xor		rax, rax		; Zeroize rax
	pop		rsi				; Retrieve the return instruction pointer as buf
	push    40				; Push string length onto the stack
	push    rsi 			; Push the string pointer onto the stack
	push 	1		  		; Push stdout fd onto the stack
	mov     rax, 4			; Assign syscall number to rax
	syscall					; Make the 'write' syscall
	add esp, 24				; Clean up the stack
;	int3                    ; Software interrupt
	
message_ptr:
	; The processor pushes the value of the EIP register 
	; 	(which contains the offset of the instruction following
	;	the CALL instruction) onto the stack (for use later 
	;	as a return-instruction pointer)
	call @back_again
message: db "That weave gotta go. Pink slip. -Cardi B"
;message_len = $-message
;message_len = 40
