#include "DatabaseManager.h"
#include "MySQL/Object/MySQL_Object.h"


UMySQL_Object* UDatabaseManager::CreateMySQL_Object(UObject* WorldContextObject, const FMySQLConnectConfig& Config)
{
	UMySQL_Object* MySQL = nullptr;

	auto CallMainThreadWork = [&]()->UMySQL_Object*
		{
#ifdef PLATFORM_PROJECT
			if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
			{
				if (UMySQL_Object* Obj = NewObject<UMySQL_Object>(World))
				{
					Obj->CreateLink(Config);
					return Obj;
				}
				return nullptr;
			}
#else
			UMySQL_Object* Obj = NewObject<UMySQL_Object>();
			if (Obj)
			{
				Obj->CreateLink(Config);
				Obj->AddToRoot();
			}
			return Obj;
#endif
		};

	if (FPlatformTLS::GetCurrentThreadId() == GGameThreadId)
	{
		MySQL = CallMainThreadWork();
	}
	else
	{
		// 呼叫主线程
		FGraphEventRef EventRef = FFunctionGraphTask::CreateAndDispatchWhenReady(
			[&]()
			{
				MySQL = CallMainThreadWork();
			},
			TStatId(),
			NULL,
			ENamedThreads::GameThread
		);

		FTaskGraphInterface::Get().WaitUntilTaskCompletes(EventRef);
	}

	return MySQL;
}
