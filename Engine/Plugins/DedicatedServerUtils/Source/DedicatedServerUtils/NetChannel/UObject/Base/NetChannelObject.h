#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NetChannelObject.generated.h"


UCLASS(Blueprintable)
class UNetChannelObject : public UObject
{
	GENERATED_BODY()

public:
	

protected:
	virtual void Init();

	virtual void Tick(float DeltaTime);

	virtual void RecvProtocol(uint8 InProtocol);

};