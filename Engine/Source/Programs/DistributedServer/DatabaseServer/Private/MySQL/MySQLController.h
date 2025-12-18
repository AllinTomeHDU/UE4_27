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

public:
	bool Post(const FString& InSQL);
	bool Get(const FString& InSQL, TArray<FMySQL_FieldsData>& Results);

protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;
	virtual void RecvProtocol(uint32 InProtocol) override;

	void DealWithLoginRequest(const FNetChannelAddrInfo& GameAddrInfo, const FString& UserID, const FString& UserName);
	void SendHallServerInfo(const FNetChannelAddrInfo& GameAddrInfo);

private:
	TSharedPtr<FMySQL_Link> ReadLink;
	TSharedPtr<FMySQL_Link> WriteLink;
};