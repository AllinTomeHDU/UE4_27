#include "ServerThreadRunnableProxy.h"


int32 FServerThreadRunnable::ThreadCount = 0;

FServerThreadRunnable::FServerThreadRunnable(bool bInSuspendAtFirst)
	: IServerThreadProxy()
	, StopTaskCounter(0)
	, bSuspendAtFirst(bInSuspendAtFirst)
	, Thread(nullptr)
{
}

FServerThreadRunnable::~FServerThreadRunnable()
{
	if (Thread != nullptr)
	{
		delete Thread;
		Thread = nullptr;
	}
}

void FServerThreadRunnable::CreateSafeThread()
{
	RunnableName = *FString::Printf(TEXT("ServerThread-%i"), ThreadCount);
	Thread = FRunnableThread::Create(this, *RunnableName.ToString(), 0, TPri_BelowNormal);
	ThreadCount++;
}

void FServerThreadRunnable::WakeupThread()
{
	ThreadEvent.Trigger();
}

bool FServerThreadRunnable::IsSuspend()
{
	return ThreadEvent.IsWait();
}

void FServerThreadRunnable::BlockingAndCompletion()
{
	ThreadEvent.Trigger();
	WaitExecuteEvent.Wait();
}

void FServerThreadRunnable::WaitAndCompleted()
{
	Stop();
	if (ThreadEvent.IsWait())
	{
		ThreadEvent.Trigger();
		StartUpEvent.Wait();
		FPlatformProcess::Sleep(0.03f);
	}
}

bool FServerThreadRunnable::Init()
{
	if (!FPlatformProcess::SupportsMultithreading())
	{
		Stop();
	}
	return StopTaskCounter.GetValue() == 0;
}

uint32 FServerThreadRunnable::Run()
{
	while (StopTaskCounter.GetValue() == 0)
	{
		if (!bSuspendAtFirst)
		{
			ThreadEvent.Wait(); // 等待唤醒
		}

		if (ThreadDelegate.IsBound())
		{
			ThreadDelegate.Execute();	// 业务逻辑
			ThreadDelegate.Unbind();	// 执行一次清除
		}

		WaitExecuteEvent.Trigger();	// 通知任务逻辑
		bSuspendAtFirst = false;	// 重置挂起标志
	}
	return 0;
}

void FServerThreadRunnable::Stop()
{
	StopTaskCounter.Increment();
}

void FServerThreadRunnable::Exit()
{
	StartUpEvent.Trigger();
}
