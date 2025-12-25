#pragma once

#include "CoreMinimal.h"
#include "DS_NetChannel/UObject/NetChannelController.h"
#include "MySQL/Link/MySQL_LinkType.h"
#include "MySQLController.generated.h"

struct FMySQL_Link;


UCLASS()
class UMySQLController : public UNetChannelController
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
	bool Post(const FString& InSQL);
	bool Get(const FString& InSQL, TArray<FMySQL_FieldsData>& Results);

protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;
	virtual void RecvProtocol(uint32 InProtocol) override;

	void DealWithRegisterRequest(const FNetChannelAddrInfo& GameAddrInfo, const FString& Account, const FString& Password,
								 const FString& Name, const FString& Country, const FString& Platform);

	void DealWithLoginRequest(const FNetChannelAddrInfo& GameAddrInfo, const FString& Account, const FString& Password);
	void SendHallServerInfo(const FNetChannelAddrInfo& GameAddrInfo);

	void DealWithUserAssetsRequest(const FNetChannelAddrInfo& GameAddrInfo, const FString& Account);
	void SendUserAssets(const FNetChannelAddrInfo& GameAddrInfo, const TArray<FMySQL_FieldsData>& Results);

private:
	TSharedPtr<FMySQL_Link> ReadLink;
	TSharedPtr<FMySQL_Link> WriteLink;
};