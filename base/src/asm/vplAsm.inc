;VPL - Vector path library
;Copyright (C) 2009 - 2011 Mattias Sahlén <Mattias Sahlén <mattias.sahlen@gmail.com>>
;This library is free software; you can redistribute it and/or
;modify it under the terms of the GNU General General Public
;License as published by the Free Software Foundation; either
;version 2.1 of the License, or (at your option) any later version.

;This library is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;General General Public License for more details.

;You should have received a copy of the GNU General General Public
;License along with this library;
;if not, see <http://www.gnu.org/licenses/>.

; -----------------------------------------------------------------------------
; Note on calling conventions:
;
; Calling convention for Windows 64bit:
; The registers RCX, RDX, R8, R9 are used for integer and pointer arguments
; (in that order left to right),and XMM0, XMM1, XMM2, XMM3 are used for
; floating point arguments

; Calling conventions for Linux:
; The registers RDI, RSI, RDX, RCX, R8 and R9 are used for integer and
; pointer arguments while XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6 and XMM7 are
; used for floating point arguments.
; For system calls, R10 is used instead of RCX.
; As in the Microsoft x64 calling convention, additional arguments are pushed
; onto the stack and the return value is stored in RAX.

; These macros will hopefully make it possible to only write one version for
; both 32 and 64 bit code

%ifdef SSE64

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

%define TMP1 r11
%define TMP2 r12
%define TMP3 r13

%define TMP1_32 r11d
%define TMP2_32 r12d
%define TMP3_32 r13d

%define TMP1_16 r11w
%define TMP2_16 r12w
%define TMP3_16 r13w

%define TMP1_8 r11b
%define TMP2_8 r12b
%define TMP3_8 r13b

%ifdef WIN64

%define ARG1 rcx
%define ARG2 rdx
%define ARG3 r8
%define ARG4 r9

%define ARG1_32 ecx
%define ARG2_32 edx
%define ARG3_32 r8d
%define ARG4_32 r9d

%else

%define ARG1 rdi
%define ARG2 rsi
%define ARG3 rdx
%define ARG4 rcx

%define ARG1_32 edi
%define ARG2_32 esi
%define ARG3_32 edx
%define ARG4_32 ecx

%endif

%else

%define ARG1 ebx
%define ARG2 esi
%define ARG3 edi

%define ARG1_32 ebx
%define ARG2_32 esi
%define ARG3_32 edi

%define TMP1 eax
%define TMP2 ecx
%define TMP3 edx

%define TMP1_32 eax
%define TMP2_32 ecx
%define TMP3_32 edx

%define TMP1_16 ax
%define TMP2_16 cx
%define TMP3_16 dx

%define TMP1_8 al
%define TMP2_8 cl
%define TMP3_8 dl

%imacro enterFunction 0
      enter 0,0
      push ebx
      push esi
      push edi
      mov ebx, [ebp + 8]
      mov esi, [ebp + 12]
      mov edi, [ebp + 16]
%endmacro

%imacro leaveFunction 0
	  pop edi
      pop esi
      pop ebx
      leave
      ret
%endmacro

%endif

