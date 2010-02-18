/*
 *  Part of the Matroska Shell Extension
 *
 *  convert.h
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
    \file convert.h
		\version \$Id: convert.h,v 1.2 2004/04/28 04:21:55 jcsston Exp $
    \brief YUV Image Conversion Code
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#ifndef _CONVERT_H_
#define _CONVERT_H_

#include <windows.h>

extern "C" {
//Declare for assembly YUV2->RGB32 conversion
void _stdcall YUV422toRGB_MMX(void* lpIn,void* lpOut,DWORD dwFlags,DWORD dwWidth,DWORD dwHeight,DWORD dwSPitch,DWORD dwDPitch);
void _stdcall YUV422toRGB24_MMX(void* lpIn,void* lpOut,DWORD dwFlags,DWORD dwWidth,DWORD dwHeight,DWORD dwSPitch,DWORD dwDPitch);
//I don't use these, but it's nice to have the declares
void _stdcall RGBtoYCrCb_SSE2(void* lpIn,void* lpOut,DWORD dwFlags,DWORD dwWidth,DWORD dwHeight,DWORD dwSPitch,DWORD dwDPitch); 
void _stdcall RGBtoYUV422_SSE2(void* lpIn,void* lpOut,DWORD dwFlags,DWORD dwWidth,DWORD dwHeight,DWORD dwSPitch,DWORD dwDPitch); 
};

// from convert_yv12.cpp
void mmx_yv12_to_yuy2(const BYTE* srcY, const BYTE* srcU, const BYTE* srcV, int src_rowsize, int src_pitch, int src_pitch_uv, 
                    BYTE* dst, int dst_pitch,
                    int height);

// from convert_xvid.cpp
void yv12_to_rgb32_mmx(BYTE *dst, 
                         int dst_stride, 
                         const BYTE *y_src,
                         const BYTE *u_src,
                         const BYTE *v_src, 
                         int y_stride, int uv_stride,
                         int width, int height);
void yv12_to_rgb24_mmx(BYTE *dst, //Currently not used (buggy!)
                         int dst_stride, 
                         const BYTE *y_src,
                         const BYTE *u_src,
                         const BYTE *v_src, 
                         int y_stride, int uv_stride,
                         int width, int height);


#endif // _CONVERT_H_
