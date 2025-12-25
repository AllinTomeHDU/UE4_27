#pragma once

#include "CoreMinimal.h"
#include "DS_NetChannel/UObject/NetChannelController.h"
#include "HallController.generated.h"


UCLASS()
class UHallController : public UNetChannelController
{
	GENERATED_BODY()

	struct FNetUserAssets
	{
		FNetUserAssets() : SpiritStone(0), ImmortalJade(0)
		{
			FMemory::Memset(Rank, 0, sizeof(Rank));
		}
		char Rank[20];
		uint32 SpiritStone;
		uint32 ImmortalJade;
	};
public:


protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;
	virtual void RecvProtocol(uint32 InProtocol) override;

private:
};