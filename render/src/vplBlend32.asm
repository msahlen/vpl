section	.text

; Exported functions
global blendSrc
global blendSrcSolid

; C prototype;
; All blend functions look like:
; void blendSolid(vplUint32* dest,vplUint32 color,vplUint count)
; void blend(vplUint32* dest,vplUint32* src,vplUint count)
; count is how many pixels that should be blended, this always at least 8

blendSrcSolid:

      ; Get arguments
      enter 0,0
      push eax
      push ebx
      push ecx
      mov eax, [ebp + 8]  ; dst pointer in eax
      mov ebx, [ebp + 12] ; color in ebx
      mov ecx, [ebp + 16] ; number of pixels in ecx

      ; Load and shuffle data
      movaps xmm0,ebx      ; xmm0 = 0 0 0 color
      shufps xmm0,xmm0,0x0 ; xmm0 = color color color color

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

; Blend src, color is not solid
blendSrc:

      ; Get arguments
      enter 0,0
      push eax
      push ebx
      push ecx
      mov eax, [ebp + 8]  ; dst pointer in eax
      mov ebx, [ebp + 12] ; src pointer in ebx
      mov ecx, [ebp + 16] ; number of pixels in ecx

doFour:

      movaps xmm0,[ebx] ; xmm0 = color color color color

      sub ecx, 4         ; Decrease remaining pixels
      movaps [eax],xmm0  ; Write result

      ; Increase pointers
      add eax, 16
      add ebx, 16

      ; Loop ?
      cmp ecx,0x4
      jge doFour

      cmp ecx,0x1
      jle doOne

doTwo:
       movq xmm0,[ebx] ; xmm0 = 0 0 color color
       sub ecx, 2      ;Decrease remaining pixels
       movq [eax],xmm0 ; Write result
       add eax, 8      ; Increase pointer

doOne:
      ; are we done?
      cmp ecx,0x0
      je blendEnd

      ; write last pixel
      mov ecx,[ebx]
      mov [eax],ecx

blendEnd:

      pop eax
      pop ebx
      pop ecx
      leave
      ret

section	.data