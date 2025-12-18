#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "../Definition/ServerThreadEvent.h"
#include "../Interface/ServerThreadProxyInterface.h"


/**
* 线程管理类，封装一套线程的 创建、暂停、唤醒、同步 的机制
*/
class DS_THREADPOOL_API FServerThreadRunnable 
	: public FRunnable, public IServerThreadProxy
{
public:
	FServerThreadRunnable(bool InSuspendAtFirst = false);
	virtual ~FServerThreadRunnable();

	virtual void CreateSafeThread() override;
	virtual void WakeupThread() override;
	virtual bool IsSuspend() override;
	virtual void BlockingAndCompletion() override;
	virtual void WaitAndCompleted() override;

private:
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	virtual void Exit();

private:
	FThreadSafeCounter		StopTaskCounter;  // 线程安全的停止标志，>0 表示该退出
	bool					bSuspendAtFirst;  // 是否线程启动后立即进入挂起状态
	class FRunnableThread*	Thread;			  // 线程对象
	FName					RunnableName;	  // 线程名字（调试/日志）
	FServerThreadEvent		ThreadEvent;      // 控制线程的暂停与唤醒 
	FServerThreadEvent		StartUpEvent;	  // 用于等待线程启动完成 
	FServerThreadEvent		WaitExecuteEvent; // 用于等待同步任务执行完成
	FCriticalSection		Mutex;			  // 临界区锁
	static int32			ThreadCount;	  // 静态计数器，用于生成线程唯一ID
};


