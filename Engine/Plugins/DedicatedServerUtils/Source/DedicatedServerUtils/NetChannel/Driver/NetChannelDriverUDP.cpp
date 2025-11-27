#include "NetChannelDriverUDP.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"
#include "DedicatedServerUtils/DedicatedServerUtils.h"
#include "DedicatedServerUtils/NetChannel/Connection/NetConnectionUDP.h"
#include "Sockets.h"
#include "SocketSubsystem.h"


FNetChannelDriverUDP::FNetChannelDriverUDP(const ENetLinkState InState)
{
	LinkState = InState;

	Connections.LocalConnection = MakeShareable(new FNetConnectionUDP());
}

bool FNetChannelDriverUDP::Init()
{
	if (Connections.LocalConnection)
	{
		auto SocketSubsystem = FNetConnectionBase::GetSocketSubsystem();
		if (!SocketSubsystem) return false;

		Socket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("UDP_TEST"));
		if (!Socket) return false;

		int32 RecvSize = 0, SendSize = 0;
		Socket->SetReceiveBufferSize(BUFFER_SIZE, RecvSize);
		Socket->SetSendBufferSize(BUFFER_SIZE, SendSize);

		switch (LinkState)
		{
			case ENetLinkState::Listen:  // 服务器
			{
				Connections.LocalConnection->GetAddr()->SetAnyAddress();
				Connections.LocalConnection->GetAddr()->SetPort(45678);

				// 若端口绑定失败，则根据端口增量重新设置端口，直至设置成功
				int32 BindPort = SocketSubsystem->BindNextPort(Socket, *Connections.LocalConnection->GetAddr(), 1, 1);
				if (BindPort == 0) return false;
				break;
			}
			case ENetLinkState::Connect:  // 客户端
			{
				bool bBindAddr = false;
				Connections.LocalConnection->GetAddr()->SetIp(TEXT("127.0.0.7"), bBindAddr);
				Connections.LocalConnection->GetAddr()->SetPort(45678);
				break;
			}

		}

		if (!Socket->SetNonBlocking()) return false;

		Connections.LocalConnection->SetSocket(Socket);
		Connections.LocalConnection->Init();

		if (LinkState == ENetLinkState::Listen)
		{
			for (int i = 0; i < CONNECT_NUM; ++i)	// 预分配
			{
				Connections.RemoteConnections.Add(MakeShareable(new FNetConnectionUDP()));
				TSharedPtr<FNetConnectionBase> NewConnection = Connections.RemoteConnections.Last();
				NewConnection->SetSocket(Socket);
				NewConnection->Init();
			}
			Connections.LocalConnection->SetState(ENetConnectionState::Join);
		}
		else
		{
			Connections.LocalConnection->Verify();
		}
	}

	return true;
}

void FNetChannelDriverUDP::Tick(float DeltaTime)
{
	if (!Socket || !Connections.LocalConnection.IsValid()) return;

	Connections.LocalConnection->Tick(DeltaTime);

	for (auto& Tmp : Connections.RemoteConnections)
	{
		Tmp->Tick(DeltaTime);
	}

	uint8 Data[BUFFER_SIZE] = { 0 };
	int32 BytesRead = 0;
	auto SocketSubsystem = FNetConnectionBase::GetSocketSubsystem();
	TSharedPtr<FInternetAddr> RemoteAddr = SocketSubsystem->CreateInternetAddr();
	if (Socket->RecvFrom(Data, BUFFER_SIZE, BytesRead, *RemoteAddr))
	{
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
