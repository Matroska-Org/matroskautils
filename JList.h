/**
	*  Part of The TCMP Matroska CDL, a plugin to access extra features of Matroska files with TCMP
	*
	*  JList.h
	*
	*  Copyright (C) Jory Stone - June 2003
	*
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
    \file JList.h
		\version \$Id: JList.h,v 1.1 2003/10/04 21:57:18 jcsston Exp $
    \brief A simple linked-list
		\author Jory Stone     <jcsston @ toughguy.net>
*/

#ifndef _JLIST_H_
#define _JLIST_H_

template<class _T> class JListItem {
public:
	JListItem() {
		m_Next = NULL;
	};
	_T m_Data;
	JListItem<_T> *m_Next;
};

/// Automaticly calls delete on data type, you should use pointers ;)
template<class _T> class JList{
public:
	JList() {
		m_FirstItem = NULL;
	};
	~JList() {
		JListItem<_T> *temp = NULL;
		temp = m_FirstItem;
		while (m_FirstItem != NULL)
		{		 
			m_FirstItem = m_FirstItem->m_Next;

			delete temp->m_Data;
			delete temp;
			temp = m_FirstItem;
		}
	};

	void AddItem(_T newData) {
		JListItem<_T> *objData = new JListItem<_T>();
		objData->m_Data = newData;		
		if (m_FirstItem == NULL) {
			m_FirstItem = objData;			
		} else {
			objData->m_Next =	m_FirstItem;
			m_FirstItem = objData;
		}
	};
protected:
	JListItem<_T> *m_FirstItem;
};

#endif // _JLIST_H_
