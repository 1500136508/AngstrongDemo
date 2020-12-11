#include "HalerThread.h"
#include <process.h>

std::mutex CHalerThread::thread_mutex;
ThreadContext CHalerThread::m_CurContext;
std::map<int,unsigned int> CHalerThread::m_mpThreadID;
std::map<int, EThreadStatus> CHalerThread::m_mpThreadStatus;
std::map<int, ThreadContext> CHalerThread::m_mpThreadContext;
std::map<int, HANDLE> CHalerThread::m_mpThreadHandle;
CHalerThread* m_pInstance = nullptr;

CHalerThread::CHalerThread()
{

}

CHalerThread::~CHalerThread()
{
	if (m_pInstance)
	{
		delete[] m_pInstance;
		m_pInstance = nullptr;
	}
}

CHalerThread * CHalerThread::GetInstance()
{
	std::unique_lock<std::mutex> locker(thread_mutex);
	if (!m_pInstance)
	{
		m_pInstance = new CHalerThread;
	}
	locker.unlock();
	return m_pInstance;
}

CHalerThread CHalerThread::GetInstatnce()
{
	std::unique_lock<std::mutex> locker(thread_mutex);
	if (!m_pInstance)
	{
		m_pInstance = new CHalerThread;
	}
	locker.unlock();
	return *m_pInstance;
}

EThreadStatus CHalerThread::GetThreadStatus(long nThreadSequence)
{
	EThreadStatus m_ThreadStatus = EThreadStatus_Unknown;
	std::map<int, EThreadStatus> ::iterator m_itor_ThreadStatus;

	std::unique_lock<std::mutex> locker(thread_mutex);
	if (m_mpThreadStatus.find(nThreadSequence) != m_mpThreadStatus.end())
	{
		m_itor_ThreadStatus = m_mpThreadStatus.find(nThreadSequence);
		m_ThreadStatus = m_itor_ThreadStatus->second;
	}
	locker.unlock();
	return m_ThreadStatus;
}

long CHalerThread::GetThreadCount()
{
	long m_nThreadCount = 0;
	m_nThreadCount = m_mpThreadHandle.size();

	return m_nThreadCount;
}

bool CHalerThread::Run(ThreadContext pContext, long nThreadSequence, bool bCreateSuspended)
{
	bool bReturn = false;

	do 
	{
		if (!pContext)
			break;

		m_CurContext = pContext;
	
		HANDLE m_hHandle;
		unsigned int m_ThreadID;
		std::map<int, HANDLE>::iterator m_itor_ThreadHandle;

		if (bCreateSuspended)
		{
			m_hHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, HalerThreadFunction, nullptr, CREATE_SUSPENDED, &m_ThreadID));

			m_mpThreadStatus.insert(std::map<int, EThreadStatus>::value_type(nThreadSequence, EThreadStatus_Suspend));
		} 
		else
		{
			m_hHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, HalerThreadFunction, nullptr, 0, &m_ThreadID));

			m_mpThreadStatus.insert(std::map<int, EThreadStatus>::value_type(nThreadSequence, EThreadStatus_Run));
		}
		
		m_mpThreadID.insert(std::map<int, unsigned int>::value_type(nThreadSequence, m_ThreadID));
		m_mpThreadContext.insert(std::map<int, ThreadContext>::value_type(nThreadSequence, pContext));
		m_mpThreadHandle.insert(std::map<int, HANDLE>::value_type(nThreadSequence, m_hHandle));

		int m_ThreadID_size = m_mpThreadID.size();
		int m_ThreadStatus_size = m_mpThreadStatus.size();
		int m_ThreadContext_size = m_mpThreadContext.size();
		int m_ThreadHandle_size = m_mpThreadHandle.size();

		Sleep(50);

		bReturn = true;
	} while (false);

	return bReturn;
}

bool CHalerThread::Suspend(long nThreadSequence, bool bSuspendAll)
{
	bool bReturn = false;

	do 
	{
		std::map<int, ThreadContext> ::iterator m_itor_ThreadContext;
		std::map<int, HANDLE> ::iterator m_itor_Handle;

		if (bSuspendAll)
		{
			for (m_itor_Handle = m_mpThreadHandle.begin(); m_itor_Handle != m_mpThreadHandle.end(); ++m_itor_Handle)
			{
				Sleep(60);
				if (m_mpThreadContext.find(m_itor_Handle->first) != m_mpThreadContext.begin())
				{
					m_itor_ThreadContext = m_mpThreadContext.find(m_itor_Handle->first);
					m_CurContext = m_itor_ThreadContext->second;
				}

				::SuspendThread(m_itor_Handle->second);
				m_mpThreadStatus[m_itor_Handle->first] = EThreadStatus_Suspend;
			}
		}
		else
		{
			if (m_mpThreadHandle.find(nThreadSequence) != m_mpThreadHandle.end())
			{
				Sleep(60);
				if (m_mpThreadContext.find(nThreadSequence) != m_mpThreadContext.begin())
				{
					m_itor_ThreadContext = m_mpThreadContext.find(nThreadSequence);
					m_CurContext = m_itor_ThreadContext->second;
				}

				m_itor_Handle = m_mpThreadHandle.find(nThreadSequence);
				::SuspendThread(m_itor_Handle->second);
				m_mpThreadStatus[nThreadSequence] = EThreadStatus_Suspend;
			}
		}

		bReturn = true;
	} while (false);

	return bReturn;
}

