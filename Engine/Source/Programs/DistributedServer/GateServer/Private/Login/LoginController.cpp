#include "LoginController.h"
#include "GateServer.h"
#include "DS_NetChannel/Definition/NetChannelProtocols.h"
#include "DS_NetChannel/Connection/Base/NetConnectionBase.h"


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
			case P_Register:
			{
				FString Account, Password, Name, Country, Platform;
				NETCHANNEL_PROTOCOLS_RECV(P_Register, Account, Password, Name, Country, Platform);

				FNetChannelAddrInfo AddrInfo;
				if (GetChannelAddrInfo(AddrInfo))
				{
					CLIENT_SEND(DatabaseClient, P_Register, AddrInfo, Account, Password, Name, Country, Platform);
				}
				else
				{
					FString ErrorMsg = TEXT("GetChannelAddrInfo Error...");
					NETCHANNEL_PROTOCOLS_SEND(P_RegisterFailure, ErrorMsg);
				}
				break;
			}
			case P_Login:
			{
				FString Account, Password;
				NETCHANNEL_PROTOCOLS_RECV(P_Login, Account, Password);

				FNetChannelAddrInfo AddrInfo;
				if (GetChannelAddrInfo(AddrInfo))
				{
					CLIENT_SEND(DatabaseClient, P_Login, AddrInfo, Account, Password);
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
			/*
			* 注册
			*/
			case P_RegisterSuccess:
			{
				FNetChannelAddrInfo GameAddrInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_RegisterSuccess, GameAddrInfo);
				SERVER_SEND(LoginServer, GameAddrInfo, P_RegisterSuccess);
				break;
			}
			case P_RegisterFailure:
			{
				FNetChannelAddrInfo GameAddrInfo;
				FString ErrorMsg;
				NETCHANNEL_PROTOCOLS_RECV(P_RegisterFailure, GameAddrInfo, ErrorMsg);
				SERVER_SEND(LoginServer, GameAddrInfo, P_RegisterFailure, ErrorMsg);
				break;
			}
			case P_AccountAlreadyExits:
			{
				FNetChannelAddrInfo GameAddrInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_AccountAlreadyExits, GameAddrInfo);
				SERVER_SEND(LoginServer, GameAddrInfo, P_AccountAlreadyExits);
				break;
			}

			/*
			* 登录
			*/
			case P_LoginSuccess:
			{
				FNetChannelAddrInfo GameAddrInfo;
				FNetServerInfo HallServerInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_LoginSuccess, GameAddrInfo, HallServerInfo);

				// 若存在多个服务器分区，可在此处向客户端发送多服务器信息（地址、名称、状态）
				// 方案一让用户自己选择服务器分区，方案二根据服务器状态自动给用户分区（负载均衡）

				SERVER_SEND(LoginServer, GameAddrInfo, P_LoginSuccess, HallServerInfo);
				if (auto Connect = LoginServer->GetRemoteConnection(GameAddrInfo.Addr))
				{
					Connect->Close();
				}
				break;
			}
			case P_LoginFailure:
			{
				FNetChannelAddrInfo AddrInfo;
				FString ErrorMsg;
				NETCHANNEL_PROTOCOLS_RECV(P_LoginFailure, AddrInfo, ErrorMsg);
				SERVER_SEND(LoginServer, AddrInfo, P_LoginFailure, ErrorMsg);
				break;
			}
			case P_AbsentAccount:
			{
				FNetChannelAddrInfo AddrInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_AbsentAccount, AddrInfo);
				SERVER_SEND(LoginServer, AddrInfo, P_AbsentAccount);
				break;
			}
			case P_AbnormalAccount:
			{
				FNetChannelAddrInfo AddrInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_AbnormalAccount, AddrInfo);
				SERVER_SEND(LoginServer, AddrInfo, P_AbnormalAccount);
				break;
			}
			case P_IncorrectPassword:
			{
				FNetChannelAddrInfo AddrInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_IncorrectPassword, AddrInfo);
				SERVER_SEND(LoginServer, AddrInfo, P_IncorrectPassword);
				break;
			}
			case P_VerificationError:
			{
				FNetChannelAddrInfo AddrInfo;
				NETCHANNEL_PROTOCOLS_RECV(P_VerificationError, AddrInfo);
				SERVER_SEND(LoginServer, AddrInfo, P_VerificationError);
				break;
			}
		}
	}
}
