#pragma once

#include "CoreMinimal.h"
#include "Base/ServerThreadTemplate.h"
#include "../Interface/ServerThreadManagerInterface.h"


struct DS_THREADPOOL_API FThreadTaskManager
	: public FServerThreadTemplate<IThreadTaskContainer>
{
	FThreadTaskManager()
	{
		Init(CPUThreadNumber);
	}

	virtual ~FThreadTaskManager()
	{
		for (auto& Tmp : *this)
		{
			Tmp->WaitAndCompleted();
		}
	}

	// 初始化线程池（设置容量），创建线程池工作线程
	void Init(int32 ThreadNum)
	{
		for (int32 i = 0; i < ThreadNum; i++)
		{
			*this << MakeShareable(new FServerThreadRunnable);
		}
		FPlatformProcess::Sleep(0.1f);
	}

	// 每次 Tick 只分配一个任务给空闲线程，避免锁竞争、CPU峰值
	void Tick(float DeltaTime)
	{
		TSharedPtr<IServerThreadProxy> ThreadProxy = nullptr;
		{
			MUTEX_LOCK;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend())
				{
					ThreadProxy = Tmp;
					break;
				}
			}
		}
		if (ThreadProxy.IsValid())
		{
			if (!((TEventQueue*)this)->IsEmpty())
			{
				FSimpleDelegate SimpleDelegate;
				if (*this <<= SimpleDelegate)
				{
					MUTEX_LOCK;
					ThreadProxy->GetThreadDelegate() = SimpleDelegate;
					ThreadProxy->WakeupThread();
				}
			}
		}
	}
};