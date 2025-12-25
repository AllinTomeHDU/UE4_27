#include "NetChannelDriverUDP.h"
#include "../DS_NetChannel.h"
#include "../NetChannelGlobalInfo.h"
#include "../Definition/NetChannelEncryption.h"
#include "../Connection/NetConnectionUDP.h"
#include "DS_ThreadPool/ServerThreadManager.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
using namespace DS_ThreadPool;

#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
FNetChannelDriverUDP::FNetChannelDriverUDP(const ENetLinkState InState)
{
	LinkState = InState;
	bAsynchronous = LinkState == ENetLinkState::Listen;
	bStopThread = false;
}

bool FNetChannelDriverUDP::Init(int32 InPort)
{
	if (!FNetChannelManager::Init(InPort)) return false;

	FString IpStr = FNetChannelGlobalInfo::Get()->GetInfo().URL;
	uint32 Port = InPort == INDEX_NONE ? FNetChannelGlobalInfo::Get()->GetInfo().Port : InPort;
	return Bind(FNetAddr(IpStr, Port));
}

bool FNetChannelDriverUDP::Bind(const FNetAddr& InAddr)
{
	if (!FNetChannelManager::Bind(InAddr)) return false;

	Connections.LocalConnection = MakeShareable(new FNetConnectionUDP());
	Connections.LocalConnection->SetNetManager(this);
	Connections.LocalConnection->SetIsMainListen(true);

	if (Connections.LocalConnection)
	{
		auto SocketSubsystem = FNetConnectionBase::GetSocketSubsystem();
		if (!SocketSubsystem) return false;

		if (Socket)
		{
			SocketSubsystem->DestroySocket(Socket);
			Socket = nullptr;
		}

		Socket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("UDP_Socket"));
		if (!Socket) return false;

		int32 RecvSize = 0, SendSize = 0;
		Socket->SetReceiveBufferSize(FNetChannelGlobalInfo::Get()->GetInfo().RecvDataSize, RecvSize);
		Socket->SetSendBufferSize(FNetChannelGlobalInfo::Get()->GetInfo().SendDataSize, SendSize);

		switch (LinkState)
		{
			case ENetLinkState::Listen:  // 服务器
			{
				Connections.LocalConnection->GetAddr()->SetAnyAddress();
				Connections.LocalConnection->GetAddr()->SetPort(InAddr.Port);

				// 若端口绑定失败，则根据端口增量重新设置端口，直至设置成功
				int32 BindPort = SocketSubsystem->BindNextPort(Socket, *Connections.LocalConnection->GetAddr(), 1, 1);
				if (BindPort == 0)
				{
					SocketSubsystem->DestroySocket(Socket);
					return false;
				}
				break;
			}
			case ENetLinkState::Connect:  // 客户端
			{
				Connections.LocalConnection->GetAddr()->SetIp(InAddr.IP);
				Connections.LocalConnection->GetAddr()->SetPort(InAddr.Port);
				break;
			}
		}

		if (!bAsynchronous)
		{
			if (!Socket->SetNonBlocking())
			{
				SocketSubsystem->DestroySocket(Socket);
				return false;
			}
		}

		Connections.LocalConnection->SetSocket(Socket);
		Connections.LocalConnection->SetLinkState(LinkState);
		Connections.LocalConnection->Init();
		Connections.LocalConnection->GetMainChannel()->InitController();

		if (LinkState == ENetLinkState::Listen)
		{
			for (int i = 0; i < FNetChannelGlobalInfo::Get()->GetInfo().MaxConnections; ++i)	// 预分配
			{
				Connections.RemoteConnections.Add(MakeShareable(new FNetConnectionUDP()));
				TSharedPtr<FNetConnectionBase> NewConnection = Connections.RemoteConnections.Last();
				NewConnection->SetSocket(Socket);
				NewConnection->SetLinkState(LinkState);
				NewConnection->Init();
			}
			Connections.LocalConnection->SetState(ENetConnectionState::Join);
		}
		else
		{
			Connections.LocalConnection->Verify();
		}
	}

	// 异步线程，创建之后立马执行
	if (bAsynchronous)
	{
		FThreadManagement::Get()->GetProxy().CreateRaw(this, &FNetChannelDriverUDP::RunThread);
	}

	return true;
}

void FNetChannelDriverUDP::Tick(float DeltaTime)
{
	FNetChannelManager::Tick(DeltaTime);

	if (!Socket || !Connections.LocalConnection.IsValid()) return;

	if (Connections.LocalConnection->GetState() == ENetConnectionState::Join)
	{
		Connections.LocalConnection->Tick(DeltaTime);
	}

	for (auto& Tmp : Connections.RemoteConnections)
	{
		if (Tmp->GetState() == ENetConnectionState::Join)
		{
			Tmp->Tick(DeltaTime);
		}
	}

	if (!bAsynchronous)
	{
		Listen();
	}
}

void FNetChannelDriverUDP::RunThread()
{
	while (!bStopThread)
	{
		Listen();
	}
}

void FNetChannelDriverUDP::Listen()
{
	uint8 RecvData[BUFFER_SIZE] = { 0 };
	int32 BytesRead = 0;
	auto SocketSubsystem = FNetConnectionBase::GetSocketSubsystem();
	TSharedPtr<FInternetAddr> RemoteAddr = SocketSubsystem->CreateInternetAddr();
	if (Socket->RecvFrom(RecvData, BUFFER_SIZE, BytesRead, *RemoteAddr))
	{
		if (BytesRead < sizeof(FNetBunchHead))
		{
			UE_LOG(LogDS_NetChannel, Display, TEXT("Recv Warning: BytesRead < sizeof(FNetBunchHead)"));
			return;
		}

		TArray<uint8> Data(RecvData, BytesRead);
		FNetChannelEncryption::XorEncryptWithKey(Data);

		FNetBunchHead Head = *(FNetBunchHead*)Data.GetData();
		if (LinkState == ENetLinkState::Listen)
		{
			UE_LOG(LogDS_NetChannel, Display, TEXT("Server Recv A Protocol from [%s][%d]: %d, size=%d"),
				*RemoteAddr->ToString(false), RemoteAddr->GetPort(), Head.ProtocolsNumber, BytesRead);

			if (auto Connect = Connections[RemoteAddr])
			{
				if (Connect->GetState() == ENetConnectionState::Join)
				{
					Connect->Analysis(Data);
				}
				else
				{
					VerifyConnectionInfo(Connect, Data);
				}
			}
			else
			{
				if (auto EmptyConnect = Connections.GetEmptyConnection(RemoteAddr))
				{
					VerifyConnectionInfo(EmptyConnect, Data);
				}
			}
		}
		else
		{
			UE_LOG(LogDS_NetChannel, Display, TEXT("Client Recv A Protocol from [%s][%d]: %d, size=%d"),
				*RemoteAddr->ToString(false), RemoteAddr->GetPort(), Head.ProtocolsNumber, BytesRead);

			if (Connections.LocalConnection->GetState() == ENetConnectionState::Join)
			{
				Connections.LocalConnection->Analysis(Data);
			}
			else
			{
				VerifyConnectionInfo(Connections.LocalConnection, Data);
			}
		}
	}
}

void FNetChannelDriverUDP::Close()
{
	FNetChannelManager::Close();

	if (Socket)
	{
		if (auto Subsystem = FNetConnectionBase::GetSocketSubsystem())
		{
			Subsystem->DestroySocket(Socket);
			Socket = nullptr;
		}
	}
	bStopThread = true;
}

#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif
