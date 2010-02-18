/*
 *  Part of the Matroska Shell Extension
 *
 *  DShowTools.h
 *
 *  Copyright (C) Jory Stone - June 2003
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
    \file CxImageRenderer.h
		\version \$Id: DShowTools.h,v 1.3 2004/03/08 07:56:51 jcsston Exp $
    \brief A DirectShow Video Renderer that renders the image to a CxImage
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#ifndef _CXIMAGE_RENDERER_H_
#define _CXIMAGE_RENDERER_H_

#include <windows.h>
#include "CxImage/ximage.h"

#define DSHOWTOOLS_DLL_EXPORT
#ifndef DSHOWTOOLS_DLL_EXPORT
#ifdef DSHOWTOOLS_DLL
#define DSHOWTOOLS_DLL_EXPORT __declspec(dllexport)
#else
#define DSHOWTOOLS_DLL_EXPORT __declspec(dllimport)
#endif
#endif

// Mode 1 = CxImageRenderer
// Mode 2 = IMediaDet
extern "C" HRESULT DSHOWTOOLS_DLL_EXPORT GetDShowPreview(LPCWSTR filename, DWORD second_offset, DWORD timeout, DWORD mode, CxImage *image);
extern "C" HRESULT DSHOWTOOLS_DLL_EXPORT PlayFile(LPCWSTR filename, DWORD mode);

#endif // _CXIMAGE_RENDERER_H_
