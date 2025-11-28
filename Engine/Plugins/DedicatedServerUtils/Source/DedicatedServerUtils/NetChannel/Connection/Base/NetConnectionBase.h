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
	virtual void Recv(const FGuid& InChannelGUID, TArray<uint8>& InData);

	virtual void Init();
	virtual void Tick(float DeltaTime);
	virtual void Close();

	virtual void Verify();
	virtual void Analysis(uint8* InData, int32 BytesNum);

	FNetChannelBase* GetMainChannel();
	void GetActiveChannelGUIDs(TArray<FGuid>& GUIDs);

	void LoopHeartBeat();
	void ResetHeartBeat();
	void CheckTimeOut();

protected:
	FSocket* Socket;
	TSharedPtr<FInternetAddr> LocalAddr;
	TArray<FNetChannelBase> Channels;

	ENetLinkState LinkState;
	ENetConnectionState State;

	double LastTime;
	uint8 bMainListen : 1;
	uint8 bLock : 1;

public:
	FORCEINLINE void SetSocket(FSocket* InSocket) { Socket = InSocket; }
	FORCEINLINE TSharedRef<FInternetAddr> GetAddr() const { return LocalAddr.ToSharedRef(); }

	FORCEINLINE void SetLinkState(const ENetLinkState InLinkState) { LinkState = InLinkState; }
	FORCEINLINE const ENetLinkState GetLinkState() const { return LinkState; }
	FORCEINLINE void SetState(const ENetConnectionState InSate) { State = InSate; }
	FORCEINLINE ENetConnectionState GetState() const { return State; }

	FORCEINLINE const bool GetIsMainListen() const { return bMainListen; }
	FORCEINLINE void SetIsMainListen(const bool bIsMain) { bMainListen = bIsMain; }

	FORCEINLINE void Lock() { bLock = true; }
	FORCEINLINE void UnLock() { bLock = false; }
	FORCEINLINE bool IsLock() const { return bLock; }
};