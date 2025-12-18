#pragma once

#include "CoreMinimal.h"
#include "../UObject/NetChannelController.h"
#include "TestController.generated.h"


UCLASS(Blueprintable)
class DS_NETCHANNEL_API UTestController : public UNetChannelController
{
	GENERATED_BODY()

public:


protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;
	virtual void RecvProtocol(uint32 InProtocol) override;
};