#pragma once

#include "CoreMinimal.h"
#include "Definition/NetChannelType.h"


class DS_NETCHANNEL_API FNetChannelGlobalInfo
{
	static FNetChannelGlobalInfo* Instance;
public:
	static FNetChannelGlobalInfo* Get();
	static void Destroy();

	/*
	* 通过文件配表方便开发调试，但是将配置文件放在游戏客户端有风险
	* 建议游戏客户端直接直接通过代码程序硬编码写入配置
	*/
	void Init(const FString& InPath = FPaths::ProjectDir()/TEXT("Config/NetChannelConfig.ini"));
	
private:
	FNetConfigInfo ConfigInfo;

public:
	FORCEINLINE void SetConfigInfo(const FNetConfigInfo& InConfigInfo) { ConfigInfo = InConfigInfo; }
	FORCEINLINE const FNetConfigInfo& GetInfo() const { return ConfigInfo; }
};
