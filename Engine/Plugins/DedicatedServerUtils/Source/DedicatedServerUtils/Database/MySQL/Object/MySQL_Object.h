#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Link/MySQL_LinkType.h"
#include "MySQL_Object.generated.h"

struct FMySQL_Link;


UCLASS(Blueprintable, BlueprintType)
class DEDICATEDSERVERUTILS_API UMySQL_Object : public UObject
{
	GENERATED_BODY()

public:
	UMySQL_Object() {};

	bool CreateLink(const FMySQLConnectConfig& Config);

	FORCEINLINE TSharedPtr<FMySQL_Link> GetLink() { return Link.ToSharedRef(); }

private:
	TSharedPtr<FMySQL_Link> Link;
};