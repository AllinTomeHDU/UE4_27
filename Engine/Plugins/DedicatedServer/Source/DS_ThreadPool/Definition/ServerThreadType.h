#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"


DECLARE_DELEGATE_TwoParams(FServerOnGoingDelegate, float, float);
typedef TFunction<void()> FServerThreadLambda;


enum class EServerThreadState :uint8
{
	Leisurely,	// 空闲中
	Working,	// 正在执行任务
	Error		// 无效句柄，异常
};


namespace DedicatedServerUtils
{
	static uint64 SpawnUniqueID()
	{
		return FMath::Abs(FMath::RandRange(100, 1256343) + FDateTime::Now().GetTicks());
	}
}
using namespace DedicatedServerUtils;


struct DS_THREADPOOL_API FServerThreadHandle : public TSharedFromThis<FServerThreadHandle>
{
	FServerThreadHandle() 
		: GUIDTypeA(SpawnUniqueID())
		, GUIDTypeB(SpawnUniqueID())
		, GUIDTypeC(SpawnUniqueID())
		, GUIDTypeD(SpawnUniqueID()) 
	{}

	bool operator==(const FServerThreadHandle& Other)
	{
		return GUIDTypeA == Other.GUIDTypeA 
			&& GUIDTypeB == Other.GUIDTypeB 
			&& GUIDTypeC == Other.GUIDTypeC 
			&& GUIDTypeD == Other.GUIDTypeD;
	}

protected:
	uint64 GUIDTypeA;
	uint64 GUIDTypeB;
	uint64 GUIDTypeC;
	uint64 GUIDTypeD;
	
};


struct DS_THREADPOOL_API FServerThreadAbandonable : FNonAbandonableTask
{
	FServerThreadAbandonable(const FSimpleDelegate& InThreadDelegate)
		: ThreadDelegate(InThreadDelegate)
	{}

	// Where threads really execute logic 
	void DoWork()
	{
		ThreadDelegate.ExecuteIfBound();
	}

	// ID
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FServerThreadAbandonable, STATGROUP_ThreadPoolAsyncTasks);
	}

protected:
	FSimpleDelegate ThreadDelegate;
};


