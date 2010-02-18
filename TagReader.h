/*
 *  Part of The TCMP Matroska CDL, and Matroska Shell Extension
 *
 *  TagReader.h
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
    \file TagReader.h
		\version \$Id: TagReader.h,v 1.2 2003/10/07 09:03:13 jcsston Exp $
    \brief Tag Importer Base class
		\author Jory Stone     <jcsston @ toughguy.net>

*/

#ifndef _TAG_READER_H_
#define _TAG_READER_H_

#include "JString.h"
#include <string>
#include <string.h>
#include "MatroskaTagData.h"

using namespace MatroskaUtilsNamespace;

class TagReader {
public:
	virtual void ReadFile(const JString &filename) = 0;
	virtual bool HasTags() = 0;
	virtual void ImportTags(MatroskaTagInfo *target) = 0;
};

#endif // _TAG_READER_H_
