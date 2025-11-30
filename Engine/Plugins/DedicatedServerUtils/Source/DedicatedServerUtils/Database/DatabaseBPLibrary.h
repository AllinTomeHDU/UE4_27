#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DatabaseBPLibrary.generated.h"


UCLASS()
class DEDICATEDSERVERUTILS_API UDatabaseBPLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|Database")
	static void TestMySQL_1();

	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|Database")
	static void TestMySQL_2();

};