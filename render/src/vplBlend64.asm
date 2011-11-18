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

      ; Get arguments
      enter 0,0
      push eax
      push ebx
      push ecx
      mov eax, [ebp + 8]  ; dst pointer in eax
      mov ebx, [ebp + 12] ; color pointer in ebx
      mov ecx, [ebp + 16] ; number of pixels in ecx

      ; Load data
      movaps xmm0,[ebx]  ; xmm0 = color color color color 

doFour:
      movaps [eax],xmm0  ; Write result

      ;Increase address, decrease remaining pixels
      sub ecx, 4
      add eax, 16

      ; Loop ?
      cmp ecx,0x4
      jge doFour

	  cmp ecx,0x1
      jle doOne

doTwo:
	  movq [eax],xmm0  ; Write result

      ;Increase address, decrease remaining pixels
      sub ecx, 2
      add eax, 8
     
doOne:
	  cmp ecx,0x0
	  je blendEnd
	  
	  mov ecx,[ebx]
	  mov [eax],ecx

blendEnd:

      pop eax
      pop ebx
      pop ecx
      leave
      ret

section	.data