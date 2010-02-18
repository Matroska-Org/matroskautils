/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  AviTagReader.h
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
		\version \$Id: AviTagReader.h,v 1.2 2003/10/05 23:58:05 jcsston Exp $
    \brief A AVI Tag Importer
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#ifndef _AVI_TAG_READER_H_
#define _AVI_TAG_READER_H_

#include "TagReader.h"
#include <time.h>
#include "MatroskaUtilsReader.h"

using namespace MatroskaUtilsNamespace;

struct iDivxTag {
	char Movie[32];
	char Author[28];
	char Year[4];
	char Comment[48];
	char Genre[3];
	char Rating;
	char Header[12];
};

struct AviTag {
	std::string id;
	std::string value;
};

class AviTagReader : public TagReader {
public:
	AviTagReader(const JString &filename);

	void ReadFile(const JString &filename);
	bool HasTags();
	void ImportTags(MatroskaTagInfo *target);

protected:
	int64 GetDate(const std::string &dateStr);
	JString m_AviFilename;
	std::vector<AviTag> m_Tags;
	iDivxTag m_DivxTag;
};

#endif // _AVI_TAG_READER_H_
