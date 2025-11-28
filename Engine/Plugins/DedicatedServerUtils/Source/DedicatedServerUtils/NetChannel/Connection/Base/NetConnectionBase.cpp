#include "NetConnectionBase.h"
#include "../../NetChannelGlobalInfo.h"
#include "../../Core/NetChannelType.h"
#include "../../Core/NetChannelProtocols.h"
#include "DedicatedServerUtils/Thread/ServerThreadManager.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
using namespace DedicatedServerUtils;


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
FNetConnectionBase::FNetConnectionBase()
	: Socket(nullptr)
	, State(ENetConnectionState::UnInit)
	, LastTime(0.0)
	, bMainListen(false)
	, bLock(false)
{
	Channels.SetNum(FNetChannelGlobalInfo::Get()->GetInfo().MaxChannels);	// 预分配

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

void FNetConnectionBase::Recv(const FGuid& InChannelGUID, TArray<uint8>& InData)
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

	if (!bMainListen && LinkState == ENetLinkState::Listen)
	{
		CheckTimeOut();
	}
}

void FNetConnectionBase::Close()
{
	for (auto& Tmp : Channels)
	{
		Tmp.Close();
	}
	State = ENetConnectionState::UnInit;
	UnLock();
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

	// 更新上层业务
	auto UpdateObject = [&]()
	{
		if (auto Channel = GetMainChannel())
		{
			if (BytesNum > sizeof(FNetBunchHead))
			{
				TArray<uint8> InNewData(InData, BytesNum);
				Channel->AddMsg(InNewData);
			}
			Channel->RecvProtocol(Head.ProtocolsNumber);
		}
	};

	// 心跳协议
	auto HeartBeat = [&]()
	{
		if (auto Channel = GetMainChannel())
		{
			NETCHANNEL_PROTOCOLS_SEND(P_HeartBeat);
		}
	};

	if (GetLinkState() == ENetLinkState::Listen)
	{
		switch (Head.ProtocolsNumber)
		{
		case P_HeartBeat:
			ResetHeartBeat();
			break;
		default:
			UpdateObject();
		}
	}
	else
	{
		switch (Head.ProtocolsNumber)
		{
		case P_Debug:
		default:
			UpdateObject();
		}
	}
}

FNetChannelBase* FNetConnectionBase::GetMainChannel()
{
	return Channels.IsValidIndex(0) ? &Channels[0] : nullptr;
}

void FNetConnectionBase::GetActiveChannelGUIDs(TArray<FGuid>& GUIDs)
{
	for (int i = 1; i < Channels.Num(); ++i)
	{
		if (Channels[i].IsValid())
		{
			GUIDs.Add(Channels[i].GetGUID());
		}
	}
}

void FNetConnectionBase::LoopHeartBeat()
{
	FNetChannelBase* Channel = GetMainChannel();
	if (Channel->IsValid())
	{
		FThreadManagement::Get()->GetCoroutines().BindLambda(
			3.f,
			[&]() 
			{ 
				NETCHANNEL_PROTOCOLS_SEND(P_HeartBeat); 
				LoopHeartBeat();
			}
		);
	}
}

void FNetConnectionBase::ResetHeartBeat()
{
	LastTime = FPlatformTime::Seconds();
}

void FNetConnectionBase::CheckTimeOut()
{
	double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastTime > 20.0)
	{
		Close();
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif


