#pragma once

#include "CoreMinimal.h"
#include "HAL/Event.h"
#include "ServerThreadPlatform.h"


// 对原生 FEvent 进一步封装，方便查询当前是否处于等待状态
struct FServerThreadEvent
{
	FServerThreadEvent() : Event(FPlatformProcess::GetSynchEventFromPool()), bWait(false)
	{}

	virtual ~FServerThreadEvent()
	{
		FPlatformProcess::ReturnSynchEventToPool(Event);
	}

	// 阻塞当前线程，直到被 Event->Trigger() 唤醒
	void Wait()
	{
		bWait = true;
		Event->Wait();
	}

	// 支持带超时的等待，若超时仍未唤醒则自动返回
	void Wait(uint32 WaitTime, const bool bIgnoreThreadIdleStats = false)
	{
		bWait = true;
		Event->Wait(WaitTime, bIgnoreThreadIdleStats);
	}

	// 唤醒线程
	void Trigger()
	{
		Event->Trigger();
		bWait = false;
	}

	FORCEINLINE bool IsWait() { return bWait; }
private:
	FEvent* Event;
	uint8 bWait : 1;
};