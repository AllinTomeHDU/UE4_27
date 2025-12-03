#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DSUtilsBPLibrary.generated.h"


UCLASS()
class DEDICATEDSERVERUTILS_API UDSUtilsBPLibrary : public UObject
{
	GENERATED_BODY()

public:
	template<class T>
	static const FString GetBinaryString32Bit(const T& Value)
	{
		FString S;
		for (int i = 31; i >= 0; --i)
		{
			S += (((uint32)Value & (1UL << i)) ? TEXT("1") : TEXT("0"));
			S += (i % 4 == 0 ? TEXT(" ") : TEXT(""));
		}
		return S;
	}

	UFUNCTION(BlueprintPure, Category = "DedicatedServerUtil")
	static const FString GetBinaryString32Bit_Int32(const int32& Value);
};