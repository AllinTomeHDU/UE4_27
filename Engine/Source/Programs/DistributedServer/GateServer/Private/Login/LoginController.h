#pragma once

#include "CoreMinimal.h"
#include "DSUNetChannel/UObject/NetChannelController.h"
#include "LoginController.generated.h"


UCLASS()
class ULoginController : public UNetChannelController
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