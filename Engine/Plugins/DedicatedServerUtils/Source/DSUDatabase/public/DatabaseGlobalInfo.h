#pragma once

#include "CoreMinimal.h"
#include "MySQL/Link/MySQL_LinkType.h"


class FMySQLGlobalInfo
{
	static FMySQLGlobalInfo* Instance;
public:
	static FMySQLGlobalInfo* Get();
	static void Destroy();

	void Init(const FString& InPath = FPaths::ProjectDir() / TEXT("MySQLConfig.ini"));

private:
	FMySQLConnectConfig ConfigInfo;

public:
	FORCEINLINE const FMySQLConnectConfig& GetInfo() const { return ConfigInfo; }
};

