#include "NetChannelBase.h"
#include "../DS_NetChannel.h"
#include "../Connection/Base/NetConnectionBase.h"
#include "../UObject/NetChannelController.h"
#include "IPAddress.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
FSimpleReturnDelegate FNetChannelBase::SimpleControllerDelegate;

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
	// 预分配策略，关闭只改变状态，不回收资源
	GUID = FGuid();
	//if (NetworkObject.IsValid())
	//{
	//	NetworkObject->Close();
	//	NetworkObject->MarkPendingKill();
	//	NetworkObject.Reset();
	//	NetworkObject = nullptr;
	//}
}

void FNetChannelBase::CloseConnect()
{
	if (Connection.IsValid())
	{
		Connection.Pin()->Close();
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


bool FNetChannelBase::Send(TArray<uint8>& InData)
{
	/*
	* 目前的发包策略是整包发送，缓冲区大小固定，当包大小大于缓冲区时会发送不全
	* 可以根据业务思考是否改变发送策略，一种方案是模仿TCP做拆包发送
	*/
	return Connection.IsValid() ? Connection.Pin()->Send(InData) : false;
}

bool FNetChannelBase::Recv(TArray<uint8>& InData)
{
	return (!MsgQueue.IsEmpty() && MsgQueue.Dequeue(InData)) ? true : false;
}

UNetChannelObject* FNetChannelBase::SpawnObject(UClass* InClass)
{
	return NewObject<UNetChannelObject>(InClass, InClass);
}

void FNetChannelBase::SpawnController()
{
	RegisterObject(SimpleControllerDelegate, UNetChannelController::StaticClass());
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
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif

void FNetChannelBase::InitController()
{
	if (NetworkObject.Get())
	{
		NetworkObject.Get()->Init();
	}
}

bool FNetChannelBase::GetChannelAddrInfo(FNetChannelAddrInfo& AddrInfo)
{
	if (Connection.IsValid())
	{
		Connection.Pin()->GetAddr()->GetIp(AddrInfo.Addr.IP);
		AddrInfo.Addr.Port = Connection.Pin()->GetAddr()->GetPort();
		AddrInfo.GUID = GUID;
		return true;
	}
	return false;
}

