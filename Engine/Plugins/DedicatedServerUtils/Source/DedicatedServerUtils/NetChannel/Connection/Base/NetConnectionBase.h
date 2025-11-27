#pragma once

#include "CoreMinimal.h"
#include "../../Core/NetChannelType.h"
#include "../../Channel/NetChannelBase.h"

class ISocketSubsystem;
class FSocket;
class FInternetAddr;


// 一个 Connection 对应一个客户端
class FNetConnectionBase : public TSharedFromThis<FNetConnectionBase>
{
public:
	FNetConnectionBase();
	virtual ~FNetConnectionBase() {};

	static ISocketSubsystem* GetSocketSubsystem();

	virtual void Send(const TArray<uint8>& InData);
	virtual void Recv(const FGuid& InChannelGUID, TArray<uint8> InData);

	virtual void Init();
	virtual void Tick(float DeltaTime);

	virtual void Verify();
	virtual void Analysis(void* Data, int32 BytesNum);

	FNetChannelBase* GetMainChannel();
	void GetActiveChannelGUIDs(TArray<FGuid>& GUIDs);

protected:
	FSocket* Socket;
	TSharedPtr<FInternetAddr> LocalAddr;
	TArray<FNetChannelBase> Channels;
	ENetConnectionState Sate;

	uint32 ID;

	uint8 bLock : 1;

public:
	FORCEINLINE void SetSocket(FSocket* InSocket) { Socket = InSocket; }
	FORCEINLINE TSharedRef<FInternetAddr> GetAddr() const { return LocalAddr.ToSharedRef(); }

	FORCEINLINE void SetState(const ENetConnectionState InSate) { Sate = InSate; }
	FORCEINLINE ENetConnectionState GetState() const { return Sate; }

	FORCEINLINE const uint32 GetID() const { return ID; }
	FORCEINLINE void SetID(const uint32& InID) { ID = InID; }

	FORCEINLINE void Lock() { bLock = true; }
	FORCEINLINE void UnLock() { bLock = false; }
	FORCEINLINE bool IsLock() const { return bLock; }
};