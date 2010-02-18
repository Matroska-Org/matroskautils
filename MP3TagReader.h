/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  MP3TagReader.h
 *
 *  Copyright (C) Jory Stone - 2003
 *
 *  The TCMP Matroska CDL is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  The TCMP Matroska CDL is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The TCMP Matroska CDL; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*!
    \file AviTagReader.h
		\version \$Id: MP3TagReader.h,v 1.3 2004/02/09 23:14:14 jcsston Exp $
    \brief A MP3 ID3 Tag Importer
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#ifndef _MP3_TAG_READER_H_
#define _MP3_TAG_READER_H_

#include "TagReader.h"
#include <time.h>
#include "MatroskaUtilsReader.h"

#ifdef USING_ID3LIB
// Dll link
#define ID3LIB_LINKOPTION 3
// Static link
//#define ID3LIB_LINKOPTION 1

#include <id3/tag.h>
//#include "id3.h"
#endif

class MP3TagReader : public TagReader {
public:
	MP3TagReader(const JString &filename);
	~MP3TagReader();

	void ReadFile(const JString &filename);
	bool HasTags();
	void ImportTags(MatroskaTagInfo *target);

protected:
	JString m_MP3Filename;	

#ifdef USING_ID3LIB
	ID3_Tag m_Tag;
#endif
};

#endif // _MP3_TAG_READER_H_
