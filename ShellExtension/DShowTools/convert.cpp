/*
 *  Part of the Matroska Shell Extension
 *
 *  convert.cpp
 *
 *  Copyright (C) Jory Stone - March 2004
 *
 *  The Matroska Shell Extension is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  The Matroska Shell Extension is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Matroska Shell Extension; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*!
    \file convert.cpp
		\version \$Id: convert.cpp,v 1.2 2004/04/28 04:21:55 jcsston Exp $
    \brief YUV Image Conversion Code
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#include "convert.h"

/*************************************
 * Progressive YV12 -> YUY2 conversion
 *
 * (c) 2003, Klaus Post.
 *
 * Requires mod 8 rowsize.
 * MMX version.
 *************************************/

void mmx_yv12_to_yuy2(const BYTE* srcY, const BYTE* srcU, const BYTE* srcV, int src_rowsize, int src_pitch, int src_pitch_uv, 
                    BYTE* dst, int dst_pitch,
                    int height) {
  static __int64 add_64=0x0002000200020002;
  const BYTE** srcp= new const BYTE*[3];
  int src_pitch_uv2 = src_pitch_uv*2;
  int skipnext = 0;

  int dst_pitch2=dst_pitch*2;
  int src_pitch2 = src_pitch*2;


  
  /**** Do first and last lines - NO interpolation:   *****/
  // MMX loop relies on C-code to adjust the lines for it.
  const BYTE* _srcY=srcY;
  const BYTE* _srcU=srcU;
  const BYTE* _srcV=srcV;
  BYTE* _dst=dst;

  for (int i=0;i<4;i++) {
    switch (i) {
    case 1:
      _srcY+=src_pitch;  // Same chroma as in 0
      _dst+=dst_pitch;
      break;
    case 2:
      _srcY=srcY+(src_pitch*(height-2));
      _srcU=srcU+(src_pitch_uv*((height>>1)-1));
      _srcV=srcV+(src_pitch_uv*((height>>1)-1));
      _dst = dst+(dst_pitch*(height-2));
      break;
    case 3: // Same chroma as in 4
      _srcY += src_pitch;
      _dst += dst_pitch;
      break;
    default:  // Nothing, case 0
        break;
    }


    __asm {
    mov edi, [_dst]
    mov eax, [_srcY]
    mov ebx, [_srcU]
    mov ecx, [_srcV]
    mov edx,0
    pxor mm7,mm7
    jmp xloop_test_p
xloop_p:
    movq mm0,[eax]    //Y
      movd mm1,[ebx]  //U
    movq mm3,mm0  
     movd mm2,[ecx]   //V
    punpcklbw mm0,mm7  // Y low
     punpckhbw mm3,mm7   // Y high
    punpcklbw mm1,mm7   // 00uu 00uu
     punpcklbw mm2,mm7   // 00vv 00vv
    movq mm4,mm1
     movq mm5,mm2
    punpcklbw mm1,mm7   // 0000 00uu low
     punpcklbw mm2,mm7   // 0000 00vv low
    punpckhbw mm4,mm7   // 0000 00uu high
     punpckhbw mm5,mm7   // 0000 00vv high
    pslld mm1,8
     pslld mm4,8
    pslld mm2,24
     pslld mm5,24
    por mm0, mm1
     por mm3, mm4
    por mm0, mm2
     por mm3, mm5
    movq [edi],mm0
     movq [edi+8],mm3
    add eax,8
    add ebx,4
    add ecx,4
    add edx,8
    add edi, 16
xloop_test_p:
      cmp edx,[src_rowsize]
      jl xloop_p
    }
  }

/****************************************
 * Conversion main loop.
 * The code properly interpolates UV from
 * interlaced material.
 * We process two lines in the same field
 * in the same loop, to avoid reloading
 * chroma each time.
 *****************************************/

  height-=4;

  dst+=dst_pitch2;
  srcY+=src_pitch2;
  srcU+=src_pitch_uv;
  srcV+=src_pitch_uv;

  srcp[0] = srcY;
  srcp[1] = srcU-src_pitch_uv;
  srcp[2] = srcV-src_pitch_uv;

  int y=0;
  int x=0;

  __asm {
    mov esi, [srcp]
    mov edi, [dst]

    mov eax,[esi]
    mov ebx,[esi+4]
    mov ecx,[esi+8]
    mov edx,0
    jmp yloop_test
    align 16
yloop:
    mov edx,0               // x counter
    jmp xloop_test
    align 16
xloop:
    mov edx, src_pitch_uv
    movq mm0,[eax]          // mm0 = Y current line
     pxor mm7,mm7
    movd mm2,[ebx+edx]            // mm2 = U top field
     movd mm3, [ecx+edx]          // mm3 = V top field
    movd mm4,[ebx]        // U prev top field
     movq mm1,mm0             // mm1 = Y current line
    movd mm5,[ecx]        // V prev top field

    punpcklbw mm2,mm7        // U 00uu 00uu 00uu 00uu
     punpcklbw mm3,mm7         // V 00vv 00vv 00vv 00vv
    punpcklbw mm4,mm7        // U 00uu 00uu 00uu 00uu
     punpcklbw mm5,mm7         // V 00vv 00vv 00vv 00vv
    paddusw mm4,mm2
     paddusw mm5,mm3
    paddusw mm4,mm2
     paddusw mm5,mm3
    paddusw mm4,mm2
     paddusw mm5,mm3
    paddusw mm4, [add_64]
     paddusw mm5, [add_64]
    psrlw mm4,2
     psrlw mm5,2


     punpcklbw mm0,mm7        // Y low
    punpckhbw mm1,mm7         // Y high*
     pxor mm6,mm6
    punpcklbw mm6,mm4         // U 0000 uu00 0000 uu00 (low)
     punpckhbw mm7,mm4         // V 0000 uu00 0000 uu00 (high
    por mm0,mm6
     por mm1,mm7
    movq mm6,mm5
     punpcklbw mm5,mm5          // V 0000 vvvv 0000 vvvv (low)
    punpckhbw mm6,mm6           // V 0000 vvvv 0000 vvvv (high)
     pslld mm5,24
    pslld mm6,24
     por mm0,mm5
    por mm1,mm6
    mov edx, src_pitch_uv2
     movq [edi],mm0
    movq [edi+8],mm1

    //Next line 
     
    movd mm4,[ebx+edx]        // U next top field
     movd mm5,[ecx+edx]       // V prev top field
    mov edx, [src_pitch]
     pxor mm7,mm7
    movq mm0,[eax+edx]        // Next U-line
    movq mm1,mm0             // mm1 = Y current line

    punpcklbw mm4,mm7        // U 00uu 00uu 00uu 00uu
     punpcklbw mm5,mm7         // V 00vv 00vv 00vv 00vv
    paddusw mm4,mm2
     paddusw mm5,mm3
    paddusw mm4,mm2
     paddusw mm5,mm3
    paddusw mm4,mm2
     paddusw mm5,mm3
    paddusw mm4, [add_64]
     paddusw mm5, [add_64]
    psrlw mm4,2
     psrlw mm5,2

     punpcklbw mm0,mm7        // Y low
    punpckhbw mm1,mm7         // Y high*
     pxor mm6,mm6
    punpcklbw mm6,mm4         // U 0000 uu00 0000 uu00 (low)
     punpckhbw mm7,mm4         // V 0000 uu00 0000 uu00 (high
    por mm0,mm6
     por mm1,mm7
    movq mm6,mm5
     punpcklbw mm5,mm5          // V 0000 vvvv 0000 vvvv (low)
    punpckhbw mm6,mm6           // V 0000 vvvv 0000 vvvv (high)
     pslld mm5,24
    mov edx,[dst_pitch]
    pslld mm6,24
     por mm0,mm5
    por mm1,mm6
     movq [edi+edx],mm0
    movq [edi+edx+8],mm1
     add edi,16
    mov edx, [x]
     add eax, 8
    add ebx, 4
     add edx, 8
    add ecx, 4
xloop_test:
    cmp edx,[src_rowsize]
    mov x,edx
    jl xloop
    mov edi, dst
    mov eax,[esi]
    mov ebx,[esi+4]
    mov ecx,[esi+8]

    add edi,[dst_pitch2]
    add eax,[src_pitch2]
    add ebx,[src_pitch_uv]
    add ecx,[src_pitch_uv]
    mov edx, [y]
    mov [esi],eax
    mov [esi+4],ebx
    mov [esi+8],ecx
    mov [dst],edi
    add edx, 2

yloop_test:
    cmp edx,[height]
    mov [y],edx
    jl yloop
    emms
  }
   delete[] srcp;
}

