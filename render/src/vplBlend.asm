; VPL - Vector path library
; Copyright (C) 2009 - 2011 Mattias Sahlén <Mattias Sahlén <mattias.sahlen@gmail.com>
; This library is free software; you can redistribute it and/or
; modify it under the terms of the GNU General General Public
; License as published by the Free Software Foundation; either
; version 2.1 of the License, or (at your option) any later version.

; This library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; General General Public License for more details.

; You should have received a copy of the GNU General General Public
; License along with this library;
; if not, see <http://www.gnu.org/licenses/>.

%include "vplAsm.inc"

section .data

MASK: db 0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff
HALF: db 0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80
NEG : db 0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0xff

section .text

global blendSrcOverDestSSE2

;-----------------------------------------------------------------
; Function multiplyPixelsSSE2
;
; C prototype;
; void multiplyPixelsSSE2(vplUint32* dest,vplUint32* color,vplUint* count)
;
; Data must be aligned before calling this.

; C code for this function
; vplUint t = (pixel & 0xff00ff) * oneMinusAlpha;
;			t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
;			t &= 0xff00ff;
;			pixel = ((pixel >> 8) & 0xff00ff) * oneMinusAlpha;
;			pixel = (pixel + ((pixel >> 8) & 0xff00ff) + 0x800080);
;			pixel &= 0xff00ff00;
;			pixel |= t;
;
;           return pixel;

blendSrcOverDestSSE2:

      enterFunction

	  cmp ARG3,4
      jl blendSrcOverDest2

	  movaps xmm0,[MASK] ; Mask in xmm0
	  movaps xmm1,[HALF] ; Constant in xmm1
	  movaps xmm7,[NEG]  ; Negation mask in xmm7

blendSrcOverDest4:

	  ; Load data
	  movaps xmm2,[ARG1] ; 4 pixels in xmm2
	  movaps xmm3,xmm2   ; duplicate pixels
	  movaps xmm4,xmm2   ; duplicate pixels
	  movaps xmm5,xmm2   ; duplicate pixels
	  psrld xmm2, 28     ; Alpha in xmm2
	  xorps xmm2,xmm7    ; 1 - alpha in xmm2

	  ; Compute t
	  andps xmm3,xmm0    ; pixel & 0xff00ff
	  mulps xmm3,xmm2    ; t = (pixel & 0xff00ff) * alpha;
	  movaps xmm4,xmm3   ; duplicate t
	  psrld xmm3,8		 ; t >> 8
	  andps xmm3,xmm0    ; (t >> 8) & 0xff00ff
	  addps xmm4,xmm3    ; t + ((t >> 8) & 0xff00ff)
	  addps xmm4,xmm1    ; (t + ((t >> 8) & 0xff00ff) + 0x800080)
	  psrld xmm4,8	     ; t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
	  andps xmm4,xmm0    ; t &= 0xff00ff;

	  ; Compute pixel
	  psrld xmm5,8       ; (pixel >> 8)
	  andps xmm5,xmm0    ; ((pixel >> 8) & 0xff00ff)
	  mulps xmm5,xmm2    ; ((pixel >> 8) & 0xff00ff) * alpha
	  movaps xmm6,xmm5   ; duplicate pixel
	  psrld xmm5,8       ; (pixel >> 8)
	  andps xmm5,xmm0    ; ((pixel >> 8) & 0xff00ff)
	  addps xmm6,xmm5    ; (pixel + ((pixel >> 8) & 0xff00ff)
	  addps xmm6,xmm5    ; (pixel + ((pixel >> 8) & 0xff00ff) + 0x800080)
	  andps xmm6,xmm0    ; pixel &= 0xff00ff;
	  orps xmm6,xmm4     ; pixel |= t;

	  ; Store data
	  movaps [ARG2],xmm6

	  ; Increase offset
	  add ARG1, 16
	  add ARG2, 16
	  sub ARG3, 4

	  cmp ARG3,4
      jge blendSrcOverDest4

blendSrcOverDest2:

	  cmp ARG3,2
      jl blendSrcOverDestEnd

	  ; Load data
	  movq xmm2,[ARG1] ; 2 pixels in xmm2
	  movq xmm3,xmm2   ; duplicate pixels
	  movq xmm4,xmm2   ; duplicate pixels
	  movq xmm5,xmm2   ; duplicate pixels
	  psrld xmm2, 28   ; Alpha in xmm2
	  xorps xmm2,xmm7  ; 1 - alpha in xmm2

	  ; Compute t
	  andps xmm3,xmm0  ; pixel & 0xff00ff
	  mulps xmm3,xmm2  ; t = (pixel & 0xff00ff) * alpha;
	  movq xmm4,xmm3   ; duplicate t
	  psrld xmm3,8	   ; t >> 8
	  andps xmm3,xmm0  ; (t >> 8) & 0xff00ff
	  addps xmm4,xmm3  ; t + ((t >> 8) & 0xff00ff)
	  addps xmm4,xmm1  ; (t + ((t >> 8) & 0xff00ff) + 0x800080)
	  psrld xmm4,8	   ; t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
	  andps xmm4,xmm0  ; t &= 0xff00ff;

	  ; Compute pixel
	  psrld xmm5,8     ; (pixel >> 8)
	  andps xmm5,xmm0  ; ((pixel >> 8) & 0xff00ff)
	  mulps xmm5,xmm2  ; ((pixel >> 8) & 0xff00ff) * alpha
	  movq xmm6,xmm5   ; duplicate pixel
	  psrld xmm5,8     ; (pixel >> 8)
	  andps xmm5,xmm0  ; ((pixel >> 8) & 0xff00ff)
	  addps xmm6,xmm5  ; (pixel + ((pixel >> 8) & 0xff00ff)
	  addps xmm6,xmm5  ; (pixel + ((pixel >> 8) & 0xff00ff) + 0x800080)
	  andps xmm6,xmm0  ; pixel &= 0xff00ff;
	  orps xmm6,xmm4   ; pixel |= t;

	  ; Store data
	  movq [ARG2],xmm6

	  ; decrease remaining pixels
	  sub ARG3, 2

blendSrcOverDestEnd:

      leaveFunction

