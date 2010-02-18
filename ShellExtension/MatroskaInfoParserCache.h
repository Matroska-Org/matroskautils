/*
 *  Matroska Shell Extension
 *
 *  MatroskaInfoParserCache.h
 *
 *  Copyright (C) Jory Stone - June 2003 - December 2004
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
    \file MatroskaInfoParserCache.h
		\version \$Id: MatroskaProp.cpp,v 1.39 2004/03/08 07:56:51 jcsston Exp $
    \brief Lightweight caching Matroska Parser object list
		\author Jory Stone     <matroskaprop @ jory.info>

*/

#ifndef _MATROSKA_INFO_PARSER_CACHE_H_
#define _MATROSKA_INFO_PARSER_CACHE_H_

#include <windows.h>
#include "MatroskaPages.h"

class MatroskaInfoParserCache {
public:
	MatroskaInfoParserCache();
	~MatroskaInfoParserCache();
	
	void AddParser(MatroskaInfoParser *parser);
	MatroskaInfoParser *Lookup(const wchar_t *filename);

#ifdef DEBUG
	void OutputList();
#endif
protected:
	int m_CacheCount;
	int m_CacheLimit;
	struct MatroskaInfoParserCacheEntry {
		MatroskaInfoParser *parser;
		MatroskaInfoParserCacheEntry *next;
	};
	MatroskaInfoParserCacheEntry *m_First;
	MatroskaInfoParserCacheEntry *m_Last;
};

#endif // _MATROSKA_INFO_PARSER_CACHE_H_
