#include "CenterController.h"
#include "CenterServer.h"
#include "DSUNetChannel/Core/NetChannelProtocols.h"
#include "DSUNetChannel/Connection/Base/NetConnectionBase.h"


void UCenterController::Init()
{
	Super::Init();

}

void UCenterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UCenterController::Close()
{
	Super::Close();
}

void UCenterController::RecvProtocol(uint32 InProtocol)
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
					CLIENT_SEND(CenterClient, P_Login, AddrInfo, UserID, UserName);
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
				FNetServerInfo CenterServerInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_LoginSuccess, GameAddrInfo, CenterServerInfo);

				// 若存在多个服务器分区，可在此处向客户端发送多服务器信息（地址、名称、状态）
				// 方案一让用户自己选择服务器分区，方案二根据服务器状态自动给用户分区（负载均衡）

				SERVER_SEND(CenterServer, GameAddrInfo, P_LoginSuccess, CenterServerInfo);
				break;
			}
			case P_LoginFailure:
			{
				FNetChannelAddrInfo AddrInfo;
				FString ErrorMsg;
				NETCHANNEL_PROTOCOLS_RECV(P_LoginFailure, AddrInfo, ErrorMsg);
				SERVER_SEND(CenterServer, AddrInfo, P_LoginFailure);
				break;
			}
		}
	}
}
