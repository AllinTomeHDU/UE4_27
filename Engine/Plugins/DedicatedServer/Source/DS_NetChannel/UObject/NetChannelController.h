#pragma once

#include "CoreMinimal.h"
#include "Base/NetChannelObject.h"
#include "NetChannelController.generated.h"


UCLASS(Blueprintable)
class DS_NETCHANNEL_API UNetChannelController : public UNetChannelObject
{
	GENERATED_BODY()

public:


protected:
	virtual void Init() override;

	virtual void Tick(float DeltaTime) override;

	virtual void RecvProtocol(uint32 InProtocol) override;
};