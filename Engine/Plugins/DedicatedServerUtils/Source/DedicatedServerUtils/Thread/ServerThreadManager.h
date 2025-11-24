#pragma once

#include "CoreMinimal.h"
#include "Manager/CoroutinesManager.h"
#include "Manager/ThreadAbandonbleManager.h"
#include "Manager/ThreadGraphManager.h"
#include "Manager/ThreadProxyManager.h"
#include "Manager/ThreadTaskManager.h"
#include "Definition/ServerThreadPlatform.h"
#include "Definition/ServerThreadType.h"
#include "Definition/ServerThreadMacro.h"


namespace DedicatedServerUtils
{

/**
* 全局线程管理系统（单例模式）
*/
class DEDICATEDSERVERUTILS_API FThreadManagement : public TSharedFromThis<FThreadManagement>
{
public:
	virtual ~FThreadManagement() {}

	static TSharedRef<FThreadManagement> Get();

public:
	static FCoroutinesManager&		  GetCoroutines()  { return Get()->CoroutinesManager; }
	static FThreadProxyManager&		  GetProxy()	   { return Get()->ThreadProxyManager; }
	static FThreadTaskManager&		  GetTask()		   { return Get()->ThreadTaskManager; }
	static FThreadGraphManager&		  GetGraph()	   { return Get()->ThreadGraphManager; }
	static FThreadAbandonableManager& GetAbandonable() { return Get()->ThreadAbandonableManager; }

	static void Destroy();

	virtual void Tick(float DeltaTime);

	// 提供多线程的性能统计接口，满足 UE 的 Tickable/Profile 约定
	virtual TStatId GetStatId() const { return TStatId(); }

protected:
	FCoroutinesManager		  CoroutinesManager;
	FThreadProxyManager		  ThreadProxyManager;
	FThreadTaskManager		  ThreadTaskManager;
	FThreadGraphManager		  ThreadGraphManager;
	FThreadAbandonableManager ThreadAbandonableManager;

private:
	static TSharedPtr<FThreadManagement> ThreadManagement;
};

}	// namespace end

