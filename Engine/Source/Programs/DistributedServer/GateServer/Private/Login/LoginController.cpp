#include "LoginController.h"
#include "GateServer.h"
#include "DSUNetChannel/Core/NetChannelProtocols.h"
#include "DSUNetChannel/Connection/Base/NetConnectionBase.h"


void ULoginController::Init()
{
	Super::Init();

}

void ULoginController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ULoginController::Close()
{
	Super::Close();
}

void ULoginController::RecvProtocol(uint32 InProtocol)
{
	//Super::RecvProtocol(InProtocol);

	if (GetLinkState() == ENetLinkState::Listen)
	{
		switch (InProtocol)
		{
			case P_Login:
			{
				FString UserID;
				FString UserName;
				NETCHANNEL_PROTOCOLS_RECV(P_Login, UserID, UserName);

				FNetChannelAddrInfo AddrInfo;
				if (GetChannelAddrInfo(AddrInfo))
				{
					CLIENT_SEND(DatabaseClient, P_Login, AddrInfo, UserID, UserName);
				}
				else
				{
					FString ErrorMsg = TEXT("GetChannelAddrInfo Error...");
					NETCHANNEL_PROTOCOLS_SEND(P_LoginFailure, ErrorMsg);
				}
				break;
			}
		}
	}
	else
	{
		switch (InProtocol)
		{
			case P_LoginSuccess:
			{
				FNetChannelAddrInfo GameAddrInfo;
				FNetServerInfo HallServerInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_LoginSuccess, GameAddrInfo, HallServerInfo);

				// 若存在多个服务器分区，可在此处向客户端发送多服务器信息（地址、名称、状态）
				// 方案一让用户自己选择服务器分区，方案二根据服务器状态自动给用户分区（负载均衡）

				SERVER_SEND(LoginServer, GameAddrInfo, P_LoginSuccess, HallServerInfo);
				break;
			}
			case P_LoginFailure:
			{
				FNetChannelAddrInfo AddrInfo;
				FString ErrorMsg;
				NETCHANNEL_PROTOCOLS_RECV(P_LoginFailure, AddrInfo, ErrorMsg);
				SERVER_SEND(LoginServer, AddrInfo, P_LoginFailure);
				break;
			}
		}
	}
}
