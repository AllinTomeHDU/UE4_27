#include "NetChannelDriverUDP.h"
#include "../DSUNetChannel.h"
#include "../NetChannelGlobalInfo.h"
#include "../Connection/NetConnectionUDP.h"
#include "DSUThreadPool/ServerThreadManager.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
using namespace DSUThreadPool;


FNetChannelDriverUDP::FNetChannelDriverUDP(const ENetLinkState InState)
{
	LinkState = InState;
	bAsynchronous = LinkState == ENetLinkState::Listen;
	bStopThread = false;

	Connections.LocalConnection = MakeShareable(new FNetConnectionUDP());
	Connections.LocalConnection->SetIsMainListen(true);
}

bool FNetChannelDriverUDP::Init()
{
	if (!FNetChannelManager::Init()) return false;

	if (Connections.LocalConnection)
	{
		auto SocketSubsystem = FNetConnectionBase::GetSocketSubsystem();
		if (!SocketSubsystem) return false;

		Socket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("UDP_TEST"));
		if (!Socket) return false;

		int32 RecvSize = 0, SendSize = 0;
		Socket->SetReceiveBufferSize(FNetChannelGlobalInfo::Get()->GetInfo().RecvDataSize, RecvSize);
		Socket->SetSendBufferSize(FNetChannelGlobalInfo::Get()->GetInfo().SendDataSize, SendSize);

		switch (LinkState)
		{
			case ENetLinkState::Listen:  // 服务器
			{
				Connections.LocalConnection->GetAddr()->SetAnyAddress();
				Connections.LocalConnection->GetAddr()->SetPort(FNetChannelGlobalInfo::Get()->GetInfo().Port);

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
				bool bBindAddr = false;
				Connections.LocalConnection->GetAddr()->SetIp(*FNetChannelGlobalInfo::Get()->GetInfo().URL, bBindAddr);
				Connections.LocalConnection->GetAddr()->SetPort(FNetChannelGlobalInfo::Get()->GetInfo().Port);
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
	uint8 Data[BUFFER_SIZE] = { 0 };
	int32 BytesRead = 0;
	auto SocketSubsystem = FNetConnectionBase::GetSocketSubsystem();
	TSharedPtr<FInternetAddr> RemoteAddr = SocketSubsystem->CreateInternetAddr();
	if (Socket->RecvFrom(Data, BUFFER_SIZE, BytesRead, *RemoteAddr))
	{
		if (BytesRead < sizeof(FNetBunchHead))
		{
			UE_LOG(LogDSUNetChannel, Display, TEXT("Recv a wrong data, size < head"));
			return;
		}
		if (LinkState == ENetLinkState::Listen)
		{
			if (auto Connect = Connections[RemoteAddr])
			{
				if (Connect->GetState() == ENetConnectionState::Join)
				{
					Connect->Analysis(Data, BytesRead);
				}
				else
				{
					VerifyConnectionInfo(Connect, Data, BytesRead);
				}
			}
			else
			{
				if (auto EmptyConnect = Connections.GetEmptyConnection(RemoteAddr))
				{
					VerifyConnectionInfo(EmptyConnect, Data, BytesRead);
				}
			}
		}
		else
		{
			if (Connections.LocalConnection->GetState() == ENetConnectionState::Join)
			{
				Connections.LocalConnection->Analysis(Data, BytesRead);
			}
			else
			{
				VerifyConnectionInfo(Connections.LocalConnection, Data, BytesRead);
			}
		}
	}
}

void FNetChannelDriverUDP::Close()
{
	FNetChannelManager::Close();

	if (auto Subsystem = FNetConnectionBase::GetSocketSubsystem())
	{
		Subsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
	bStopThread = true;
}