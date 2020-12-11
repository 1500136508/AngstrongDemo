#pragma once

#include <functional>
#include <map>
#include <mutex>
#include <windows.h>
#include "HalerThreadDefinition.h"

#define CreateHalerThreadContext(pContext) ThreadContext pContext; pContext = [=](void) -> void
#define CreateHalerThreadContextCaptureRef(pContext) ThreadContext pContext; pContext = [&](void) -> void

typedef std::function<void(void)> ThreadContext;

class CHalerThread
{
private:
	CHalerThread();
public:
	virtual ~CHalerThread();

	static CHalerThread* GetInstance();
	static CHalerThread GetInstatnce();

	static EThreadStatus GetThreadStatus(_In_ long nThreadSequence);
	static long GetThreadCount();

	static bool Run(_In_ ThreadContext pContext, _In_ long nThreadSequenc, _In_opt_ bool bCreateSuspended = false);

	static bool Suspend(_In_ long nThreadSequenc, _In_opt_ bool bSuspendAll = false);
	static bool Resume(_In_ long nThreadSequence, _In_opt_ bool bResumeAll = false);
	static bool Terminate(_In_ long nThreadSequence, _In_opt_ bool bTerminateAll = false);

	static bool Wait(_In_ long nThreadSequence, _In_ DWORD dwWaitTimeMS, _In_opt_ bool bTerminateWhenTimeout = true);

private:
	static unsigned __stdcall HalerThreadFunction(void*);

	static std::mutex thread_mutex;
	static ThreadContext m_CurContext;
	static std::map<int, unsigned int> m_mpThreadID;
	static std::map<int, EThreadStatus> m_mpThreadStatus;
	static std::map<int, ThreadContext> m_mpThreadContext;
	static std::map<int, HANDLE> m_mpThreadHandle;
};

