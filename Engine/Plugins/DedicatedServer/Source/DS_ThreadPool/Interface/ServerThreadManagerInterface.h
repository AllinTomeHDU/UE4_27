#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "ServerThreadProxyInterface.h"
#include "../Definition/ServerThreadMacro.h"
#include "../Definition/ServerThreadType.h"
#include "../Runnable/ServerThreadRunnableProxy.h"
#include "../Coroutines/ServerCoroutines.h"

#ifdef PLATFORM_PROJECT
#include "Engine/StreamableManager.h"
#endif

/**
* 协程池容器：负责所有协程对象的生命周期管理、时间推进与任务创建。
*/
#pragma region Coroutines Container
class ICoroutinesContainer
{
public:
	ICoroutinesContainer() : TmpTotalTime(0.f) 
	{}

	virtual ~ICoroutinesContainer()
	{
		IServerCoroutinesObject::CoroutinesArr.Empty();
	}

	ICoroutinesContainer& operator<<(float TotalTime)
	{
		TmpTotalTime = TotalTime;
		return *this;
	}

	ICoroutinesContainer& operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		IServerCoroutinesObject::CoroutinesArr.Add(MakeShareable(new FServerCoroutinesObject(TmpTotalTime, ThreadDelegate)));
		return *this;
	}

	ICoroutinesContainer& operator<<(const FServerOnGoingDelegate& ThreadDelegate)
	{
		IServerCoroutinesObject::CoroutinesArr.Add(MakeShareable(new FServerCoroutinesObject(TmpTotalTime, ThreadDelegate)));
		return *this;
	}

	// 协程系统驱动器：每帧调用，用于更新所有正在运行的协程
	void operator<<=(float DeltaTime)
	{
		TArray<TSharedPtr<IServerCoroutinesObject>> RemoveObject;
		for (int32 i = 0; i < IServerCoroutinesObject::CoroutinesArr.Num(); i++) //改成i的形式 是为了安全考虑
		{
			if (auto Tmp = IServerCoroutinesObject::CoroutinesArr[i])
			{
				FServerCoroutinesRequest Request(DeltaTime);
				Tmp->Update(Request);
				if (Request.bCompleteRequest)
				{
					RemoveObject.Add(Tmp);
				}
			}
		}
		for (auto& RemoveInstance : RemoveObject)
		{
			IServerCoroutinesObject::CoroutinesArr.Remove(RemoveInstance);
		}
	}

	// 创建一个立即触发的协程，并返回该协程的句柄
	FCoroutinesHandle operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		IServerCoroutinesObject::CoroutinesArr.Add(MakeShareable(new FServerCoroutinesObject(ThreadDelegate)));
		return IServerCoroutinesObject::CoroutinesArr[IServerCoroutinesObject::CoroutinesArr.Num() - 1];
	}

private:
	// 临时变量，用来在连续 << 操作中传递总时长参数，相当于一个“配置缓存”
	float TmpTotalTime;
};
#pragma endregion


/**
* 线程池容器基类：子类通过重载“<<”和“>>”实现任务的 添加/分配/执行/查询。
*/
#pragma region Thread Container
class IServerThreadContainer
{
public:
	IServerThreadContainer() {}
	virtual ~IServerThreadContainer() {}

protected:
	FCriticalSection Mutex;
};
#pragma endregion


/**
* 线程代理池容器: 管理所有线程代理实例，自动复用空闲线程，支持动态创建线程。
*/
#pragma region Thread Proxy Container
class IThreadProxyContainer
	: public TArray<TSharedPtr<IServerThreadProxy>>, public IServerThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IServerThreadProxy>> TProxyArray;

public:
	// 向容器内添加一个新的线程代理
	IThreadProxyContainer& operator<<(const TSharedPtr<IServerThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCK;
		ThreadProxy->CreateSafeThread();
		this->Add(ThreadProxy);
		return *this;
	}

	// 从线程池中分配一个空闲线程执行任务，若没找到空闲线程，则创建一个新的运行线程代理
	TWeakPtr<FServerThreadHandle> operator>>(const FSimpleDelegate& ThreadProxy)
	{
		TWeakPtr<FServerThreadHandle> ThreadHandle = nullptr;
		{
			MUTEX_LOCK;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend() && !Tmp->GetThreadDelegate().IsBound())
				{
					Tmp->GetThreadDelegate() = ThreadProxy;
					ThreadHandle = Tmp->GetThreadHandle();
					Tmp->WakeupThread();
					break;
				}
			}
		}
		if (!ThreadHandle.IsValid())
		{
			TSharedPtr<IServerThreadProxy> Proxy = MakeShareable(new FServerThreadRunnable(true));
			Proxy->GetThreadDelegate() = ThreadProxy;
			*this << Proxy;
			ThreadHandle = Proxy->GetThreadHandle();
		}
		return ThreadHandle;
	}

	// 分配一个“等待执行任务”的线程，不立即执行
	TWeakPtr<FServerThreadHandle> operator<<(const FSimpleDelegate& ThreadProxy)
	{
		TWeakPtr<FServerThreadHandle> ThreadHandle = nullptr;
		{
			MUTEX_LOCK;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend() && !Tmp->GetThreadDelegate().IsBound())
				{
					Tmp->GetThreadDelegate() = ThreadProxy;
					ThreadHandle = Tmp->GetThreadHandle();
					break;
				}
			}
		}
		if (!ThreadHandle.IsValid())
		{
			ThreadHandle = *this << MakeShareable(new FServerThreadRunnable) << ThreadProxy;
		}
		return ThreadHandle;
	}

	// 通过线程句柄获取线程对象（反查机制）
	TSharedPtr<IServerThreadProxy> operator>>(const TWeakPtr<FServerThreadHandle>& Handle)
	{
		MUTEX_LOCK;
		for (auto& Tmp : *this)
		{
			if (Tmp->GetThreadHandle() == Handle)
			{
				return Tmp;
			}
		}
		return nullptr;
	}
};
#pragma endregion


