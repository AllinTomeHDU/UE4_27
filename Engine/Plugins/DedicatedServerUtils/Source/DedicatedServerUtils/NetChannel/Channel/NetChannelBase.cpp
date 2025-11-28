#include "NetChannelBase.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"
#include "../Connection/Base/NetConnectionBase.h"
#include "../UObject/NetChannelController.h"
#include "../UObject/NetChannelPlayer.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
FSimpleReturnDelegate FNetChannelBase::SimpleControllerDelegate;
FSimpleReturnDelegate FNetChannelBase::SimplePlayerDelegate;

FNetChannelBase::FNetChannelBase()
{

}

void FNetChannelBase::Init()
{
	GUID = FGuid::NewGuid();
}

void FNetChannelBase::Tick(float DeltaTime)
{
	if (NetworkObject.IsValid())
	{
		NetworkObject->Tick(DeltaTime);
	}
}

void FNetChannelBase::Close()
{
	GUID = FGuid();
	if (NetworkObject.IsValid())
	{
		NetworkObject->Close();
		NetworkObject->MarkPendingKill();
		NetworkObject.Reset();
		NetworkObject = nullptr;
	}
}

void FNetChannelBase::RecvProtocol(uint32 InProtocol)
{
	if (NetworkObject.IsValid())
	{
		NetworkObject->RecvProtocol(InProtocol);
	}
}

void FNetChannelBase::AddMsg(TArray<uint8>& InData)
{
	MsgQueue.Enqueue(InData);
}


void FNetChannelBase::Send(const TArray<uint8>& InData)
{
	if (Connection.IsValid())
	{
		Connection.Pin()->Send(InData);
	}
	else
	{
		UE_LOG(LogDedicatedServerUtils, Display, TEXT("Channel Connection is not Valid"));
	}
}

bool FNetChannelBase::Recv(TArray<uint8>& InData)
{
	return (!MsgQueue.IsEmpty() && MsgQueue.Dequeue(InData)) ? true : false;
}

void FNetChannelBase::SpawnController()
{
	RegisterObject(SimpleControllerDelegate, UNetChannelController::StaticClass());
}

void FNetChannelBase::SpawnPlayer()
{
	RegisterObject(SimplePlayerDelegate, UNetChannelPlayer::StaticClass());
}

UNetChannelObject* FNetChannelBase::SpawnObject(UClass* InClass)
{
	return NewObject<UNetChannelObject>(InClass, InClass);
}

void FNetChannelBase::RegisterObject(FSimpleReturnDelegate InDelegate, UClass* InObjectClass)
{
	if (NetworkObject.IsValid())
	{
		Close();
	}

	if (InDelegate.IsBound())
	{
		if (UClass* InClass = InDelegate.Execute())
		{
			NetworkObject = TStrongObjectPtr<UNetChannelObject>(SpawnObject(InClass));
		}
		else
		{
			NetworkObject = TStrongObjectPtr<UNetChannelObject>(SpawnObject(InObjectClass));
		}
	}
	else
	{
		NetworkObject = TStrongObjectPtr<UNetChannelObject>(SpawnObject(InObjectClass));
	}

	if (NetworkObject.IsValid())
	{
		NetworkObject->LinkState = Connection.Pin()->GetLinkState();
		NetworkObject->Channel = this;
		NetworkObject->Init();
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif