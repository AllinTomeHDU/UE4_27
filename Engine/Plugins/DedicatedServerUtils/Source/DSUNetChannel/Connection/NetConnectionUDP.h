#pragma once

#include "CoreMinimal.h"
#include "Base/NetConnectionBase.h"


class FNetConnectionUDP : public FNetConnectionBase
{
public:
	virtual bool Send(TArray<uint8>& InData) override;
	virtual bool Recv(const FGuid& InChannelGUID, TArray<uint8>& InData) override;

	virtual void Verify() override;
	virtual void Analysis(TArray<uint8>& InData) override;

protected:

};


