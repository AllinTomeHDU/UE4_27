#pragma once

#include "CoreMinimal.h"
#include "../Definition/ServerThreadType.h"


struct DS_THREADPOOL_API FServerCoroutinesRequest
{
	FServerCoroutinesRequest(float InIntervalTime) 
		: bCompleteRequest(false), IntervalTime(InIntervalTime)
	{}

	bool bCompleteRequest;
	float IntervalTime;
};


class DS_THREADPOOL_API IServerCoroutinesObject : public TSharedFromThis<IServerCoroutinesObject>
{
	friend class ICoroutinesContainer;

public:
	IServerCoroutinesObject() : bAwaken(false) {}
	virtual ~IServerCoroutinesObject() {}

	bool operator==(const IServerCoroutinesObject& ThreadHandle)
	{
		return this->Handle == ThreadHandle.Handle;
	}

	FORCEINLINE void Awaken() { bAwaken = true; }	//Wakeup process 

protected:
	virtual void Update(FServerCoroutinesRequest& CoroutinesRequest) = 0;

protected:
	uint8 bAwaken : 1;
	FServerThreadHandle Handle;
	static TArray<TSharedPtr<IServerCoroutinesObject>> CoroutinesArr;
};

// Process handle 
typedef TWeakPtr<IServerCoroutinesObject> FCoroutinesHandle;


/**
* 服务端协程执行单元，模拟“延迟执行”或“按任务执行”的逻辑，即 UE版本 的服务端时间驱动的协程任务：
*	- 保存执行委托（SimpleDelegate/OnGoingDelegate）；
*	- 根据总时长和时间间隔来推进执行进度；
*	- 当进度到达终点时，触发完成回调。
*/
class DS_THREADPOOL_API FServerCoroutinesObject : public IServerCoroutinesObject
{
public:
	// 即时执行模式：只要被唤醒就立即执行
	FServerCoroutinesObject(const FSimpleDelegate& InDelegate);

	// 定时执行模式：被唤醒后积累运行时间，当运行时间大于总等待时间之后执行
	FServerCoroutinesObject(float InTotalTime, const FSimpleDelegate& InDelegate);

	// 持续执行模式：处理持续过程中的时间回调
	FServerCoroutinesObject(const FServerOnGoingDelegate& InDelegate);

	// 持续+定时执行模式：同时具备总时长控制和进度反馈的功能
	FServerCoroutinesObject(float InTotalTime, const FServerOnGoingDelegate& InDelegate);

	// 注册之后每帧更新
	virtual void Update(FServerCoroutinesRequest& CoroutinesRequest) final;

private:
	FSimpleDelegate SimpleDelegate;
	FServerOnGoingDelegate OnGoingDelegate;
	const float TotalTime;
	float RuningTime;
};

