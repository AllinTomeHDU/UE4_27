#pragma once

#include "CoreMinimal.h"


struct DEDICATEDSERVERUTILS_API FServerCoroutinesRequest
{
	FServerCoroutinesRequest(float InIntervalTime)
		: bCompleteRequest(false), IntervalTime(InIntervalTime)
	{}

	bool bCompleteRequest;
	float IntervalTime;
};


class DEDICATEDSERVERUTILS_API IServerCoroutinesObject : public TSharedFromThis<IServerCoroutinesObject>
{
	friend class ICoroutinesContainer;

public:
	IServerCoroutinesObject() : bAwaken(false) {}
	virtual ~IServerCoroutinesObject() {}

	bool operator==(const IServerCoroutinesObject& ThreadHandle)
	{
		return this->Handle == ThreadHandle.Handle;
	}

	FORCEINLINE void Awaken() { bAwaken = true; }	//Wakeup process 

protected:
	virtual void Update(FServerCoroutinesRequest& CoroutinesRequest) = 0;

	static TArray<TSharedPtr<IServerCoroutinesObject>>& GetArray()
	{
		static TArray<TSharedPtr<IServerCoroutinesObject>> Instance;
		return Instance;
	}

protected:
	uint8 bAwaken : 1;
	FServerThreadHandle Handle;
};

// Process handle 
typedef TWeakPtr<IServerCoroutinesObject> FCoroutinesHandle;