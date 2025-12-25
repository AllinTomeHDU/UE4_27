#pragma once

#include "CoreMinimal.h"
#include "Definition/NetChannelType.h"

#ifdef PLATFORM_PROJECT
#include "Tickable.h"
#endif

class FNetConnectionBase;
class FInternetAddr;
class UNetChannelController;
class UNetChannelObject;
class FNetChannelBase;


class DS_NETCHANNEL_API FNetChannelManager
#ifdef PLATFORM_PROJECT
	: public FTickableGameObject
#endif
{
public:
	virtual ~FNetChannelManager() {};
	static FNetChannelManager* CreateNetChannelManager(ENetLinkState InState, ENetSocketType InType);
	static void Destroy(FNetChannelManager* InInstance);

	ENetConnectionState GetLocalConnectState() const;
	TSharedPtr<FNetConnectionBase> GetLocalConnection() const;
	TSharedPtr<FNetConnectionBase> GetRemoteConnection(const FNetAddr& InAddr);

	FNetChannelBase* GetLocalChannel();
	FNetChannelBase* GetRemoteChannel(const FNetChannelAddrInfo& AddrInfo);

	UNetChannelObject* GetNetChannelObject(const FNetChannelAddrInfo& AddrInfo);
	UNetChannelController* GetController();

	virtual bool Init(int32 InPort = INDEX_NONE);
	virtual bool Bind(const FNetAddr& InAddr);
	virtual void Tick(float DeltaTime);
	virtual void Close();

protected:
	virtual TStatId GetStatId() const { return TStatId(); }

	virtual void VerifyConnectionInfo(TSharedPtr<FNetConnectionBase> InConnection, TArray<uint8>& InData);

	struct FNetConnections
	{
		TSharedPtr<FNetConnectionBase> operator[](TSharedPtr<FInternetAddr> InternetAddr);
		TSharedPtr<FNetConnectionBase> GetEmptyConnection(TSharedPtr<FInternetAddr> InternetAddr);

		TSharedPtr<FNetConnectionBase> LocalConnection;
		TArray<TSharedPtr<FNetConnectionBase>> RemoteConnections;
	} Connections;

	ENetLinkState LinkState;
	bool bAsynchronous;

public:
	FORCEINLINE ENetLinkState GetLinkState() const { return LinkState; }
};
