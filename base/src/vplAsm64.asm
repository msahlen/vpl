section	.text

; Exported functions
global transformSSE
global estimateFlatness

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

; function transformSSE
; transform an array of vectors
; C prototype;
; void transformSSE(float* transform,float* vectors,unsigned int numVectors)

; Matrix layout
; SX  transform[0]
; SHX transform[1]
; TX  transform[2]
; SHY transform[3]
; SY  transform[4]
; TY  transform[5]

;new_x = x * SX  + y_ * SHX + TX;
;new_y = x * SHY + y_ * SY  + TY;

transformSSE:

	  push	rbp
	  mov	rax,rsp		    ; rax = original rbp
	  sub	rsp, byte 4
	  and	rsp, byte (-16)	; align to 128 bits
	  mov	[rsp],rax
	  mov	rbp,rsp		    ; rbp = aligned rbp
	  lea	rsp, [rbp - 32] ; assign the frame pointer with a bias of 32

	  ; Arguments:
      ; matrix pointer in ARG1
      ; vector pointer in ARG2
      ; number of vectors in ARG3

      ; Load matrix and shuffle data
      movaps xmm1,[ARG1 + 16]  ; xmm1 = 0   0  TY  SY
      movaps xmm0,[ARG1]       ; xmm0 = SHY TX SHX SX
      movaps xmm2,xmm1         ; xmm2 = 0   0  TY  SY

      shufps xmm1,xmm0,0x0    ; xmm1 = SX  SX  SY  SY
      shufps xmm2,xmm0,0xA5   ; xmm2 = TX  TX  TY  TY
      shufps xmm1,xmm1,0x26   ; xmm1 = SY  SX  SY  SX
      shufps xmm2,xmm2,0x26   ; xmm2 = TY  TX  TY  TX
      shufps xmm0,xmm0,0xdd   ; xmm0 = SHY SHX SHY SHX

      ; If only one vector
      cmp ARG3,0x1
      jl doOne

      ;Load two vectors
doTwo:
      movaps xmm4, [ARG2]      ; xmm4 = y2 x2 y1 x1
      movaps xmm5, [ARG2]      ; xmm5 = y2 x2 y1 x1

      ;Reorder one vector
      shufps xmm5,xmm5,0xB1   ; xmm5 = x2 y2 x1 y1

      ; Perform multiplication
      mulps xmm4,xmm1         ; xmm4 = y2*SY  x2*SX  y1*SY  x1*SX
      mulps xmm5,xmm0         ; xmm5 = x2*SHY y2*SHX x1*SHY y1*SHX
      addps xmm4,xmm5         ; xmm4 = xmm4 + xmm5 ;)
      addps xmm4,xmm2         ; result for two vectors in xmm4
      movaps [ARG2],xmm4      ; Write result

      ;Increase address, decrease remaining vectors
      sub ARG3, 2
      add ARG2, 16

      ; Loop ?
      cmp ARG3,0x2
      jge doTwo

      cmp ARG3,0x1
      jl transformEnd

      ; Load one vector
doOne:
      movlps xmm4, [ARG2]      ; xmm4 = X X y1 x1
      movlps xmm5, [ARG2]      ; xmm5 = X X y1 x1

      ;Reorder one vector
      shufps xmm5,xmm5,0xB1   ; xmm4 = X X x1 y1

      ; Perform multiplication
      mulps xmm4,xmm1         ; xmm4 = X*SY  X*SX  y1*SY  x1*SX
      mulps xmm5,xmm0         ; xmm5 = X*SHY X*SHX x1*SHY y1*SHX
      addps xmm4,xmm5
      addps xmm4,xmm2         ; result for one vector in low part of xmm4
      movlps [ARG2],xmm4      ; Write result

transformEnd:

	  mov	rsp,rbp ; rsp <- aligned rbp
	  pop	rsp		; rsp <- original rbp
	  pop	rbp
	  ret


; function estimateFlatness
; C prototype;
; void estimateFlatness(float* bezier,float* deltas)
; Data stored like:
; bezier[0] = from.x_;
; bezier[1] = from.y_;
; bezier[2] = control1.x_;
; bezier[3] = control1.y_;
; bezier[4] = control2.x_;
; bezier[5] = control2.y_;
; bezier[6] = to.x_;
; bezier[7] = to.y_;
;
; Abbrevations used in comments
; p1 = from;
; p2 = control1;
; p3 = control2;
; p4 = to;

estimateFlatness:

	  push	rbp
	  mov	rax,rsp		    ; rax = original rbp
	  sub	rsp, byte 4
	  and	rsp, byte (-16)	; align to 128 bits
	  mov	[rsp],rax
	  mov	rbp,rsp		    ; rbp = aligned rbp
	  lea	rsp, [rbp - 32] ; assign the frame pointer with a bias of 32

	  ; Arguments:
      ; Pointer to bezier points in ARG1
      ; Pointer to result ARG2

      ; Load bezier and estimate flatness of curve
      movaps xmm0,[ARG1]       ; xmm0 = p2.y p2.x p1.y p1.x
      movaps xmm1,[ARG1 + 16]  ; xmm1 = p4.y p4.x p3.y p3.x
      movaps xmm2, xmm0       ; xmm2 = p2.y p2.x p1.y p1.x

      ; Reorder data
      shufps xmm2,xmm1,0xE4   ; xmm2 = p4.y p4.x p1.y p1.x
      shufps xmm0,xmm1,0x4E   ; xmm0 = p3.y p3.x p2.y p2.x

      ; Duplicate
      movaps xmm1,xmm2        ; xmm1 = p4.y p4.x p1.y p1.x
      movaps xmm3,xmm0        ; xmm3 = p3.y p3.x p2.y p2.x

      ; Multiply with constants
      addps xmm0,xmm0
      addps xmm0,xmm3         ; xmm0 = (p3.y p3.x p2.y p2.x)*3
      addps xmm1,xmm1         ; xmm1 = (p4.y p4.x p1.y p1.x)*2

      ; Reorder duplicate
      shufps xmm2,xmm2,0x4E   ; xmm2 = p1.y p1.x p4.y p4.x

      ; Perform subtractions
      subps xmm0,xmm1
      subps xmm0,xmm2

      ; Square result and store it
      mulps xmm0,xmm0         ; xmm0   = dy2 dx2 dy1 dx1
      movaps [ARG2],xmm0      ; result = dy2 dx2 dy1 dx1

	  mov	rsp,rbp ; rsp <- aligned rbp
	  pop	rsp		; rsp <- original rbp
	  pop	rbp
	  ret

section	.data