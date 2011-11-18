; Calling convention for Windows 64bit:
; The registers RCX, RDX, R8, R9 are used for integer and pointer arguments (in that order left to right),
; and XMM0, XMM1, XMM2, XMM3 are used for floating point arguments

; Calling conventions for Linux:
; The registers RDI, RSI, RDX, RCX, R8 and R9 are used for integer and pointer arguments
; while XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6 and XMM7 are used for floating point arguments.
; For system calls, R10 is used instead of RCX.[9] As in the Microsoft x64 calling convention,
; additional arguments are pushed onto the stack and the return value is stored in RAX.

%ifdef WIN64

%define ARG1 rcx
%define ARG2 rdx
%define ARG3 r8
%define ARG4 r9

%else

%define ARG1 rdi
%define ARG2 rsi
%define ARG3 rdx
%define ARG4 rcx

%endif

%imacro enterFunction 0
	push	rbp
	mov	rax,rsp		    ; rax = original rbp
	sub	rsp, byte 4
	and	rsp, byte (-16)	; align to 128 bits
	mov	[rsp],rax
	mov	rbp,rsp		    ; rbp = aligned rbp
	lea	rsp, [rbp - 32] ; assign the frame pointer with a bias of 32
%endmacro

%imacro leaveFunction 0
	mov	rsp,rbp ; rsp <- aligned rbp
	pop	rsp		; rsp <- original rbp
	pop	rbp
	ret
%endmacro

