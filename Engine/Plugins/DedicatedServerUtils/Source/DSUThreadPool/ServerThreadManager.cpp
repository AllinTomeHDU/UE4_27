#include "ServerThreadManager.h"
using namespace USDThreadPool;


TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> USDThreadPool::FThreadManagement::Get()
{
	if (!ThreadManagement.IsValid())
	{
		ThreadManagement = MakeShareable(new FThreadManagement());
	}
	return ThreadManagement.ToSharedRef();
}

void USDThreadPool::FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid())
	{
		ThreadManagement.Reset();
	}
}

void USDThreadPool::FThreadManagement::Tick(float DeltaTime)
{
	CoroutinesManager.Tick(DeltaTime);	// Coordination task
	ThreadTaskManager.Tick(DeltaTime);	// Thread task
}




