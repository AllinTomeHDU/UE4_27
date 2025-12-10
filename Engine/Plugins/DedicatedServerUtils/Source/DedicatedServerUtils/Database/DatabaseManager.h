#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MySQL/Link/MySQL_LinkType.h"
#include "DatabaseManager.generated.h"

class UMySQL_Object;


UCLASS(meta = (BlueprintThreadSafe, ScriptName = "DatabaseManager"))
class DEDICATEDSERVERUTILS_API UDatabaseManager : public UObject
{
	GENERATED_BODY()

public:
	static UMySQL_Object* CreateMySQL_Object(UObject* WorldContextObject, const FMySQLConnectConfig& Config);


};