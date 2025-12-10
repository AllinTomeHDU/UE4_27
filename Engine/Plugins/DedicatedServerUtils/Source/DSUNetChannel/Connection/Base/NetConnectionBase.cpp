#include "NetConnectionBase.h"
#include "../../NetChannelGlobalInfo.h"
#include "../../Core/NetChannelType.h"
#include "../../Core/NetChannelProtocols.h"
#include "SocketSubsystem.h"
#include "Sockets.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
FNetConnectionBase::FNetConnectionBase()
	: Socket(nullptr)
	, State(ENetConnectionState::UnInit)
	, bMainListen(false)
	, HeartBeatWaitTime(0.f)
	, LastHeartBeatTime(0.f)
	, bLoopHeartBeat(false)
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

	// 心跳由客户端发送，服务端检测
	if (LinkState == ENetLinkState::Listen)
	{
		if (!bMainListen)
		{
			CheckTimeOut();	// 服务器远端连接检查心跳
		}
	}
	else
	{
		if (bLoopHeartBeat)
		{
			HeartBeatWaitTime += DeltaTime;
			if (HeartBeatWaitTime > FNetChannelGlobalInfo::Get()->GetInfo().HeartBeatTimeInterval)
			{
				HeartBeatWaitTime = 0.f;
				SendHeatBeat();
			}
		}
	}
}

void FNetConnectionBase::Close()
{
	for (auto& Tmp : Channels)
	{
		Tmp.Close();
	}
	State = ENetConnectionState::UnInit;
	bLoopHeartBeat = false;
	HeartBeatWaitTime = 0.f;
	UnLock();
}

void FNetConnectionBase::Verify()
{
	if (auto Channel = GetMainChannel())
	{
		FString Version = FNetChannelGlobalInfo::Get()->GetInfo().Version;

		NETCHANNEL_PROTOCOLS_SEND(P_Hello, Version);
		SetState(ENetConnectionState::Verify);
		ResetHeartBeat();
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

	if (GetLinkState() == ENetLinkState::Listen)
	{
		switch (Head.ProtocolsNumber)
		{
		case P_HeartBeat:
			ResetHeartBeat();
			UE_LOG(LogTemp, Display, TEXT("Recv Heat Beat"));
			break;
		case P_Close:
			Close();
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

void FNetConnectionBase::SendHeatBeat()
{
	if (auto Channel = GetMainChannel())
	{
		if (Channel->IsValid())
		{
			NETCHANNEL_PROTOCOLS_SEND(P_HeartBeat);
			UE_LOG(LogTemp, Display, TEXT("Send Heat Beat"));
		}
	}
}

void FNetConnectionBase::ResetHeartBeat()
{
	LastHeartBeatTime = FPlatformTime::Seconds();
}

void FNetConnectionBase::CheckTimeOut()
{
	double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastHeartBeatTime > FNetChannelGlobalInfo::Get()->GetInfo().HeatBeatTimeOutTime)
	{
		Close();
		UE_LOG(LogTemp, Display, TEXT("Connection timeout..."));
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif


