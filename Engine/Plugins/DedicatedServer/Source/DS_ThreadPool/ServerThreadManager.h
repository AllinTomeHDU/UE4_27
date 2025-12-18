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

#ifdef PLATFORM_PROJECT
#include "Tickable.h"
#endif


namespace DS_ThreadPool
{

	/**
	* 全局线程管理系统（单例模式）
	*/
	class DS_THREADPOOL_API FThreadManagement 
		: public TSharedFromThis<FThreadManagement>
#ifdef PLATFORM_PROJECT
		, public FTickableGameObject
#endif
	{
	public:
		virtual ~FThreadManagement() {}

		static TSharedRef<FThreadManagement> Get();

#ifdef PLATFORM_PROJECT
		static FResourceLoadingManage& GetResourceLoading() { return Get()->ResourceLoadingManage; }
#endif

	public:
		static FCoroutinesManager&		  GetCoroutines()  { return Get()->CoroutinesManager; }
		static FThreadProxyManager&		  GetProxy()	   { return Get()->ThreadProxyManager; }
		static FThreadTaskManager&		  GetTask()		   { return Get()->ThreadTaskManager; }
		static FThreadGraphManager&		  GetGraph()	   { return Get()->ThreadGraphManager; }
		static FThreadAbandonableManager& GetAbandonable() { return Get()->ThreadAbandonableManager; }
#ifdef PLATFORM_PROJECT
		static FResourceLoadingManage& GetResourceLoading() { return Get()->ResourceLoadingManage; }
#endif

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
#ifdef PLATFORM_PROJECT
		FResourceLoadingManage ResourceLoadingManage;
#endif

	private:
		static TSharedPtr<FThreadManagement> ThreadManagement;
	};

}	// namespace end

