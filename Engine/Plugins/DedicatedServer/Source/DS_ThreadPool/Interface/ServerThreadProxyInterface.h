#pragma once

#include "CoreMinimal.h"
#include "../Definition/ServerThreadType.h"


class DS_THREADPOOL_API IServerThreadProxy : public TSharedFromThis<IServerThreadProxy>
{
public:
	IServerThreadProxy() : ThreadHandle(MakeShareable(new FServerThreadHandle)) {}

	virtual ~IServerThreadProxy() {};

	// 创建线程对象
	virtual void CreateSafeThread() = 0;

	// 唤醒线程，通知线程执行任务
	virtual void WakeupThread() = 0;

	// 查询线程是否处于挂起状态
	virtual bool IsSuspend() = 0;

	// 唤醒线程执行任务，并阻塞等待任务执行结束
	virtual void BlockingAndCompletion() = 0;

	// 发送停止信号，并等待线程安全退出
	virtual void WaitAndCompleted() = 0;

	FORCEINLINE FSimpleDelegate& GetThreadDelegate() { return ThreadDelegate; }
	FORCEINLINE TWeakPtr<FServerThreadHandle> GetThreadHandle() { return ThreadHandle; }

protected:
	FSimpleDelegate ThreadDelegate;

private:
	TSharedPtr<FServerThreadHandle> ThreadHandle;
};