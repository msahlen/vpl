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
