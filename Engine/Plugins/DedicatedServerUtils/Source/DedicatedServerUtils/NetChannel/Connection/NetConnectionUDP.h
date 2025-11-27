#pragma once

#include "CoreMinimal.h"
#include "Base/NetConnectionBase.h"


class FNetConnectionUDP : public FNetConnectionBase
{
public:
	virtual void Send(const TArray<uint8>& InData) override;
	virtual void Recv(const FGuid& InChannelGUID, TArray<uint8> InData) override;

	virtual void Verify() override;
	virtual void Analysis(uint8* InData, int32 BytesNum) override;

protected:

};