/**
* 任务队列线程池容器：存放任务队列，管理线程对象。
*/
#pragma region Thread Task Container
class IThreadTaskContainer
	: public TQueue<FSimpleDelegate>
	, public TArray<TSharedPtr<IServerThreadProxy>>
	, public IServerThreadContainer
{
protected:
	typedef TArray<TSharedPtr<IServerThreadProxy>> TProxyArray;
	typedef TQueue<FSimpleDelegate>	TEventQueue;

public:
	// 将一个委托推入任务队列中，不立即执行（异步任务推送逻辑）
	void operator<<(const FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCK;
		this->Enqueue(ThreadProxy);
	}

	// 从队列中取出一个任务，线程自己取任务（pull模式）
	bool operator<<=(FSimpleDelegate& ThreadProxy)
	{
		MUTEX_LOCK;
		return this->Dequeue(ThreadProxy);
	}

	// 向容器中添加一个线程代理对象，相当于“增添工人”
	IThreadTaskContainer& operator<<(const TSharedPtr<IServerThreadProxy>& ThreadProxy)
	{
		MUTEX_LOCK;
		this->Add(ThreadProxy);
		ThreadProxy->CreateSafeThread();
		return *this;
	}
	
	// 尝试把任务分配给空闲线程，若没有空闲线程则放入任务队列
	void operator>>(const FSimpleDelegate& ThreadProxy)
	{
		bool bSuccessful = false;
		{
			MUTEX_LOCK;
			for (auto& Tmp : *this)
			{
				if (Tmp->IsSuspend() && !Tmp->GetThreadDelegate().IsBound())
				{
					Tmp->GetThreadDelegate() = ThreadProxy;
					Tmp->WakeupThread();
					bSuccessful = true;
					break;
				}
			}
		}
		if (!bSuccessful)
		{
			*this << ThreadProxy;
		}
	}
};
#pragma endregion


/**
* 任务图容器：调度执行某一个任务到不同线程中
*/
#pragma region Thread Graph Container
class IThreadGraphContainer : public IServerThreadContainer
{
protected:
	// 把任务派发到游戏线程（主线程）
	FGraphEventRef operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		MUTEX_LOCK;
		return FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			ThreadDelegate, TStatId(), nullptr, ENamedThreads::GameThread);
	}

	// 把任务派发到任意线程（工作线程）
	FGraphEventRef operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		MUTEX_LOCK;
		return FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			ThreadDelegate, TStatId(), nullptr, ENamedThreads::AnyThread);
	}
};
#pragma endregion


/**
* 
*/
#pragma region Thread Abandonable Container
class IThreadAbandonableContainer : public IServerThreadContainer
{
	// 同步任务绑定
	void operator<<(const FSimpleDelegate& ThreadDelegate)
	{
		FAsyncTask<FServerThreadAbandonable>* Abandonable = new FAsyncTask<FServerThreadAbandonable>(ThreadDelegate);
		Abandonable->StartBackgroundTask();
		Abandonable->EnsureCompletion();
		delete Abandonable;
	}

	// 异步任务绑定
	void operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		(new FAutoDeleteAsyncTask<FServerThreadAbandonable>(ThreadDelegate))->StartBackgroundTask();
	}
};
#pragma endregion


/**
* 资源流式加载的抽象接口类
*/
#pragma region Streamable Container
#ifdef PLATFORM_PROJECT
class IStreamableContainer
{
public:
	virtual ~IStreamableContainer() {}

	// 设置资源路径
	IStreamableContainer& operator>>(const TArray<FSoftObjectPath>& InObjectPath)
	{
		SetObjectPath(InObjectPath);
		return *this;
	}

	// 异步加载
	TSharedPtr<struct FStreamableHandle> operator>>(const FSimpleDelegate& ThreadDelegate)
	{
		return GetStreamableManager()->RequestAsyncLoad(GetObjectPath(), ThreadDelegate);
	}

	// 同步加载
	TSharedPtr<struct FStreamableHandle> operator<<(const TArray<FSoftObjectPath>& InObjectPath)
	{
		return GetStreamableManager()->RequestSyncLoad(InObjectPath);
	}

protected:
	virtual void SetObjectPath(const TArray<FSoftObjectPath>& InObjectPath) = 0;
	virtual TArray<FSoftObjectPath>& GetObjectPath() = 0;

	virtual FStreamableManager* GetStreamableManager() = 0;
};
#endif
#pragma endregion


