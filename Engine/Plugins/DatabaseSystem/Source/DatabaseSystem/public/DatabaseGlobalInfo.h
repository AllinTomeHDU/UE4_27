#pragma once

#include "CoreMinimal.h"
#include "MySQL/Link/MySQL_LinkType.h"


class DATABASESYSTEM_API FMySQLGlobalInfo
{
	static FMySQLGlobalInfo* Instance;
public:
	static FMySQLGlobalInfo* Get();
	static void Destroy();

	void Init(const FString& InPath = FPaths::ProjectDir() / TEXT("Config/DatabaseConfig.ini"));

private:
	FMySQLConnectConfig MySQLConfig;

public:
	FORCEINLINE const FMySQLConnectConfig& GetMySQLInfo() const { return MySQLConfig; }
};