bool CHalerThread::Resume(long nThreadSequence, bool bResumeAll)
{
	bool bReturn = false;

	do 
	{
		std::map<int, ThreadContext> ::iterator m_itor_ThreadContext;
		std::map<int, HANDLE> ::iterator m_itor_Handle;

		if (bResumeAll)
		{
			for (m_itor_Handle = m_mpThreadHandle.begin(); m_itor_Handle != m_mpThreadHandle.end(); ++m_itor_Handle)
			{
				Sleep(60);
				if (m_mpThreadContext.find(m_itor_Handle->first) != m_mpThreadContext.end())
				{
					m_itor_ThreadContext = m_mpThreadContext.find(m_itor_Handle->first);
					m_CurContext = m_itor_ThreadContext->second;
				}

				::ResumeThread(m_itor_Handle->second);
				m_mpThreadStatus[m_itor_Handle->first] = EThreadStatus_Run;
			}
		}
		else
		{
			if (m_mpThreadHandle.find(nThreadSequence) != m_mpThreadHandle.end())
			{
				Sleep(60);
				if (m_mpThreadContext.find(nThreadSequence) != m_mpThreadContext.end())
				{
					m_itor_ThreadContext = m_mpThreadContext.find(nThreadSequence);
					m_CurContext = m_itor_ThreadContext->second;
				}

				m_itor_Handle = m_mpThreadHandle.find(nThreadSequence);
				::ResumeThread(m_itor_Handle->second);
				m_mpThreadStatus[nThreadSequence] = EThreadStatus_Run;
			}
		}

		bReturn = true;
	} while (false);
		
	return bReturn;
}

bool CHalerThread::Terminate(long nThreadSequence, bool bTerminateAll)
{
	bool bReturn = false;

	do
	{
		std::map<int, ThreadContext> ::iterator m_itor_ThreadContext;
		std::map<int, HANDLE> ::iterator m_itor_Handle;

		if (bTerminateAll)
		{
			for (m_itor_Handle = m_mpThreadHandle.begin(); m_itor_Handle != m_mpThreadHandle.end(); ++m_itor_Handle)
			{
				Sleep(60);
				if (m_mpThreadContext.find(m_itor_Handle->first) != m_mpThreadContext.end())
				{
					m_itor_ThreadContext = m_mpThreadContext.find(m_itor_Handle->first);
					m_CurContext = m_itor_ThreadContext->second;
				}

				::TerminateThread(m_itor_Handle->second,0);
				m_mpThreadStatus[m_itor_Handle->first] = EThreadStatus_Terminate;
			}
		}
		else
		{
			if (m_mpThreadHandle.find(nThreadSequence) != m_mpThreadHandle.end())
			{
				Sleep(60);
				if (m_mpThreadContext.find(nThreadSequence) != m_mpThreadContext.end())
				{
					m_itor_ThreadContext = m_mpThreadContext.find(nThreadSequence);
					m_CurContext = m_itor_ThreadContext->second;
				}

				m_itor_Handle = m_mpThreadHandle.find(nThreadSequence);
				::TerminateThread(m_itor_Handle->second,0);
				m_mpThreadStatus[nThreadSequence] = EThreadStatus_Terminate;
			}
		}

		bReturn = true;
	} while (false);

	return bReturn;
}

bool CHalerThread::Wait(long nThreadSequence, DWORD dwWaitTimeMS, bool bTerminateWhenTimeout)
{
	bool bReturn = true;

	do 
	{
		DWORD m_BeforeTime;
		DWORD m_AfterTime;
		DWORD m_TestTime;
		unsigned long long m_nTestTime;

		m_BeforeTime = GetTickCount();

		std::map<int, EThreadStatus>::iterator m_itor_ThreadStatus;
		std::map<int, HANDLE>::iterator m_itorr_ThreadHandle;

		while (true)
		{
			m_AfterTime = GetTickCount();
			
			m_TestTime = m_AfterTime - m_BeforeTime;
			m_nTestTime = m_TestTime;

			if (EThreadStatus_Run == GetThreadStatus(nThreadSequence))
			{
				break;
			}

			if (m_nTestTime > dwWaitTimeMS)
			{
				if (bTerminateWhenTimeout)
				{
					if (EThreadStatus_Unknown != GetThreadStatus(nThreadSequence) && EThreadStatus_Terminate != GetThreadStatus(nThreadSequence))
					{
						m_itorr_ThreadHandle = m_mpThreadHandle.find(nThreadSequence);
						::TerminateThread(m_itorr_ThreadHandle->second, 0);
						m_mpThreadStatus[nThreadSequence] = EThreadStatus_Terminate;
					}
				}
				else
				{
					bReturn = false;
					break;
				}
			}
			else
			{
				continue;
			}
		}
	} while (false);

	return bReturn;
}

unsigned __stdcall CHalerThread::HalerThreadFunction(void * pParam)
{
	if (m_CurContext)
	{
		m_CurContext();//启动可调用对象-->lambda
	}

	return 0;
}
