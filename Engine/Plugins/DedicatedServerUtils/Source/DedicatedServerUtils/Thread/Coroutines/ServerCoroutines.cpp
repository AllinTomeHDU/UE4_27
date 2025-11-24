#include "ServerCoroutines.h"


FServerCoroutinesObject::FServerCoroutinesObject(const FSimpleDelegate& InDelegate)
	: IServerCoroutinesObject(), SimpleDelegate(InDelegate), TotalTime(INDEX_NONE), RuningTime(0.f)
{
}

FServerCoroutinesObject::FServerCoroutinesObject(float InTotalTime, const FSimpleDelegate& InDelegate)
	: IServerCoroutinesObject(), SimpleDelegate(InDelegate), TotalTime(InTotalTime), RuningTime(0.f)
{
}

FServerCoroutinesObject::FServerCoroutinesObject(const FServerOnGoingDelegate& InDelegate)
	: IServerCoroutinesObject(), OnGoingDelegate(InDelegate), TotalTime(INDEX_NONE), RuningTime(0.f)
{
}

FServerCoroutinesObject::FServerCoroutinesObject(float InTotalTime, const FServerOnGoingDelegate& InDelegate)
	: IServerCoroutinesObject(), OnGoingDelegate(InDelegate), TotalTime(InTotalTime), RuningTime(0.f)
{
}

void FServerCoroutinesObject::Update(FServerCoroutinesRequest& CoroutinesRequest)
{
	if (TotalTime != INDEX_NONE)
	{
		RuningTime += CoroutinesRequest.IntervalTime;

		// 计算OnGoing时间
		if (OnGoingDelegate.IsBound())
		{
			if (TotalTime != 0.f)
			{
				float Ratio = RuningTime / TotalTime;
				OnGoingDelegate.ExecuteIfBound(Ratio, CoroutinesRequest.IntervalTime);
			}
		}

		if (RuningTime >= TotalTime)
		{
			SimpleDelegate.ExecuteIfBound();
			CoroutinesRequest.bCompleteRequest = true;
		}
	}
	else
	{
		if (bAwaken)
		{
			SimpleDelegate.ExecuteIfBound();
			CoroutinesRequest.bCompleteRequest = true;
		}
	}
}

