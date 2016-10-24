#pragma once

#include "ods.h"

#define PTHREAD_SUFFIX



class CriticalSection
{
public:
	CriticalSection()
	{
		::InitializeCriticalSection(&m_section PTHREAD_SUFFIX);
	}

	~CriticalSection()
	{
		::DeleteCriticalSection(&m_section);
	}

	void Enter()
	{
		::EnterCriticalSection(&m_section);
	}

	void Leave()
	{
		::LeaveCriticalSection(&m_section);
	}

	bool TryEnter()
	{
		return ::TryEnterCriticalSection(&m_section);
	}

private:
	CRITICAL_SECTION	m_section;
};

class Thread
{
public:
	Thread()
		: m_hThread(NULL)
		, m_dwThreadId(0)
	{
	};

	~Thread()
	{
		if (NULL != m_hThread) 
			::CloseHandle(m_hThread);
	}

	bool Start(LPTHREAD_START_ROUTINE fnRoutine, LPVOID lpParameter)
	{
		if (NULL != m_hThread) return FALSE;

		m_hThread = ::CreateThread(NULL, 0, fnRoutine, lpParameter, 0, &m_dwThreadId);
		return (NULL != m_hThread);
	}

	bool WaitForEnd(DWORD dwTimeoutMS = INFINITE)
	{
		if (NULL == m_hThread) return TRUE;

		DWORD dwRet = 0;


		dwRet = ::WaitForSingleObject(m_hThread, dwTimeoutMS);

		if (WAIT_OBJECT_0 == dwRet)
		{
			m_hThread = NULL;
			m_dwThreadId = 0;

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	void Terminate()
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	bool IsRunning()
	{
		return ! WaitForEnd(0);
	}

private:
	HANDLE		m_hThread;
	DWORD		m_dwThreadId;
};
