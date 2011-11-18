%include "vplAsm.inc"

section	.text

; Exported functions
global blendSrc

; C prototype;
; void blendSrc(vplUint32* dest,vplUint32* color,vplUint count)
; dest is pointer to destination
; color is pointer to 4 32bit colors
; count is how many pixels that should be blended, this always at least 8
 
blendSrc:

      enterFunction

      ; Load data
      movaps xmm0,[ARG2]  ; xmm0 = color color color color 

doFour:
      movaps [ARG1],xmm0  ; Write result

      ;Increase address, decrease remaining pixels
      sub ARG3, 4
      add ARG1, 16

      ; Loop ?
      cmp ARG3,0x4
      jge doFour

	  cmp ARG3,0x1
      jle doOne

doTwo:
	  movq [ARG1],xmm0  ; Write result

      ;Increase address, decrease remaining pixels
      sub ARG3, 2
      add ARG1, 8
     
doOne:
	  cmp ARG3,0x0
	  je blendEnd
	  
	  mov ARG3,[ARG2]
	  mov [ARG1],ARG3

blendEnd:

      leaveFunction

section	.data
