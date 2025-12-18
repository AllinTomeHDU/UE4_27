#pragma once

#include "CoreMinimal.h"
#include "DS_NetChannel/UObject/NetChannelController.h"
#include "CenterController.generated.h"


UCLASS()
class UCenterController : public UNetChannelController
{
	GENERATED_BODY()

public:


protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;
	virtual void RecvProtocol(uint32 InProtocol) override;

private:

};