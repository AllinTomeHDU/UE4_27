#pragma once

#include "CoreMinimal.h"
#include "Core/NetChannelType.h"


class DSUNETCHANNEL_API FNetChannelGlobalInfo
{
	static FNetChannelGlobalInfo* Instance;
public:
	static FNetChannelGlobalInfo* Get();
	static void Destroy();

	void Init(const FString& InPath = FPaths::ProjectDir()/TEXT("NetChannelConfig.ini"));
	
private:
	FNetConfigInfo ConfigInfo;

public:
	FORCEINLINE const FNetConfigInfo& GetInfo() const { return ConfigInfo; }
};
