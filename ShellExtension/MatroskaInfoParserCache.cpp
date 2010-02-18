/*
 *  Matroska Shell Extension
 *
 *  MatroskaInfoParserCache.cpp
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
    \file MatroskaInfoParserCache.cpp
		\version \$Id: MatroskaProp.cpp,v 1.39 2004/03/08 07:56:51 jcsston Exp $
    \brief Lightweight caching Matroska Parser object list
		\author Jory Stone     <matroskaprop @ jory.info>

*/

#include "MatroskaInfoParserCache.h"

MatroskaInfoParserCache::MatroskaInfoParserCache()
{
	m_First = new MatroskaInfoParserCacheEntry;
	m_First->parser = NULL;
	m_First->next = NULL;
	m_Last = m_First;
	m_CacheCount = 0;

	//Load the ParserCacheLimit
	m_CacheLimit = MatroskaShellExt_GetRegistryValue(_T("Parser Cache Limit"), 7);
	if ((m_CacheLimit > 255) || (m_CacheLimit < 1))
		//Out of range, using default 7
		m_CacheLimit = 7;

}

MatroskaInfoParserCache::~MatroskaInfoParserCache()
{
	while (m_Last != NULL) {
		m_Last = m_First->next;
		delete m_First->parser;
		delete m_First;
		m_First = m_Last;
	}
}

void MatroskaInfoParserCache::AddParser(MatroskaInfoParser *parser)
{
	NOTE1("MatroskaInfoParserCache::AddParser(parser = %p)", parser);
	if (m_CacheCount > m_CacheLimit) {
		// Reuse the first entry
		m_Last->next = m_First;
		m_Last = m_First;
		delete m_First->parser;
		m_First = m_Last->next;
		// This line is not really needed, because it's repeated a few line below
		//m_Last->next = NULL;
		m_CacheCount--;
	} else {
		// Alloc a new entry
		m_Last->next = new MatroskaInfoParserCacheEntry;
		m_Last = m_Last->next;
	}
	m_Last->parser = parser;
	m_Last->next = NULL;
	
	m_CacheCount++;
#ifdef DEBUG
	OutputList();
#endif
}

MatroskaInfoParser *MatroskaInfoParserCache::Lookup(const wchar_t *filename)
{
	NOTE1("MatroskaInfoParserCache::Lookup(filename = %S)", filename);

	MatroskaInfoParserCacheEntry *temp = m_First;

	while (temp != NULL) {
		if ((temp->parser != NULL) && !wcscmp(filename, temp->parser->GetFilename()))
			return temp->parser;

		temp = temp->next;
	}

	return NULL;
}

#ifdef DEBUG
void MatroskaInfoParserCache::OutputList()
{
	int i = 0;
	MatroskaInfoParserCacheEntry *temp = m_First;

	NOTE("MatroskaInfoParserCache Report");
	NOTE("==============================");
	while (temp != NULL) {
		i++;
		if (temp->parser != NULL)
			NOTE2("%i : %S", i, temp->parser->GetFilename());

		temp = temp->next;
	}
	NOTE("==============================");
}
#endif