__declspec(align(8)) static __int64 Y_SUB = 0x0010001000100010;
__declspec(align(8)) static __int64 U_SUB = 0x0080008000800080;
__declspec(align(8)) static __int64 Y_MUL = 0x004a004a004a004a;
__declspec(align(8)) static __int64 UG_MUL = 0x0019001900190019;
__declspec(align(8)) static __int64 VG_MUL = 0x0034003400340034;
__declspec(align(8)) static __int64 UB_MUL = 0x0081008100810081;
__declspec(align(8)) static __int64 VR_MUL = 0x0066006600660066;



void __declspec(naked) yv12_to_rgb32_mmx(BYTE *dst, 
                         int dst_stride, 
                         const BYTE *y_src,
                         const BYTE *u_src,
                         const BYTE *v_src, 
                         int y_stride, int uv_stride,
                         int width, int height) {

#define localsize 72
#define TEMP_Y1  esp
#define TEMP_Y2  esp + 8
#define TEMP_G1  esp + 16
#define TEMP_G2  esp + 24
#define TEMP_B1  esp + 32
#define TEMP_B2  esp + 40
#define y_dif    esp + 48
#define dst_dif	 esp + 52
#define uv_dif   esp + 56
#define height   esp + 60
#define width_8  esp + 64
#define height_2 esp + 68
#define SCALEBITS 6

  __asm {

	push ebx
	push esi
	push edi
	push ebp

	;  local vars alloc
	sub esp, localsize

	; function code 
	mov eax, [esp + 52 + localsize]	          ; height -> eax
	cmp eax, 0x00
	jge near dont_flip		                    ; flip?

	neg eax				; neg height
	mov [height], eax		

	mov esi, [esp + 48 + localsize]	          ; width -> esi

	mov ebp, [esp + 40 + localsize]	          ; y_stride -> ebp
	mov ebx, ebp
	shl ebx, 1			                          ; 2 * y_stride -> ebx
	neg ebx
	sub ebx, esi			                        ; y_dif -> eax

	mov [y_dif], ebx

	sub eax, 1			                          ; height - 1 -> eax
	mul ebp				                            ; (height - 1) * y_stride -> ebp
	mov ecx, eax
	mov eax, [esp + 28 + localsize]	          ; y_src -> eax
	add eax, ecx			                        ; y_src -> eax
	mov ebx, eax
	sub ebx, ebp			                        ; y_src2 -> ebx

	mov ecx, [esp + 24 + localsize]	          ; dst_stride -> ecx
	mov edx, ecx
	shl edx, 3
	mov ecx, edx			                        ; 8 * dst_stride -> ecx
	shl esi, 2
	sub ecx, esi			                        ; 8 * dst_stride - 4 * width -> ecx

	mov [dst_dif], ecx

	mov esi, [esp + 20 + localsize]	          ; dst -> esi
	mov edi, esi
	shr edx, 1
	add edi, edx			                        ; dst2 -> edi

	mov ebp, [esp + 48 + localsize]	          ; width -> ebp
	mov ecx, ebp			                        ; width -> ecx
	shr ecx, 1
	shr ebp, 3			                          ; width / 8 -> ebp
	mov [width_8], ebp

	mov ebp, [esp + 44 + localsize]	; uv_stride -> ebp
	mov edx, ebp
	neg edx
	sub edx, ecx
	mov [uv_dif], edx

	mov edx, ebp
	mov ebp, eax
	mov eax, [height]		; height -> eax
	shr eax, 1			; height / 2 -> eax

	mov ecx, [esp + 32 + localsize]	; u_src -> ecx
	sub eax, 1
	mul edx
	add ecx, eax

	mov edx, [esp + 36 + localsize]	; v_src -> edx
	add edx, eax

	mov eax, ebp

	mov ebp, [height]		; height -> ebp
	shr ebp, 1			; height / 2 -> ebp

	pxor mm7, mm7
	jmp y_loop

  align 16
dont_flip:
	mov esi, [esp + 48 + localsize]	; width -> esi

	mov ebp, [esp + 40 + localsize]	; y_stride -> ebp
	mov ebx, ebp
	shl ebx, 1			; 2 * y_stride -> ebx
	sub ebx, esi			; y_dif -> ebx

	mov [y_dif], ebx

	mov eax, [esp + 28 + localsize]	; y_src -> eax
	mov ebx, eax
	add ebx, ebp			; y_src2 -> ebp

	mov ecx, [esp + 24 + localsize]	; dst_stride -> ecx
	shl ecx, 3
	mov edx, ecx			; 8 * dst_stride -> edx
	shl esi, 2
	sub ecx, esi			; 8 * dst_stride - 4 * width -> ecx

	mov [dst_dif], ecx

	mov esi, [esp + 20 + localsize]	; dst -> esi
	mov edi, esi
	shr edx, 1
	add edi, edx			; dst2 -> edi

	mov ebp, [esp + 48 + localsize]	; width -> ebp
	mov ecx, ebp			; width -> ecx
	shr ecx, 1
	shr ebp, 3			; width / 8 -> ebp
	mov [width_8], ebp

	mov ebp, [esp + 44 + localsize]	; uv_stride -> ebp
	sub ebp, ecx
	mov [uv_dif], ebp

	mov ecx, [esp + 32 + localsize]	; u_src -> ecx
	mov edx, [esp + 36 + localsize]	; v_src -> edx

	mov ebp, [esp + 52 + localsize]	; height -> ebp
	shr ebp, 1			; height / 2 -> ebp

	pxor mm7, mm7

  align 16
y_loop:
	mov [height_2], ebp
	mov ebp, [width_8]

  align 16
x_loop:
	movd mm2, [ecx]
	movd mm3, [edx]

	punpcklbw mm2, mm7		; u3u2u1u0 -> mm2
	punpcklbw mm3, mm7		; v3v2v1v0 -> mm3

	psubsw mm2, [U_SUB]		; U - 128
	psubsw mm3, [U_SUB]		; V - 128

	movq mm4, mm2
	movq mm5, mm3

	pmullw mm2, [UG_MUL]
	pmullw mm3, [VG_MUL]

	movq mm6, mm2			; u3u2u1u0 -> mm6
	punpckhwd mm2, mm2		; u3u3u2u2 -> mm2
	punpcklwd mm6, mm6		; u1u1u0u0 -> mm6

	pmullw mm4, [UB_MUL]		; B_ADD -> mm4

	movq mm0, mm3
	punpckhwd mm3, mm3		; v3v3v2v2 -> mm2
	punpcklwd mm0, mm0		; v1v1v0v0 -> mm6

	paddsw mm2, mm3
	paddsw mm6, mm0

	pmullw mm5, [VR_MUL]		; R_ADD -> mm5

	movq mm0, [eax]			; y7y6y5y4y3y2y1y0 -> mm0

	movq mm1, mm0
	punpckhbw mm1, mm7		; y7y6y5y4 -> mm1
	punpcklbw mm0, mm7		; y3y2y1y0 -> mm0

	psubsw mm0, [Y_SUB]		; Y - Y_SUB
	psubsw mm1, [Y_SUB]		; Y - Y_SUB

	pmullw mm1, [Y_MUL] 
	pmullw mm0, [Y_MUL]

	movq [TEMP_Y2], mm1		; y7y6y5y4 -> mm3
	movq [TEMP_Y1], mm0		; y3y2y1y0 -> mm7

	psubsw mm1, mm2			; g7g6g5g4 -> mm1
	psubsw mm0, mm6			; g3g2g1g0 -> mm0

	psraw mm1, SCALEBITS
	psraw mm0, SCALEBITS

	packuswb mm0, mm1		;g7g6g5g4g3g2g1g0 -> mm0

	movq [TEMP_G1], mm0

	movq mm0, [ebx]			; y7y6y5y4y3y2y1y0 -> mm0

	movq mm1, mm0

	punpckhbw mm1, mm7		; y7y6y5y4 -> mm1
	punpcklbw mm0, mm7		; y3y2y1y0 -> mm0

	psubsw mm0, [Y_SUB]		; Y - Y_SUB
	psubsw mm1, [Y_SUB]		; Y - Y_SUB

	pmullw mm1, [Y_MUL] 
	pmullw mm0, [Y_MUL]

	movq mm3, mm1
	psubsw mm1, mm2			; g7g6g5g4 -> mm1

	movq mm2, mm0
	psubsw mm0, mm6			; g3g2g1g0 -> mm0

	psraw mm1, SCALEBITS
	psraw mm0, SCALEBITS

	packuswb mm0, mm1		; g7g6g5g4g3g2g1g0 -> mm0

	movq [TEMP_G2], mm0

	movq mm0, mm4
	punpckhwd mm4, mm4		; u3u3u2u2 -> mm2
	punpcklwd mm0, mm0		; u1u1u0u0 -> mm6

	movq mm1, mm3			; y7y6y5y4 -> mm1
	paddsw mm3, mm4			; b7b6b5b4 -> mm3

	movq mm7, mm2			; y3y2y1y0 -> mm7

	paddsw mm2, mm0			; b3b2b1b0 -> mm2

	psraw mm3, SCALEBITS
	psraw mm2, SCALEBITS

	packuswb mm2, mm3		; b7b6b5b4b3b2b1b0 -> mm2

	movq [TEMP_B2], mm2

	movq mm3, [TEMP_Y2]
	movq mm2, [TEMP_Y1]

	movq mm6, mm3			; TEMP_Y2 -> mm6
	paddsw mm3, mm4			; b7b6b5b4 -> mm3

	movq mm4, mm2			; TEMP_Y1 -> mm4
	paddsw mm2, mm0			; b3b2b1b0 -> mm2

	psraw mm3, SCALEBITS
	psraw mm2, SCALEBITS

	packuswb mm2, mm3		; b7b6b5b4b3b2b1b0 -> mm2

	movq [TEMP_B1], mm2

	movq mm0, mm5
	punpckhwd mm5, mm5		; v3v3v2v2 -> mm5
	punpcklwd mm0, mm0		; v1v1v0v0 -> mm0

	paddsw mm1, mm5			; r7r6r5r4 -> mm1
	paddsw mm7, mm0			; r3r2r1r0 -> mm7

	psraw mm1, SCALEBITS
	psraw mm7, SCALEBITS

	packuswb mm7, mm1		; r7r6r5r4r3r2r1r0 -> mm7 (TEMP_R2)

	paddsw mm6, mm5			; r7r6r5r4 -> mm6
	paddsw mm4, mm0			; r3r2r1r0 -> mm4

	psraw mm6, SCALEBITS
	psraw mm4, SCALEBITS

	packuswb mm4, mm6		; r7r6r5r4r3r2r1r0 -> mm4 (TEMP_R1)
	
	movq mm0, [TEMP_B1]
	movq mm1, [TEMP_G1]

	movq mm6, mm7

	movq mm2, mm0
	punpcklbw mm2, mm4		; r3b3r2b2r1b1r0b0 -> mm2
	punpckhbw mm0, mm4		; r7b7r6b6r5b5r4b4 -> mm0

	pxor mm7, mm7

	movq mm3, mm1
	punpcklbw mm1, mm7		; 0g30g20g10g0 -> mm1
	punpckhbw mm3, mm7		; 0g70g60g50g4 -> mm3

	movq mm4, mm2
	punpcklbw mm2, mm1		; 0r1g1b10r0g0b0 -> mm2
	punpckhbw mm4, mm1		; 0r3g3b30r2g2b2 -> mm4

	movq mm5, mm0
	punpcklbw mm0, mm3		; 0r5g5b50r4g4b4 -> mm0
	punpckhbw mm5, mm3		; 0r7g7b70r6g6b6 -> mm5

	movq [esi], mm2			
	movq [esi + 8], mm4		
	movq [esi + 16], mm0	
	movq [esi + 24], mm5	

	movq mm0, [TEMP_B2]
	movq mm1, [TEMP_G2]

	movq mm2, mm0
	punpcklbw mm2, mm6		; r3b3r2b2r1b1r0b0 -> mm2
	punpckhbw mm0, mm6		; r7b7r6b6r5b5r4b4 -> mm0

	movq mm3, mm1 
	punpcklbw mm1, mm7		; 0g30g20g10g0 -> mm1
	punpckhbw mm3, mm7		; 0g70g60g50g4 -> mm3

	movq mm4, mm2
	punpcklbw mm2, mm1		; 0r1g1b10r0g0b0 -> mm2
	punpckhbw mm4, mm1		; 0r3g3b30r2g2b2 -> mm4

	movq mm5, mm0
	punpcklbw mm0, mm3		; 0r5g5b50r4g4b4 -> mm0
	punpckhbw mm5, mm3		; 0r7g7b70r6g6b6 -> mm5

	movq [edi], mm2
	movq [edi + 8], mm4
	movq [edi + 16], mm0
	movq [edi + 24], mm5

	add esi, 32
	add edi, 32

	add eax, 8
	add ebx, 8
	add ecx, 4
	add edx, 4

	dec ebp

	jnz near x_loop

	add esi, [dst_dif]
	add edi, [dst_dif]

	add eax, [y_dif]
	add ebx, [y_dif]

	add ecx, [uv_dif]
	add edx, [uv_dif]

	mov ebp, [height_2]
	dec ebp
	jnz near y_loop

	emms

	;; Local vars deallocation
	add esp, localsize
#undef TEMP_Y1
#undef TEMP_Y2
#undef TEMP_G1
#undef TEMP_G2
#undef TEMP_B1
#undef TEMP_B2
#undef y_dif
#undef dst_dif
#undef uv_dif
#undef height
#undef width_8
#undef height_2
#undef localsize
	
	pop ebp
	pop edi
	pop esi
	pop ebx

	ret
  }
}

