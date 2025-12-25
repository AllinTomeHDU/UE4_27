#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Definition/NetChannelType.h"
#include "NetChannelObject.generated.h"

class FNetChannelBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FNetChannelJoinReturnDelegate, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FNetChannelRecvReturnDelegate, uint32, FNetChannelBase*);


UCLASS(Blueprintable)
class DS_NETCHANNEL_API UNetChannelObject : public UObject
{
	GENERATED_BODY()

	friend FNetChannelBase;
public:
	FNetChannelJoinReturnDelegate JoinDelegate;
	FNetChannelRecvReturnDelegate RecvDelegate;

	bool GetChannelAddrInfo(FNetChannelAddrInfo& AddrInfo);

protected:
	virtual void Init();
	virtual void Tick(float DeltaTime);
	virtual void Close();

	virtual void RecvProtocol(uint32 InProtocol);

	UPROPERTY()
	ENetLinkState LinkState;

	FNetChannelBase* Channel;

public:
	FORCEINLINE const ENetLinkState GetLinkState() const { return LinkState; }
	FORCEINLINE FNetChannelBase* GetChannel() const { return Channel; }
};