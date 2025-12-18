#pragma once

#include "CoreMinimal.h"
#include "Base/ServerThreadTemplate.h"
#include "../Interface/ServerThreadManagerInterface.h"


struct DS_THREADPOOL_API FThreadGraphManager
	: FServerThreadTemplate<IThreadGraphContainer, FGraphEventRef>
{
	// 阻塞等待单个任务执行完毕
	static void Wait(const FGraphEventRef& EventRef)
	{
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(EventRef);
	}

	// 阻塞等待多个任务执行完毕
	static void Wait(const FGraphEventArray& EventRef)
	{
		FTaskGraphInterface::Get().WaitUntilTasksComplete(EventRef);
	}
};