#include "ServerThreadManager.h"

TSharedPtr<FThreadManagement> FThreadManagement::ThreadManagement = nullptr;

TSharedRef<FThreadManagement> DedicatedServerUtils::FThreadManagement::Get()
{
	if (!ThreadManagement.IsValid())
	{
		ThreadManagement = MakeShareable(new FThreadManagement());
	}
	return ThreadManagement.ToSharedRef();
}

void DedicatedServerUtils::FThreadManagement::Destroy()
{
	if (ThreadManagement.IsValid())
	{
		ThreadManagement.Reset();
	}
}

void DedicatedServerUtils::FThreadManagement::Tick(float DeltaTime)
{
	CoroutinesManager.Tick(DeltaTime);	// Coordination task
	ThreadTaskManager.Tick(DeltaTime);	// Thread task
}




