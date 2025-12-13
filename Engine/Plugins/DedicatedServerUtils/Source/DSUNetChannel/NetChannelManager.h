#pragma once

#include "CoreMinimal.h"
#include "Core/NetChannelType.h"

#ifdef PLATFORM_PROJECT
#include "Tickable.h"
#endif

class FNetConnectionBase;
class FInternetAddr;
class UNetChannelController;


class DSUNETCHANNEL_API FNetChannelManager
#ifdef PLATFORM_PROJECT
	: public FTickableGameObject
#endif
{
public:
	virtual ~FNetChannelManager() {};
	static FNetChannelManager* CreateNetChannelManager(ENetLinkState InState, ENetSocketType InType);
	static void Destroy(FNetChannelManager* InInstance);

	UNetChannelController* GetController();

	virtual bool Init(int32 InPort = INDEX_NONE);
	virtual void Tick(float DeltaTime);
	virtual void Close();

protected:
	virtual TStatId GetStatId() const { return TStatId(); }

	virtual void VerifyConnectionInfo(TSharedPtr<FNetConnectionBase> InConnection, uint8* InData, int32 BytesNum);

	struct FNetConnections
	{
		TSharedPtr<FNetConnectionBase> operator[](TSharedPtr<FInternetAddr> InternetAddr);
		void Close(int32 Index);
		TSharedPtr<FNetConnectionBase> GetEmptyConnection(TSharedPtr<FInternetAddr> InternetAddr);

		TSharedPtr<FNetConnectionBase> LocalConnection;
		TArray<TSharedPtr<FNetConnectionBase>> RemoteConnections;
		TArray<int32> AliveRemoveConnectionsIndex;
	} Connections;

	ENetLinkState LinkState;
	bool bAsynchronous;
public:
	FORCEINLINE ENetLinkState GetLinkState() const { return LinkState; }
	ENetConnectionState GetLocalConnectState() const;
};