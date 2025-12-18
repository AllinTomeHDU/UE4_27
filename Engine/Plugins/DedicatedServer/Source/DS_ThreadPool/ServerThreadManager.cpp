#include "ServerThreadManager.h"
using namespace DS_ThreadPool;


TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> DS_ThreadPool::FThreadManagement::Get()
{
	if (!ThreadManagement.IsValid())
	{
		ThreadManagement = MakeShareable(new FThreadManagement());
	}
	return ThreadManagement.ToSharedRef();
}

void DS_ThreadPool::FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid())
	{
		ThreadManagement.Reset();
	}
}

void DS_ThreadPool::FThreadManagement::Tick(float DeltaTime)
{
	CoroutinesManager.Tick(DeltaTime);	// Coordination task
	ThreadTaskManager.Tick(DeltaTime);	// Thread task
}




