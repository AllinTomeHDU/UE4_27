#pragma once

#include "CoreMinimal.h"
#include "Base/NetChannelObject.h"
#include "NetChannelPlayer.generated.h"


UCLASS(Blueprintable)
class DSUNETCHANNEL_API UNetChannelPlayer : public UNetChannelObject
{
	GENERATED_BODY()

public:


protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;
	virtual void RecvProtocol(uint32 InProtocol) override;
};