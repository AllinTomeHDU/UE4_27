#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "../UObject/Base/NetChannelObject.h"

class FNetConnectionBase;

DECLARE_DELEGATE_RetVal(UClass*, FSimpleReturnDelegate);


class DSUNETCHANNEL_API FNetChannelBase
{
public:
	static FSimpleReturnDelegate SimpleControllerDelegate;
	static FSimpleReturnDelegate SimplePlayerDelegate;

	FNetChannelBase();
	virtual ~FNetChannelBase() {};

	virtual void Init();
	virtual void Tick(float DeltaTime);
	virtual void Close();

	virtual void RecvProtocol(uint32 InProtocol);

	void AddMsg(TArray<uint8>& InData);

	void Send(const TArray<uint8>& InData);
	bool Recv(TArray<uint8>& InData);

	void SpawnController();
	void SpawnPlayer();

	void InitController();
	void InitPlayer();

	template<class T>
	T* GetNetObject()
	{
		return Cast<T>(NetworkObject.Get());
	}

protected:
	FGuid GUID;
	TWeakPtr<FNetConnectionBase> Connection;
	TQueue<TArray<uint8>> MsgQueue;

	TStrongObjectPtr<UNetChannelObject> NetworkObject;
	UNetChannelObject* SpawnObject(UClass* InClass);
	void RegisterObject(FSimpleReturnDelegate InDelegate, UClass* InObjectClass);

public:
	FORCEINLINE bool IsValid() const { return GUID != FGuid(); }
	FORCEINLINE const FGuid& GetGUID() const { return GUID; }
	FORCEINLINE void SetGUID(const FGuid& NewGUID) { GUID = NewGUID; }
	FORCEINLINE void SetConnection(TWeakPtr<FNetConnectionBase> InConnection) { Connection = InConnection; }
	FORCEINLINE TSharedPtr<FNetConnectionBase> GetConnection() const { return Connection.Pin(); }
};
