#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MySQL/Link/MySQL_LinkType.h"
#include "DatabaseManager.generated.h"

class UMySQL_Object;
struct FMySQL_Link;


UCLASS(meta = (BlueprintThreadSafe, ScriptName = "DatabaseManager"))
class DATABASESYSTEM_API UDatabaseManager : public UObject
{
	GENERATED_BODY()

public:
	static UMySQL_Object* CreateMySQL_Object(UObject* WorldContextObject, const FMySQLConnectConfig& Config);

	static TSharedPtr<FMySQL_Link> CreateMySQL_Link(const FMySQLConnectConfig& Config);
};