#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "../UObject/Base/NetChannelObject.h"

class FNetConnectionBase;


class DEDICATEDSERVERUTILS_API FNetChannelBase
{
public:
	FNetChannelBase();
	virtual ~FNetChannelBase() {};

	virtual void Init();
	virtual void Close();

	void AddMsg(TArray<uint8>& InData);

	void Send(const TArray<uint8>& InData);
	bool Recv(TArray<uint8>& InData);

	void SpawnController();
	void SpawnPlayer();

protected:
	FGuid GUID;
	TWeakPtr<FNetConnectionBase> Connection;
	TArray<TArray<uint8>> MsgQueue;
	TStrongObjectPtr<UNetChannelObject> NetworkObject;

public:
	FORCEINLINE bool IsValid() const { return GUID != FGuid(); }
	//FORCEINLINE const uint32 GetConnectID() const { return Connection.Pin()->GetID(); }
	FORCEINLINE const FGuid GetGUID() const { return GUID; }
	FORCEINLINE void SetGUID(const FGuid& NewGUID) { GUID = NewGUID; }
	FORCEINLINE void SetConnection(TWeakPtr<FNetConnectionBase> InConnection) { Connection = InConnection; }
};
