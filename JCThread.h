/*
 * Part of The Matroska Shell Extension, a Windows Explorer shell ext to access extra features of Matroska files
 *
 * JCThread.h
 *
 * Copyright (C) 2003 Jory 'jcsston' Stone <jcsston@hotpop.com>
 *
 * The Matroska Shell Extension is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The Matroska Shell Extension is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*!
	\version \$Id: JCThread.h,v 1.2 2004/03/08 07:56:51 jcsston Exp $
	\author Jory Stone            <jcsston @ toughguy.net>	
 	\brief Thread base class, designed to be replacable with the wxThread class
	
 	\par CVS Log 
  	$Log: JCThread.h,v $
  	Revision 1.2  2004/03/08 07:56:51  jcsston
  	Fixed some bugs in the bitrate scaning code
  	Fixed a bug in the JCThread class, sometimes the thread would exit too quickly
  	Fixed a bug in the attachment opening code
  	
  	Revision 1.1  2003/11/29 02:04:48  jcsston
  	Changed to my JCCritcSec class for multi-threading, Started bitrate class
  	
  	Revision 1.1  2003/11/24 21:55:29  jcsston
  	Intial Commit, decoding working
  	  			
*/

#ifndef _JC_THREAD_H_
#define _JC_THREAD_H_

#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <assert.h>

// wrapper for whatever critical section we have
class JCCritSec {
public:
	JCCritSec() {
		InitializeCriticalSection(&m_CritSec);
	};
	~JCCritSec() {
		DeleteCriticalSection(&m_CritSec);
	};
	void Lock() {
		EnterCriticalSection(&m_CritSec);
	};
	void Unlock() {
		LeaveCriticalSection(&m_CritSec);
	};

protected:
	CRITICAL_SECTION m_CritSec;
private:
	// Make copy constructor and assignment operator inaccessible
	// Copying a critical section is a bad idea :P
	JCCritSec(const JCCritSec &refCritSec) { };
	JCCritSec &operator=(const JCCritSec &refCritSec) { return *this; };
};

// Auto-Lock class
class JCAutoLockCritSec {
public:
	JCAutoLockCritSec(JCCritSec *lockCritSec) {
		m_CritSec = lockCritSec;
		m_CritSec->Lock();
	};
	~JCAutoLockCritSec() {
		m_CritSec->Unlock();
	};

protected:
	JCCritSec *m_CritSec;
private:
	// Make copy constructor and assignment operator inaccessible
	// Copying a critical section is a bad idea :P
	JCAutoLockCritSec(const JCAutoLockCritSec &refCritSec) { assert(!"We should never ever be here!"); };
	JCAutoLockCritSec &operator=(const JCAutoLockCritSec &refCritSec) { assert(!"We should never ever be here!"); return *this; };
};

class JCThread {
private:
	static void __cdecl JCThreadFunction(void *threadData) {		
		JCThread *threadObj = (JCThread *)threadData;
		// Check thread pointer
		assert(threadObj != NULL);

		threadObj->JCThread_AddRef();

		threadObj->m_StartedCrit.Lock();
		threadObj->m_bStarted = true;
		threadObj->m_StartedCrit.Unlock();
		threadObj->Entry();	

		threadObj->JCThread_RemoveRef();
		
		_endthread();
	};
public:
	JCThread() { 
		m_hThread = NULL;
		m_State = JCThread_State_Waiting;
		m_bStarted = false;
		m_RefCount = 0;
	};
	virtual ~JCThread() { 
		/*if (GetState() == JCThread_State_Running) {
			// We need to stop the thread before deleting it
			Stop();
		}*/
	};

	void Run() {
		JCThread_AddRef();
		m_StateCrit.Lock();
		m_State = JCThread_State_Running;
		m_StateCrit.Unlock();
		m_hThread = (HANDLE)_beginthread(JCThreadFunction, 0, (void *)this);
		// Wait for the thread to start
		while (!TestDestroy()) {
			m_StartedCrit.Lock();
			if (m_bStarted) {
				m_StartedCrit.Unlock();
				break;
			}
			m_StartedCrit.Unlock();
			Sleep(50);
		}
		JCThread_RemoveRef();
	}
	void Stop() {
		m_StateCrit.Lock();
		m_State = JCThread_State_Stopped;
		m_StateCrit.Unlock();		
	}
	void Pause() {
		JCAutoLockCritSec stateLock(&m_StateCrit);
		if (m_State == JCThread_State_Running) {
			if (SuspendThread(m_hThread) != 0xFFFFFFFF)
				m_State = JCThread_State_Paused;
		}
	}
	void Resume() {
		JCAutoLockCritSec stateLock(&m_StateCrit);
		if (m_State == JCThread_State_Paused) {
			if (ResumeThread(m_hThread) != 0xFFFFFFFF)
				m_State = JCThread_State_Running;
		}
	}
	
	virtual void *Entry() = 0;	

	int GetState() {
		int localState;
		m_StateCrit.Lock();
		localState = (int)m_State;
		m_StateCrit.Unlock();
		return localState;
	};

	long JCThread_AddRef() {
		JCAutoLockCritSec refCountLock(&m_RefCountCrit);		
		return m_RefCount++;
	}
	long JCThread_RemoveRef() {
    {
		  JCAutoLockCritSec refCountLock(&m_RefCountCrit);	

		  if (--m_RefCount)
			  return m_RefCount;
    }

		delete this;

		return 0L;
	}

protected:	
	bool TestDestroy() {
		bool bExit = false;
		m_StateCrit.Lock();
		if (m_State == JCThread_State_Stopped)
			bExit = true;
		m_StateCrit.Unlock();
		return bExit;
	}
	HANDLE m_hThread;
	JCCritSec m_StateCrit;
	enum JCThread_State { 
		JCThread_State_Waiting = 0,
		JCThread_State_Stopped,
		JCThread_State_Running,
		JCThread_State_Paused
	};
	JCThread_State m_State;
	JCCritSec m_StartedCrit;
	bool m_bStarted;
	JCCritSec m_RefCountCrit;
	long m_RefCount;
};



#endif // _JC_THREAD_H_