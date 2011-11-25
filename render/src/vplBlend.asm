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

%include "vplAsm.inc"

section	.text

; Exported functions
global blendSrc
global blendSrcSolid

; C prototype;
; All blend functions look like:
; void blendSolid(vplUint32* dest,vplUint32* color,vplUint count)
; void blend(vplUint32* dest,vplUint32* src,vplUint count)
; count is how many pixels that should be blended, this always at least 8

; Src blend, solid color
blendSrcSolid:

      enterFunction

      ; Load data and shuffle data
      movd xmm0,[ARG2]     ; xmm0 = 0 0 0 color
      shufps xmm0,xmm0,0x0 ; xmm0 = color color color color

doFourSolid:
      movaps [ARG1],xmm0  ; Write result

      ;Increase address, decrease remaining pixels
      sub ARG3, 4
      add ARG1, 16

      ; Loop ?
      cmp ARG3,0x4
      jge doFourSolid

	  cmp ARG3,0x1
      jle doOneSolid

doTwoSolid:
	  movq [ARG1],xmm0  ; Write result

      ;Increase address, decrease remaining pixels
      sub ARG3, 2
      add ARG1, 8

doOneSolid:
	  cmp ARG3,0x0
	  je blendEndSolid

	  mov ARG3,[ARG2]
	  mov [ARG1],ARG3

blendEndSolid:

      leaveFunction


; Blend src, color is not solid
blendSrc:

      enterFunction

doFour:

      movaps xmm0,[ARG2] ; xmm0 = color color color color

      sub ARG3, 4         ; Decrease remaining pixels
      movaps [ARG1],xmm0  ; Write result

      ; Increase pointers
      add ARG1, 16
      add ARG2, 16

      ; Loop ?
      cmp ARG3,0x4
      jge doFour

      cmp ARG3,0x1
      jle doOne

doTwo:
       movq xmm0,[ARG2] ; xmm0 = 0 0 color color
       sub ARG3, 2      ; Decrease remaining pixels
       movq [ARG1],xmm0 ; Write result
       add ARG1, 8      ; Increase pointer

doOne:
      ; are we done?
      cmp ARG3,0x0
      je blendEnd

      ; write last pixel
      mov ARG3,[ARG2]
      mov [ARG1],ARG3

blendEnd:

      leaveFunction

section	.data

; -----------------------------------------------------------
; Function memFillSSE2
;
; SSE2 version of memFill
; -----------------------------------------------------------

;align16
;memFillSSE2:
		
;		enterFunction
		
;		movd xmm0, 	 
;		cmp ARG3 32
;		jl lessThan32

;moreThan32:
		
		; Move 32 bytes
;	    movq xmm0, [ARG2]
;       movq xmm1, [ARG2 + 8]
;        movq xmm2, [ARG2 + 16]
;        movq xmm3, [ARG2 + 24]
;        movq qword [ARG1],xmm0
;        movq qword [ARG1 + 8],xmm1
;        movq qword [ARG1 + 16],xmm2
;        movq qword [ARG1 + 24],xmm3
		
		; Update pointers
;		add ARG1 32
;		add ARG2 32
;		sub ARG3 32

;lessThan32:

;		cmp ARG3 16
;		jl lessThan16
 
		; Move 16 bytes
;		movq xmm0, [ARG2]
;       movq xmm1, [ARG2 + 8]
;      movq qword [ARG1],xmm0
;        movq qword [ARG1 + 8],xmm1
		
		; Update pointers
;		add ARG1 16
;		add ARG2 16
;		sub ARG3 16

;lessThan16:

;		cmp ARG3 8
;		jl lessThan8

		 ; move 8 bytes
 ;       movq xmm0, [ARG2]
 ;       movq qword [ARG1], xmm0

		; Update pointers
;		add ARG1 8
;		add ARG2 8
;		sub ARG3 8

;lessThan8:

;		cmp ARG3 4
;		jl lessThan4
		
		; Move 4 bytes
;		mov TMP1, [ARG2]
;        mov [ARG1], TMP1_32

		; Update pointers
;		add ARG1 4
;		add ARG2 4
;		sub ARG3 4

lessThan4:

;		cmp ARG3 2
;		jl lessThan2

		; Move 2 bytes
;        movzx   TMP1, word [ARG2]
;        mov     [ARG1], TMP1_16

lessThan2:
		
;		cmp ARG3 1
;		jl memFillEnd

		; Move 1 byte
;        movzx   TMP1, byte [ARG2]
;        mov     [ARG1], TMP1_8

;memFillEnd:

;		leaveFunction