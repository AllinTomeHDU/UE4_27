#include "ServerThreadManager.h"
using namespace DSUThreadPool;


TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> DSUThreadPool::FThreadManagement::Get()
{
	if (!ThreadManagement.IsValid())
	{
		ThreadManagement = MakeShareable(new FThreadManagement());
	}
	return ThreadManagement.ToSharedRef();
}

void DSUThreadPool::FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid())
	{
		ThreadManagement.Reset();
	}
}

void DSUThreadPool::FThreadManagement::Tick(float DeltaTime)
{
	CoroutinesManager.Tick(DeltaTime);	// Coordination task
	ThreadTaskManager.Tick(DeltaTime);	// Thread task
}




