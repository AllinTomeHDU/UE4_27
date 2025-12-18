#pragma once

#include "CoreMinimal.h"
#include "Base/ServerThreadTemplate.h"
#include "../Interface/ServerThreadManagerInterface.h"


/**
* 线程代理系统上层管理类，统一的线程调度器
*/
class DS_THREADPOOL_API FThreadProxyManager
	: public FServerThreadTemplate<IThreadProxyContainer, TWeakPtr<FServerThreadHandle>>
{
public:
	virtual ~FThreadProxyManager()
	{
		for (auto& Tmp : *this)
		{
			Tmp->WaitAndCompleted();
		}
	}

	// 同步等待接口
	bool Join(TWeakPtr<FServerThreadHandle> Handle)
	{
		TSharedPtr<IServerThreadProxy> ThreadProxy = *this >> Handle;
		if (ThreadProxy.IsValid())
		{
			ThreadProxy->BlockingAndCompletion();
			return true;
		}
		return false;
	}

	// 异步调用接口
	bool Detach(TWeakPtr<FServerThreadHandle> Handle)
	{
		TSharedPtr<IServerThreadProxy> ThreadProxy = *this >> Handle;
		if (ThreadProxy.IsValid())
		{
			ThreadProxy->WakeupThread();
			return true;
		}
		return false;
	}

	// 查询指定句柄对应的线程状态
	EServerThreadState Joinable(TWeakPtr<FServerThreadHandle> Handle)
	{
		TSharedPtr<IServerThreadProxy> ThreadProxy = *this >> Handle;
		if (ThreadProxy.IsValid())
		{
			return ThreadProxy->IsSuspend() ? EServerThreadState::Leisurely
											: EServerThreadState::Working;
		}
		return EServerThreadState::Error;
	}
};