#include "NetConnectionBase.h"
#include "../../NetChannelGlobalInfo.h"
#include "../../Core/NetChannelType.h"
#include "../../Core/NetChannelProtocols.h"
#include "SocketSubsystem.h"
#include "Sockets.h"


FNetConnectionBase::FNetConnectionBase()
	: Socket(nullptr)
	, Sate(ENetConnectionState::UnInit)
	, bLock(false)
{
	Channels.SetNum(CHANNEL_NUM);	// 预分配

	if (auto Subsystem = GetSocketSubsystem())
	{
		LocalAddr = Subsystem->CreateInternetAddr();
	}
}

ISocketSubsystem* FNetConnectionBase::GetSocketSubsystem()
{
	return ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
}

void FNetConnectionBase::Send(const TArray<uint8>& InData)
{
}

void FNetConnectionBase::Recv(const FGuid& InChannelGUID, TArray<uint8> InData)
{
	for (auto& Tmp : Channels)
	{
		if (Tmp.IsValid() && Tmp.GetGUID() == InChannelGUID)
		{
			Tmp.AddMsg(InData);
			break;
		}
	}
}

void FNetConnectionBase::Init()
{
	for (auto& Tmp : Channels)
	{
		Tmp.SetConnection(this->AsShared());
	}
	if (auto MainChannel = GetMainChannel())
	{
		MainChannel->Init();
		MainChannel->SpawnController();
	}
}

void FNetConnectionBase::Tick(float DeltaTime)
{
	for (auto& Tmp : Channels)
	{
		if (Tmp.IsValid())
		{
			Tmp.Tick(DeltaTime);
		}
	}
}

void FNetConnectionBase::Verify()
{
	if (auto Channel = GetMainChannel())
	{
		FString Version = FNetChannelGlobalInfo::Get()->GetInfo().Version;

		NETCHANNEL_PROTOCOLS_SEND(P_Hello, Version);
		SetState(ENetConnectionState::Verify);
	}
}

void FNetConnectionBase::Analysis(uint8* InData, int32 BytesNum)
{
	FNetBunchHead Head = *(FNetBunchHead*)InData;
	if (auto Channel = GetMainChannel())
	{
		if (BytesNum > sizeof(FNetBunchHead))
		{
			TArray<uint8> InNewData(InData, BytesNum);
			Channel->AddMsg(InNewData);
		}
		Channel->RecvProtocol(Head.ProtocolsNumber);
	}
}

FNetChannelBase* FNetConnectionBase::GetMainChannel()
{
	return Channels.IsValidIndex(0) ? &Channels[0] : nullptr;
}

void FNetConnectionBase::GetActiveChannelGUIDs(TArray<FGuid>& GUIDs)
{
	for (auto& Tmp : Channels)
	{
		if (Tmp.IsValid())
		{
			GUIDs.Add(Tmp.GetGUID());
		}
	}
}



