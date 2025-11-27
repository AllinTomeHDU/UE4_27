#pragma once

#include "CoreMinimal.h"
#include "Core/NetChannelType.h"

class FNetConnectionBase;
class FInternetAddr;


class DEDICATEDSERVERUTILS_API FNetChannelManager
{
	

public:
	static FNetChannelManager* CreateNetChannelManager(ENetLinkState InState, ENetSocketType InType);
	virtual ~FNetChannelManager() {};

	virtual bool Init();
	virtual void Tick(float DeltaTime);

protected:
	virtual TStatId GetStatId() const { return TStatId(); }

	virtual void VerifyConnectionInfo(TSharedPtr<FNetConnectionBase> InConnection, uint8* InData, int32 InBytesNum);

	struct FNetConnections
	{
		TSharedPtr<FNetConnectionBase> operator[](TSharedPtr<FInternetAddr> InternetAddr);
		//bool IsAddr(TSharedPtr<FInternetAddr> InternetAddr);
		//int32 Add(TSharedPtr<FInternetAddr> InternetAddr);
		void Close(int32 Index);
		TSharedPtr<FNetConnectionBase> GetEmptyConnection(TSharedPtr<FInternetAddr> InternetAddr);

		TSharedPtr<FNetConnectionBase> LocalConnection;
		TArray<TSharedPtr<FNetConnectionBase>> RemoteConnections;
		TArray<int32> AliveRemoveConnectionsIndex;
	}Connections;

	ENetLinkState LinkState;

public:
	FORCEINLINE ENetLinkState GetLinkState() const { return LinkState; }
};