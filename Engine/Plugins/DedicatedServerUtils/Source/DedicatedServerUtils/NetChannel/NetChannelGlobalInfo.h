#pragma once

#include "CoreMinimal.h"
#include "Core/NetChannelType.h"


class DEDICATEDSERVERUTILS_API FNetChannelGlobalInfo
{
	static FNetChannelGlobalInfo* Instance;
public:
	static FNetChannelGlobalInfo* Get();
	static void Destroy();

	void Init();
	
private:
	FNetConfigInfo ConfigInfo;

public:
	FORCEINLINE const FNetConfigInfo& GetInfo() const { return ConfigInfo; }
};