void __declspec(naked) yv12_to_rgb24_mmx(BYTE *dst, 
                         int dst_stride, 
                         const BYTE *y_src,
                         const BYTE *u_src,
                         const BYTE *v_src, 
                         int y_stride, int uv_stride,
                         int width, int height) {

#define localsize 72
#define TEMP_Y1  esp
#define TEMP_Y2  esp + 8
#define TEMP_G1  esp + 16
#define TEMP_G2  esp + 24
#define TEMP_B1  esp + 32
#define TEMP_B2  esp + 40
#define y_dif    esp + 48
#define dst_dif	 esp + 52
#define uv_dif   esp + 56
#define height   esp + 60
#define width_8  esp + 64
#define height_2 esp + 68
#define SCALEBITS 6

  __asm {

	push ebx
	push esi
	push edi
	push ebp

	;  local vars alloc
	sub esp, localsize


	mov eax, [esp + 52 + localsize]	; height -> eax
	cmp eax, 0x00
	jge near dont_flip		; flip?
	
	neg eax
	mov [height], eax

	mov esi, [esp + 48 + localsize]	; width -> esi

	mov ebp, [esp + 40 + localsize]	; y_stride -> ebp
	mov ebx, ebp
	shl ebx, 1			; 2 * y_stride -> ebx
	neg ebx
	sub ebx, esi			; y_dif -> eax

	mov [y_dif], ebx

	sub eax, 1			; height - 1 -> eax
	mul ebp				; (height - 1) * y_stride -> ebp
	mov ecx, eax
	mov eax, [esp + 28 + localsize]	; y_src -> eax
	add eax, ecx			; y_src -> eax
	mov ebx, eax
	sub ebx, ebp			; y_src2 -> ebx

	mov ecx, [esp + 24 + localsize]	; dst_stride -> ecx
	mov edx, ecx
	add ecx, edx
	shl edx, 2
	add ecx, edx			; 6 * dst_stride -> ecx
	mov edx, ecx
	sub ecx, esi
	shl esi, 1
	sub ecx, esi			; 6 * dst_stride - 3 * width -> ecx

	mov [dst_dif], ecx

	mov esi, [esp + 20 + localsize]	; dst -> esi
	mov edi, esi
	shr edx, 1
	add edi, edx			; dst2 -> edi

	mov ebp, [esp + 48 + localsize]	; width -> ebp
	mov ecx, ebp			; width -> ecx
	shr ecx, 1
	shr ebp, 3			; width / 8 -> ebp
	mov [width_8], ebp

	mov ebp, [esp + 44 + localsize]	; uv_stride -> ebp
	mov edx, ebp
	neg edx
	sub edx, ecx
	mov [uv_dif], edx

	mov edx, ebp
	mov ebp, eax
	mov eax, [height]		; height -> eax
	shr eax, 1			; height / 2 -> eax

	mov ecx, [esp + 32 + localsize]	; u_src -> ecx
	sub eax, 1
	mul edx
	add ecx, eax

	mov edx, [esp + 36 + localsize]	; v_src -> edx
	add edx, eax

	mov eax, ebp

	mov ebp, [height]		; height -> ebp
	shr ebp, 1			; height / 2 -> ebp

	pxor mm7, mm7			; clear mm7
	jmp y_loop


  align 16
dont_flip:
	mov esi, [esp + 48 + localsize]	; width -> esi

	mov ebp, [esp + 40 + localsize]	; y_stride -> ebp
	mov ebx, ebp
	shl ebx, 1			; 2 * y_stride -> ebx
	sub ebx, esi			; y_dif -> ebx

	mov [y_dif], ebx

	mov eax, [esp + 28 + localsize]	; y_src -> eax
	mov ebx, eax
	add ebx, ebp			; y_src2 -> ebp

	mov ecx, [esp + 24 + localsize]	; dst_stride -> ecx
	mov edx, ecx
	add ecx, edx
	shl edx, 2
	add ecx, edx			; 6 * dst_stride -> ecx
	mov edx, ecx
	sub ecx, esi
	shl esi, 1
	sub ecx, esi			; 6 * dst_stride - 3 * width -> ecx

	mov [dst_dif], ecx

	mov esi, [esp + 20 + localsize]	; dst -> esi
	mov edi, esi
	shr edx, 1
	add edi, edx			; dst2 -> edi

	mov ebp, [esp + 48 + localsize]	; width -> ebp
	mov ecx, ebp			; width -> ecx
	shr ecx, 1
	shr ebp, 3			; width / 8 -> ebp
	mov [width_8], ebp

	mov ebp, [esp + 44 + localsize]	; uv_stride -> ebp
	sub ebp, ecx
	mov [uv_dif], ebp

	mov ecx, [esp + 32 + localsize]	; u_src -> ecx
	mov edx, [esp + 36 + localsize]	; v_src -> edx

	mov ebp, [esp + 52 + localsize]	; height -> ebp
	shr ebp, 1			; height / 2 -> ebp

	pxor mm7, mm7

  align 16
y_loop:
	mov [height_2], ebp
	mov ebp, [width_8]

  align 16
x_loop:
	movd mm2, [ecx]
	movd mm3, [edx]

	punpcklbw mm2, mm7		; u3u2u1u0 -> mm2
	punpcklbw mm3, mm7		; v3v2v1v0 -> mm3

	psubsw mm2, [U_SUB]		; U - 128
	psubsw mm3, [U_SUB]		; V - 128

	movq mm4, mm2
	movq mm5, mm3

	pmullw mm2, [UG_MUL]
	pmullw mm3, [VG_MUL]

	movq mm6, mm2			; u3u2u1u0 -> mm6
	punpckhwd mm2, mm2		; u3u3u2u2 -> mm2
	punpcklwd mm6, mm6		; u1u1u0u0 -> mm6

	pmullw mm4, [UB_MUL]		; B_ADD -> mm4

	movq mm0, mm3
	punpckhwd mm3, mm3		; v3v3v2v2 -> mm2
	punpcklwd mm0, mm0		; v1v1v0v0 -> mm6

	paddsw mm2, mm3
	paddsw mm6, mm0

	pmullw mm5, [VR_MUL]		; R_ADD -> mm5

	movq mm0, [eax]			; y7y6y5y4y3y2y1y0 -> mm0

	movq mm1, mm0
	punpckhbw mm1, mm7		; y7y6y5y4 -> mm1
	punpcklbw mm0, mm7		; y3y2y1y0 -> mm0

	psubsw mm0, [Y_SUB]		; Y - Y_SUB
	psubsw mm1, [Y_SUB]		; Y - Y_SUB

	pmullw mm1, [Y_MUL] 
	pmullw mm0, [Y_MUL]

	movq [TEMP_Y2], mm1		; y7y6y5y4 -> mm3
	movq [TEMP_Y1], mm0		; y3y2y1y0 -> mm7

	psubsw mm1, mm2			; g7g6g5g4 -> mm1
	psubsw mm0, mm6			; g3g2g1g0 -> mm0

	psraw mm1, SCALEBITS
	psraw mm0, SCALEBITS

	packuswb mm0, mm1		;g7g6g5g4g3g2g1g0 -> mm0

	movq [TEMP_G1], mm0

	movq mm0, [ebx]			; y7y6y5y4y3y2y1y0 -> mm0

	movq mm1, mm0

	punpckhbw mm1, mm7		; y7y6y5y4 -> mm1
	punpcklbw mm0, mm7		; y3y2y1y0 -> mm0

	psubsw mm0, [Y_SUB]		; Y - Y_SUB
	psubsw mm1, [Y_SUB]		; Y - Y_SUB

	pmullw mm1, [Y_MUL] 
	pmullw mm0, [Y_MUL]

	movq mm3, mm1
	psubsw mm1, mm2			; g7g6g5g4 -> mm1

	movq mm2, mm0
	psubsw mm0, mm6			; g3g2g1g0 -> mm0

	psraw mm1, SCALEBITS
	psraw mm0, SCALEBITS

	packuswb mm0, mm1		; g7g6g5g4g3g2g1g0 -> mm0

	movq [TEMP_G2], mm0

	movq mm0, mm4
	punpckhwd mm4, mm4		; u3u3u2u2 -> mm2
	punpcklwd mm0, mm0		; u1u1u0u0 -> mm6

	movq mm1, mm3			; y7y6y5y4 -> mm1
	paddsw mm3, mm4			; b7b6b5b4 -> mm3

	movq mm7, mm2			; y3y2y1y0 -> mm7

	paddsw mm2, mm0			; b3b2b1b0 -> mm2

	psraw mm3, SCALEBITS
	psraw mm2, SCALEBITS

	packuswb mm2, mm3		; b7b6b5b4b3b2b1b0 -> mm2

	movq [TEMP_B2], mm2

	movq mm3, [TEMP_Y2]
	movq mm2, [TEMP_Y1]

	movq mm6, mm3			; TEMP_Y2 -> mm6
	paddsw mm3, mm4			; b7b6b5b4 -> mm3

	movq mm4, mm2			; TEMP_Y1 -> mm4
	paddsw mm2, mm0			; b3b2b1b0 -> mm2

	psraw mm3, SCALEBITS
	psraw mm2, SCALEBITS

	packuswb mm2, mm3		; b7b6b5b4b3b2b1b0 -> mm2

	movq [TEMP_B1], mm2

	movq mm0, mm5
	punpckhwd mm5, mm5		; v3v3v2v2 -> mm5
	punpcklwd mm0, mm0		; v1v1v0v0 -> mm0

	paddsw mm1, mm5			; r7r6r5r4 -> mm1
	paddsw mm7, mm0			; r3r2r1r0 -> mm7

	psraw mm1, SCALEBITS
	psraw mm7, SCALEBITS

	packuswb mm7, mm1		; r7r6r5r4r3r2r1r0 -> mm7 (TEMP_R2)

	paddsw mm6, mm5			; r7r6r5r4 -> mm6
	paddsw mm4, mm0			; r3r2r1r0 -> mm4

	psraw mm6, SCALEBITS
	psraw mm4, SCALEBITS

	packuswb mm4, mm6		; r7r6r5r4r3r2r1r0 -> mm4 (TEMP_R1)
	
	movq mm0, [TEMP_B1]
	movq mm1, [TEMP_G1]

	movq mm6, mm7

	movq mm2, mm0
	punpcklbw mm2, mm4		; r3b3r2b2r1b1r0b0 -> mm2
	punpckhbw mm0, mm4		; r7b7r6b6r5b5r4b4 -> mm0

	pxor mm7, mm7

	movq mm3, mm1
	punpcklbw mm1, mm7		; 0g30g20g10g0 -> mm1
	punpckhbw mm3, mm7		; 0g70g60g50g4 -> mm3

	movq mm4, mm2
	punpcklbw mm2, mm1		; 0r1g1b10r0g0b0 -> mm2
	punpckhbw mm4, mm1		; 0r3g3b30r2g2b2 -> mm4

	movq mm5, mm0
	punpcklbw mm0, mm3		; 0r5g5b50r4g4b4 -> mm0
	punpckhbw mm5, mm3		; 0r7g7b70r6g6b6 -> mm5

	movd [esi], mm2			
	psrlq mm2, 32

	movd [esi + 3], mm2
	movd [esi + 6], mm4		

	psrlq mm4, 32

	movd [esi + 9], mm4
	movd [esi + 12], mm0	

	psrlq mm0, 32

	movd [esi + 15], mm0
	movd [esi + 18], mm5	

	psrlq mm5, 32

	movd [esi + 21], mm5	

	movq mm0, [TEMP_B2]
	movq mm1, [TEMP_G2]

	movq mm2, mm0
	punpcklbw mm2, mm6		; r3b3r2b2r1b1r0b0 -> mm2
	punpckhbw mm0, mm6		; r7b7r6b6r5b5r4b4 -> mm0

	movq mm3, mm1 
	punpcklbw mm1, mm7		; 0g30g20g10g0 -> mm1
	punpckhbw mm3, mm7		; 0g70g60g50g4 -> mm3

	movq mm4, mm2
	punpcklbw mm2, mm1		; 0r1g1b10r0g0b0 -> mm2
	punpckhbw mm4, mm1		; 0r3g3b30r2g2b2 -> mm4

	movq mm5, mm0
	punpcklbw mm0, mm3		; 0r5g5b50r4g4b4 -> mm0
	punpckhbw mm5, mm3		; 0r7g7b70r6g6b6 -> mm5

	movd [edi], mm2
	psrlq mm2, 32

	movd [edi + 3], mm2
	movd [edi + 6], mm4

	psrlq mm4, 32

	movd [edi + 9], mm4
	movd [edi + 12], mm0

	psrlq mm0, 32

	movd [edi + 15], mm0
	movd [edi + 18], mm5

	psrlq mm5, 32

	movd [edi + 21], mm5

	add esi, 24
	add edi, 24

	add eax, 8
	add ebx, 8
	add ecx, 4
	add edx, 4

	dec ebp

	jnz near x_loop

	add esi, [dst_dif]
	add edi, [dst_dif]

	add eax, [y_dif]
	add ebx, [y_dif]

	add ecx, [uv_dif]
	add edx, [uv_dif]

	mov ebp, [height_2]
	dec ebp
	jnz near y_loop

	emms

	;; Local vars deallocation
	add esp, localsize
#undef TEMP_Y1
#undef TEMP_Y2
#undef TEMP_G1
#undef TEMP_G2
#undef TEMP_B1
#undef TEMP_B2
#undef y_dif
#undef dst_dif
#undef uv_dif
#undef height
#undef width_8
#undef height_2
#undef localsize
	
	pop ebp
	pop edi
	pop esi
	pop ebx

	ret
  }
}
