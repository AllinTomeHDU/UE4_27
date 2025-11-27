#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Core/NetChannelType.h"
#include "NetChannelObject.generated.h"

class FNetChannelBase;

DECLARE_MULTICAST_DELEGATE_TwoParams(FSimpleNetRecvReturnDelegate, uint32, FNetChannelBase*);


UCLASS(Blueprintable)
class UNetChannelObject : public UObject
{
	GENERATED_BODY()

	friend FNetChannelBase;
public:
	FSimpleNetRecvReturnDelegate RecvDelegate;

	template<typename ...ParamTypes>
	void Send(uint32 InProtocol, ParamTypes& ...Params)
	{
		NETCHANNEL_PROTOCOLS_SEND(InProtocol, Params...);
	}

protected:
	virtual void Init();
	virtual void Tick(float DeltaTime);
	virtual void Close();

	virtual void RecvProtocol(uint32 InProtocol);

private:
	UPROPERTY()
	ENetLinkState LinkState;

	FNetChannelBase* Channel;

public:
	FORCEINLINE const ENetLinkState GetLinkState() const { return LinkState; }
};