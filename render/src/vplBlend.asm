; VPL - Vector path library
; Copyright (C) 2009 - 2011 Mattias Sahlén <Mattias Sahlén <mattias.sahlen@gmail.com>>
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

section .text

global multiplyPixelsSSE2

;-----------------------------------------------------------------
; Function multiplyPixelsSSE2
;
; C prototype;
; void multiplyPixelsSSE2(vplUint32* dest,vplUint32* color,vplUint* count)
;
; Data must be aligned before calling this.

; C code for this function
; vplUint t = (pixel & 0xff00ff) * alpha;
;			t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
;			t &= 0xff00ff;
;			pixel = ((pixel >> 8) & 0xff00ff) * alpha;
;			pixel = (pixel + ((pixel >> 8) & 0xff00ff) + 0x800080);
;			pixel &= 0xff00ff00;
;			pixel |= t;

multiplyPixelsSSE2:

      enterFunction

      movaps xmm0,[MASK] ; Mask in xmm0
      movaps xmm1,[HALF] ; Constant in xmm1
      movaps xmm2,[ARG1] ; 4 pixels in xmm2

multiplyPixelsEnd:

      leaveFunction

section	.text

